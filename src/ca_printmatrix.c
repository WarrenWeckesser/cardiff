#include <stdio.h>

#include "ca.h"

/*
 * Global (ack!) variables...
 */

extern int  current;
extern unsigned char matrix[2][DIM][DIM];
extern char I[DIM][DIM];
extern char K[DIM][DIM];
extern int  S[DIM][DIM];


void PrintMatrix(void)
    {
    int i,j;

    for (i = 0; i < DIM; ++i)
        {
        for (j = 0; j < DIM; ++j)
            {
            printf("%3d ",matrix[current][i][j]);
            }
        printf("\n");
        }
    }
