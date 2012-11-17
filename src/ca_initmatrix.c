

#include <stdio.h>
#include <math.h>

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

long random();

int max(int x,int y);
int min(int x,int y);
int irnd(int max);
double dmin(double a,double b);
double dmax3(double x, double y, double z);
double dist(int x1, int y1, int x2, int y2, int max);


void InitMatrixRandom(void)
    {
    int  i,j;
    long r;

    current = 0;
    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            matrix[0][i][j] = irnd(MAXV+1);
            if (matrix[0][i][j] > MAXV)
                printf("Hey!  This don't work!\n");
            }
    }


void InitMatrixRandom2(void)
    {
    int  i,j;
    int  x1,y1,x2,y2,x3,y3,x4,y4;
    double m;


    x1 = DIM/4 + irnd(DIM/2);
    y1 = DIM/4 + irnd(DIM/2);
    x2 = DIM/4 + irnd(DIM/2);
    y2 = DIM/4 + irnd(DIM/2);
    x3 = DIM/4 + irnd(DIM/2);
    y3 = DIM/4 + irnd(DIM/2);
    x4 = irnd(DIM);
    y4 = irnd(DIM);    
    m = sqrt(2.0)*0.5*DIM;    
    current = 0;
    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            double d,d1,d2,d3,d4;
            d1 = dist(i,j,x1,y1,DIM)/m;
            d2 = dist(i,j,x2,y2,DIM)/m;
            d3 = dist(i,j,x3,y3,DIM)/m;
            d4 = dist(i,j,x4,y4,DIM)/m;
            d = dmin(d1,dmin(d2,dmin(d3,d4)));
            d = 1.0/(1.0+10*d);
            matrix[0][i][j] = (int) (MAXV*d) + irnd(10);
            if (matrix[0][i][j] > MAXV)
                matrix[0][i][j] = MAXV;
            }
    }

void InitMatrixRandom3(void)
    {
    int  k,i,j;

    current = 0;
    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            matrix[0][i][j] = 0;
            }
    for (k = 0; k < MAXV-25; ++k)
        {
        int x1,y1,width,height,v;
        x1 = irnd(DIM);
        y1 = irnd(DIM);
        width  = irnd(DIM/2);
        height = irnd(DIM/2);
        v = irnd(MAXV-5);
        for (i = x1; i < x1+width; ++i)
            for (j = y1; j < y1+height; ++j)
                {
                int ii,jj;
                ii = i % DIM;
                jj = j % DIM;
                matrix[0][ii][jj] = v + irnd(5);
                }
        }
    }

void InitMatrix1(void)
    {
    int  i,j;

    current = 0;
    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            double a;
            a = sqrt((double)((i-DIM/2)*(i-DIM/2)+(j-DIM/2)*(j-DIM/2)));
            if ((j > DIM/2) || a > DIM/4.0 || a < DIM/12.0)
                matrix[0][i][j] = 0;
            else
                matrix[0][i][j] = MAXV - (int) (MAXV*(a-DIM/12.0)/(DIM/4.0-DIM/12.0));
            }
    }

void InitMatrix2(void)
    {
    int  i,j;

    current = 0;
    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            double a;
            a = sqrt((double)((i-DIM/2)*(i-DIM/2)+(j-DIM/2)*(j-DIM/2)));
            if (a > (double) DIM/4.0)
                matrix[0][i][j] = 0;
            else
                matrix[0][i][j] = (int) (MAXV*a*4.0/DIM);
            }
    }

void InitMatrix3(void)
    {
    int  i,j;

    current = 0;
    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            double a,theta1,theta2;
            theta1 = 3 * 2.0*3.14159*((double) i/DIM);
            theta2 = 3 * 2.0*3.14159*((double) j/DIM);
            a = sin(theta1)*sin(theta2)+1.0;
            matrix[0][i][j] = (int) (MAXV*a/2.0);
            }
    }

void InitMatrix4(void)
    {
    int  i,j;

    current = 0;
    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            double a,r,theta1,theta2;
            r = sqrt((double)((i-DIM/2)*(i-DIM/2)+(j-DIM/2)*(j-DIM/2)));
            if (r > DIM/4.0)
                r = 0.0;
            theta1 = 3 * 2.0*3.14159*((double) i/DIM);
            theta2 = 2 * 2.0*3.14159*(4*r/DIM);
            a = sin(theta1)*cos(theta2)+1.0;
            matrix[0][i][j] = (int) (MAXV*a/2.0);
            }
    }

void InitMatrix5(void)
    {
    int  i,j;

    current = 0;
    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            matrix[0][i][j] = (i%(MAXV)+j%(MAXV))/2;
            }
    }
