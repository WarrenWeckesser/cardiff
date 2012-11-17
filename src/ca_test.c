#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <tiffio.h>

#include "ca.h"

void InitMatrixRandom();
void PrintMatrix();
void IterateTorus();

/*
 * Global (ack!) variables...
 */

int  current;
char matrix[2][DIM][DIM];
char I[DIM][DIM];
char K[DIM][DIM];
int  S[DIM][DIM];

int  g  = 15;
int  k1 = 3;
int  k2 = 3;

#define NUM_ITERATIONS 500

main(int argc, char **argv)
    {
    int i,j,k;

    printf("Initializing the data.\n");
    InitMatrixRandom();
    printf("Iterating %d times.\n",NUM_ITERATIONS);
    for (k = 0; k < NUM_ITERATIONS; ++k)
        {
        IterateTorus();
        }

    /*
     *  Create RGB "color map"
     */
    unsigned char red[MAXV+1],green[MAXV+1],blue[MAXV+1];
    for (i = 0; i <= MAXV; ++i)
        {
        red[i]   = (unsigned char) MAXV-i;
        green[i] = (unsigned char) 10;
        blue[i]  = (unsigned char) 255-i;
        }

    printf("Creating TIFF output file ca_output.tif.\n");

    TIFF *out;

    /*
     *  Open the TIFF file
     */
    if ((out = TIFFOpen("ca_output.tif", "w")) == NULL)
        {
        printf("Could not open ca_output.tif for writing\n");
        exit(42);
        }

    /*
     *  We need to set some values for basic tags before we can add any data
     */
    TIFFSetField(out, TIFFTAG_IMAGEWIDTH,  DIM);
    TIFFSetField(out, TIFFTAG_IMAGELENGTH, DIM);
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 3);
    TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, DIM);

    TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

    /*
     *  Length in memory of one row of pixels in the image.
     */
    tsize_t linebytes = 3 * DIM;

    /*
     *  Buffer used to store the row of pixel information for writing to file
     */
    unsigned char *buf = NULL; 

    /*
     *  Allocating memory to store the pixels of current row
     */
    if (TIFFScanlineSize(out) < linebytes)
        buf = (unsigned char *) _TIFFmalloc(linebytes);
    else
        buf = (unsigned char *) _TIFFmalloc(TIFFScanlineSize(out));

    /*
     *  We set the strip size of the file to be size of one row of pixels
     */
    TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(out,3*DIM));

    /*
     *  Now writing image to the file one strip at a time
     */
    uint32 row;
    for (row = 0; row < DIM; row++)
        {
        for (i = 0; i < DIM; ++i)
            {
            buf[3*i]   = red[matrix[current][row][i]];
            buf[3*i+1] = green[matrix[current][row][i]];
            buf[3*i+2] = blue[matrix[current][row][i]];
            }
        if (TIFFWriteScanline(out, buf, row, 0) < 0)
            break;
        }

    /*
     *  Close the file
     */
    TIFFClose(out);
    if (buf)
        _TIFFfree(buf);

    printf("Done.\n");
    }
