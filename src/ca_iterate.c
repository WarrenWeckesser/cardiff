
#include "ca.h"

/*
 * Global (ack!) variables...
 */

extern int  current;
extern unsigned char matrix[2][DIM][DIM];
extern char I[DIM][DIM];
extern char K[DIM][DIM];
extern int  S[DIM][DIM];

extern int  g;
extern int  k1;
extern int  k2;

int max(int x,int y);
int min(int x,int y);
int irnd(int max);
double dmin(double a,double b);
double dmax3(double x, double y, double z);
double dist(int x1, int y1, int x2, int y2, int max);

        
void Iterate(void)
    {
    int i,j;

    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            K[i][j] = 0;
            I[i][j] = 0;
            S[i][j] = 0;
            }
    for (i = 1; i < DIM-1; ++i)
        for (j = 1; j < DIM-1; ++j)
            {
            if (matrix[current][i][j] == MAXV)
                {
                ++K[i-1][j-1]; ++K[i][j-1]; ++K[i+1][j-1];
                ++K[i-1][j];   ++K[i][j];   ++K[i+1][j];
                ++K[i-1][j+1]; ++K[i][j+1]; ++K[i+1][j+1];
                }
            else if (matrix[current][i][j] > 0)
                {
                int s;
                ++I[i-1][j-1]; ++I[i][j-1]; ++I[i+1][j-1];
                ++I[i-1][j];   ++I[i][j];   ++I[i+1][j];
                ++I[i-1][j+1]; ++I[i][j+1]; ++I[i+1][j+1];
                s = matrix[current][i][j];
                S[i-1][j-1] += s;  S[i][j-1] += s; S[i+1][j-1] += s;
                S[i-1][j]   += s;  S[i][j]   += s; S[i+1][j]   += s;
                S[i-1][j+1] += s;  S[i][j+1] += s; S[i+1][j+1] += s;
                }
            }
    for (i = 1; i < DIM-1; ++i)
        {
        /*  Left side  */
        if (matrix[current][i][0] == MAXV)
            {
            ++K[i-1][0]; ++K[i][0]; ++K[i+1][0];
            ++K[i-1][1]; ++K[i][1]; ++K[i+1][1];
            }
        else if (matrix[current][i][0] > 0)
            {
            int s;
            ++I[i-1][0]; ++I[i][0]; ++I[i+1][0];
            ++I[i-1][1]; ++I[i][1]; ++I[i+1][1];
            s = matrix[current][i][j];
            S[i-1][0] += s; S[i][0] += s; S[i+1][0] += s;
            S[i-1][1] += s; S[i][1] += s; S[i+1][1] += s;
            }
        /* Right side */
        if (matrix[current][i][DIM-1] == MAXV)
            {
            ++K[i-1][DIM-2]; ++K[i][DIM-2]; ++K[i+1][DIM-2];
            ++K[i-1][DIM-1]; ++K[i][DIM-1]; ++K[i+1][DIM-1];
            }
        else if (matrix[current][i][DIM-1] > 0)
            {
            int s;
            ++I[i-1][DIM-2]; ++I[i][DIM-2]; ++I[i+1][DIM-2];
            ++I[i-1][DIM-1]; ++I[i][DIM-1]; ++I[i+1][DIM-1];
            s = matrix[current][i][j];
            S[i-1][DIM-2] += s; S[i][DIM-2] += s; S[i+1][DIM-2] += s;
            S[i-1][DIM-1] += s; S[i][DIM-1] += s; S[i+1][DIM-1] += s;
            }
        /* Top row */
        if (matrix[current][0][i] == MAXV)
            {
            ++K[0][i-1]; ++K[0][i]; ++K[0][i+1];
            ++K[1][i-1]; ++K[1][i]; ++K[1][i+1];
            }
        else if (matrix[current][0][i] > 0)
            {
            int s;
            ++I[0][i-1]; ++I[0][i]; ++I[0][i+1];
            ++I[1][i-1]; ++I[1][i]; ++I[1][i+1];
            s = matrix[current][i][j];
            S[0][i-1] += s; S[0][i] += s; S[0][i+1] += s;
            S[1][i-1] += s; S[1][i] += s; S[1][i+1] += s;
            }
        /* Bottom row */
        if (matrix[current][DIM-1][i] == MAXV)
            {
            ++K[DIM-2][i-1]; ++K[DIM-2][i]; ++K[DIM-2][i+1];
            ++K[DIM-1][i-1]; ++K[DIM-1][i]; ++K[DIM-1][i+1];
            }
        else if (matrix[current][DIM-1][i] > 0)
            {
            int s;
            ++I[DIM-2][i-1]; ++I[DIM-2][i]; ++I[DIM-2][i+1];
            ++I[DIM-1][i-1]; ++I[DIM-1][i]; ++I[DIM-1][i+1];
            s = matrix[current][i][j];
            S[DIM-2][i-1] += s; S[DIM-2][i] += s; S[DIM-2][i+1] += s;
            S[DIM-1][i-1] += s; S[DIM-1][i] += s; S[DIM-1][i+1] += s;
            }
        }
    /* UL corner */
    if (matrix[current][0][0] == MAXV)
        {
        ++K[0][0]; ++K[0][1];
        ++K[1][0]; ++K[1][1];
        }
    else if (matrix[current][0][0] > 0)
        {
        int s;
        ++I[0][0]; ++I[0][1];
        ++I[1][0]; ++I[1][1];
        s = matrix[current][0][0];
        S[0][0] += s; S[0][1] += s;
        S[1][0] += s; S[1][1] += s;
        }
    /* UR corner */
    if (matrix[current][0][DIM-1] == MAXV)
        {
        ++K[0][DIM-2]; ++K[0][DIM-1];
        ++K[1][DIM-2]; ++K[1][DIM-1];
        }
    else if (matrix[current][0][DIM-1] > 0)
        {
        int s;
        ++I[0][DIM-2]; ++I[0][DIM-1];
        ++I[1][DIM-2]; ++I[1][DIM-1];
        s = matrix[current][0][DIM-1];
        S[0][DIM-2] += s; S[0][DIM-1] += s;
        S[1][DIM-2] += s; S[1][DIM-1] += s;
        }
    /* LR corner */
    if (matrix[current][DIM-1][DIM-1] == MAXV)
        {
        ++K[DIM-2][DIM-2]; ++K[DIM-2][DIM-1];
        ++K[DIM-1][DIM-2]; ++K[DIM-1][DIM-1];
        }
    else if (matrix[current][DIM-1][DIM-1] > 0)
        {
        int s;
        ++I[DIM-2][DIM-2]; ++I[DIM-2][DIM-1];
        ++I[DIM-1][DIM-2]; ++I[DIM-1][DIM-1];
        s = matrix[current][DIM-1][DIM-1];
        S[DIM-2][DIM-2] += s; S[DIM-2][DIM-1] += s;
        S[DIM-1][DIM-2] += s; S[DIM-1][DIM-1] += s;
        }
    /* LL corner */
    if (matrix[current][DIM-1][0] == MAXV)
        {
        ++K[DIM-2][0]; ++K[DIM-2][1];
        ++K[DIM-1][0]; ++K[DIM-1][1];
        }
    else if (matrix[current][DIM-1][DIM-1] > 0)
        {
        int s;
        ++I[DIM-2][0]; ++I[DIM-2][1];
        ++I[DIM-1][0]; ++I[DIM-1][1];
        s = matrix[current][DIM-1][DIM-1];
        S[DIM-2][0] += s; S[DIM-2][1] += s;
        S[DIM-1][0] += s; S[DIM-1][1] += s;
        }

    int next = 1 - current;
    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            if (matrix[current][i][j] == MAXV)
                matrix[next][i][j] = 0;
            else
                {
                if (matrix[current][i][j] == 0)
                    matrix[next][i][j] = (int)((float)K[i][j]/k1 + (float)I[i][j]/k2);
/*
                    matrix[next][i][j] = K[i][j]/k1 + I[i][j]/k2;
*/
                else
                    matrix[next][i][j] = min((int)((float)S[i][j]/I[i][j] + g), MAXV);
                }
            }
    current = next;
    }
        
void IterateTorus(void)
    {
    int i,j;

    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            K[i][j] = 0;
            I[i][j] = 0;
            S[i][j] = 0;
            }
    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            int im1,ip1,jm1,jp1;
            im1 = (i==0)?(DIM-1):(i-1);
            ip1 = (i==DIM-1)?(0):(i+1);
            jm1 = (j==0)?(DIM-1):(j-1);
            jp1 = (j==DIM-1)?(0):(j+1);
            if (matrix[current][i][j] == MAXV)
                {
                ++K[im1][jm1]; ++K[i][jm1]; ++K[ip1][jm1];
                ++K[im1][j];   ++K[i][j];   ++K[ip1][j];
                ++K[im1][jp1]; ++K[i][jp1]; ++K[ip1][jp1];
                }
            else if (matrix[current][i][j] > 0)
                {
                int s;
                ++I[im1][jm1]; ++I[i][jm1]; ++I[ip1][jm1];
                ++I[im1][j];   ++I[i][j];   ++I[ip1][j];
                ++I[im1][jp1]; ++I[i][jp1]; ++I[ip1][jp1];
                s = matrix[current][i][j];
                S[im1][jm1] += s;  S[i][jm1] += s; S[ip1][jm1] += s;
                S[im1][j]   += s;  S[i][j]   += s; S[ip1][j]   += s;
                S[im1][jp1] += s;  S[i][jp1] += s; S[ip1][jp1] += s;
                }
            }

    int next = 1 - current;
    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            if (matrix[current][i][j] == MAXV)
                matrix[next][i][j] = 0;
            else
                {
                if (matrix[current][i][j] == 0)
                    matrix[next][i][j] = (int)((float)K[i][j]/k1 + (float)I[i][j]/k2);
/*
                    matrix[next][i][j] = K[i][j]/k1 + I[i][j]/k2;
*/
                else
                    matrix[next][i][j] = min((int)((float)S[i][j]/I[i][j] + g), MAXV);
                }
            }
    current = next;
    }
