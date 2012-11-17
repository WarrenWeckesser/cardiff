#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <tiffio.h>

#include <FL/Fl.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>

#include "ca.h"

extern int  current;
extern unsigned char matrix[2][DIM][DIM];
extern char I[DIM][DIM];
extern char K[DIM][DIM];
extern int  S[DIM][DIM];

extern unsigned char red[MAXV+1],green[MAXV+1],blue[MAXV+1];

extern Fl_Button *b;
extern Fl_RGB_Image *rgb_image;

extern int going;

void idle_cb( void *);
void IterateTorus(void);
void convert(int w, int h);

void save_tiff_cb(Fl_Widget *w, void *data)
    {
    int i;
    Fl_File_Chooser *fc = new Fl_File_Chooser(".","TIFF Files (*{.tif,.tiff})",2,"TIFF Output");
    fc->preview(0);
    /* printf("In cb_saveas\n"); */
    fc->show();
    while (fc->shown())
        Fl::wait();
    if (fc->value() == NULL)
        {
        delete fc;
        return;
        }

    TIFF *out;

    /*
     *  Open the TIFF file
     */
    if ((out = TIFFOpen(fc->value(), "w")) == NULL)
        {
        Fl_Widget *msgicon = fl_message_icon();
        msgicon->color(FL_YELLOW);
        char buf[256];
        snprintf(buf,255,"Could not open %s for writing.\n",fc->value());
        fl_alert(buf);
        delete fc;
        return;
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

    /* printf("Done.\n"); */

    delete fc; 
    }

void nout_cb(Fl_Widget* w, void* gobtn)
    {
    TIFF *out;
    int i,k;
    int N = 14;
    char fn[40];

    if (going)
        {
        Fl::remove_idle(idle_cb);
        going = 0;
        ((Fl_Button *) gobtn)->label("Go");
        }


    for (k = 0; k < N; ++k)
        {
        sprintf(fn,"out%03d.tif",k);
        printf("Opening %s\n",fn);
        out = TIFFOpen(fn,"w");
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
        printf("Writing file\n");
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
        printf("Closing file\n");
        TIFFClose(out);
        _TIFFfree(buf);

        IterateTorus();
        convert(DIM,DIM);
        rgb_image->uncache();
        b->redraw();

        }
    }
