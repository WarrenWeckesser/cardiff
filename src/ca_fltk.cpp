#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <tiffio.h>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Button.H>


#include "ca.h"

void InitMatrixRandom(void);
void PrintMatrix(void);
void IterateTorus(void);

Fl_Window *create_about_window(void);


//
// Global (ack!) variables...
//

int  current;
unsigned char matrix[2][DIM][DIM];
char I[DIM][DIM];
char K[DIM][DIM];
int  S[DIM][DIM];

int  g  = 18;
int  k1 = 4;
int  k2 = 3;

unsigned char *image_buf;

unsigned char red[3][MAXV+1],green[3][MAXV+1],blue[3][MAXV+1];
int colormap = 0;

Fl_Button *b;
Fl_RGB_Image *rgb_image;

int going;
int update_rate = 1;
int update_count = update_rate;

//////////////////////////////////////////////////////

unsigned char *myimage(int w, int h)
    {
    unsigned char *image;
    int depth = 3;

    image = (unsigned char *) malloc(w*h*depth);
    if (image == NULL)
        return image;
    return image;
    }

void convert(int w, int h)
    {
    int i,j;
    int depth = 3;

    for (i = 0; i < w; ++i)
        for (j = 0; j < h; ++j)
            {
            image_buf[depth*h*i+depth*j]   = red[colormap][matrix[current][i][j]];
            image_buf[depth*h*i+depth*j+1] = green[colormap][matrix[current][i][j]];
            image_buf[depth*h*i+depth*j+2] = blue[colormap][matrix[current][i][j]];
            }
    }


//////////////////////////////////////////////////////

//
// FLTK Callbacks...
//

void idle_cb( void *)
    {
    IterateTorus();
    convert(DIM,DIM);
    rgb_image->uncache();
    if (--update_count == 0)
        {
        b->redraw();
        update_count = update_rate;
        }
    }

void go_cb(Fl_Button *btn, void *)
    {
    if (!going)
        {
        Fl::add_idle(idle_cb);
        going = 1;
        btn->label("Stop");
        }
    else
        {
        Fl::remove_idle(idle_cb);
        going = 0;
        btn->label("Go");
        }
    }

void onestep_cb(Fl_Button *btn, void *)
    {
    if (going)
        {
        Fl::remove_idle(idle_cb);
        going = 0;
        btn->label("Go");
        }
    IterateTorus();
    convert(DIM,DIM);
    rgb_image->uncache();
    b->redraw();
    }

void randomize_cb(Fl_Button *, void *)
    {
    InitMatrixRandom();
    convert(DIM,DIM);
    rgb_image->uncache();
    b->redraw();
    }

void gslider_cb(Fl_Value_Slider *s, void *)
    {
    g = (int) (s->value());
    }

void k1slider_cb(Fl_Value_Slider *s, void *)
    {
    k1 = (int) (s->value());
    }

void k2slider_cb(Fl_Value_Slider *s, void *)
    {
    k2 = (int) (s->value());
    }

void cb1_cb(Fl_Check_Button *btn, void *)
    {
    if (btn->value() == 1)
        colormap = 0;
        convert(DIM,DIM);
        rgb_image->uncache();
        b->redraw();
    }

void cb2_cb(Fl_Check_Button *btn, void *)
    {
    if (btn->value() == 1)
        colormap = 1;
        convert(DIM,DIM);
        rgb_image->uncache();
        b->redraw();
    }

void cb3_cb(Fl_Check_Button *btn, void *)
    {
    if (btn->value() == 1)
        colormap = 2;
        convert(DIM,DIM);
        rgb_image->uncache();
        b->redraw();
    }

void update_slider_cb(Fl_Value_Slider *s, void *)
    {
    update_rate = (int) (s->value());
    update_count = update_rate;
    }

void show_window_cb(Fl_Widget* w, void* data)
    {
    Fl_Window *window = (Fl_Window *) data;
    window->show();
    }

void close_window_cb(Fl_Widget* w, void* data)
    {
    Fl_Window *window = (Fl_Window *) data;
    window->hide();
    }



void quit_cb(Fl_Button *, void *)
    {
    exit(0);
    }

void save_tiff_cb(Fl_Widget *, void *);
void nout_cb(Fl_Widget *, void *);

void image_button_cb(Fl_Button *w, void *data)
    {
    int x,y;

    x = Fl::event_x() - (w->x());
    y = Fl::event_y() - (w->y());
    printf("x = %d, y = %d\n",x,y);
    //cout << "x = " << x << ", y = " << y << endl;
    matrix[current][y][x] = 10;
    convert(DIM,DIM);
    rgb_image->uncache();
    b->redraw();
    }


//////////////////////////////////////////////////////

#define BUTTON_WIDTH 90

main(int argc, char **argv)
    {
    int i,j,k;

    // printf("Initializing the data.\n");
    InitMatrixRandom();

    /*
     *  Create RGB color maps
     */

    for (i = 0; i <= MAXV; ++i)
        {
        double z = (double) i /(double) MAXV;
        // red/yellow
        red[0][i]   = (unsigned char) (50.0*z+200.0);
        green[0][i] = (unsigned char) (225.0*z);
        blue[0][i]  = (unsigned char) (100.0*z);
        // blue/green
        red[1][i]   = (unsigned char) 0;
        green[1][i] = (unsigned char) (255.0*z);
        blue[1][i]  = (unsigned char) (255.0*(1-z));
        // black/white
        red[2][i]   = (unsigned char) (255.0*z);
        green[2][i] = (unsigned char) (255.0*z);
        blue[2][i]  = (unsigned char) (255.0*z);
        }


    going = 0;
    int width  = DIM;
    int height = DIM;
    int depth  = 3;
    image_buf = myimage(width,height);
    if (image_buf == NULL)
        {
        printf("myimage(...) failed.\n");
        exit(-1);
        }
    convert(DIM,DIM);

    Fl::scheme("plastic");

    Fl_Window *about_window = create_about_window();

    //
    // The main window
    //
    Fl_Double_Window *main_win;
    main_win = new Fl_Double_Window(DIM+185,DIM+60);
    main_win->label("Cardiff");
    //
    // Widgets for the image.
    //
    b = new Fl_Button(10,10,width,height);
    rgb_image = new Fl_RGB_Image((const uchar *) image_buf, width, height, depth);
    b->image(rgb_image);
    b->callback((Fl_Callback *) image_button_cb);
    //
    // A Pack group for the parameter sliders
    //
    Fl_Pack *sliders = new Fl_Pack(DIM+20,10,155,0);
    sliders->type(FL_VERTICAL);
    Fl_Box *par_label = new Fl_Box(0,0,155,20);
    par_label->label("Parameters");
    //
    // A Value_Slider for g
    //
    Fl_Value_Slider *gslider = new Fl_Value_Slider(0,0,155,20);
    gslider->type(FL_HOR_NICE_SLIDER);
    gslider->bounds(1.0,20.0);
    gslider->precision(0);
    gslider->value((double) g);
    gslider->callback((Fl_Callback *) gslider_cb);
    //
    // A Value_Slider for k1
    //
    Fl_Value_Slider *k1slider = new Fl_Value_Slider(0,0,155,20);
    k1slider->type(FL_HOR_NICE_SLIDER);
    k1slider->bounds(1.0,5.0);
    k1slider->precision(0);
    k1slider->value((double) k1);
    k1slider->callback((Fl_Callback *) k1slider_cb);
    //
    // A Value_Slider for k2
    //
    Fl_Value_Slider *k2slider = new Fl_Value_Slider(0,0,155,20);
    k2slider->type(FL_HOR_NICE_SLIDER);
    k2slider->bounds(1.0,5.0);
    k2slider->precision(0);
    k2slider->value((double) k2);
    k2slider->callback((Fl_Callback *) k2slider_cb);
    //
    new Fl_Box(0,0,155,15);  // Empty box for spacing
    //
    // A group for the color check boxes
    //
    Fl_Pack *cbgroup = new Fl_Pack(0,0,155,50);
    cbgroup->type(FL_VERTICAL);
    //
    Fl_Check_Button *color1 = new Fl_Check_Button(0,0,0,20);
    color1->label("Red && Yellow");
    color1->type(FL_RADIO_BUTTON);
    color1->callback((Fl_Callback *) cb1_cb);
    color1->set();
    Fl_Check_Button *color2 = new Fl_Check_Button(0,0,0,20);
    color2->label("Blue && Green");
    color2->type(FL_RADIO_BUTTON);
    color2->callback((Fl_Callback *) cb2_cb);
    Fl_Check_Button *color3 = new Fl_Check_Button(0,0,0,20);
    color3->label("Black && White");
    color3->type(FL_RADIO_BUTTON);
    color3->callback((Fl_Callback *) cb3_cb);
    cbgroup->end();
    //
    new Fl_Box(0,0,155,15);  //spacing
    //
    Fl_Box *ur_label = new Fl_Box(0,0,155,20);
    ur_label->label("Update Rate");
    //
    // A Value_Slider for the update rate.
    //
    Fl_Value_Slider *update_slider = new Fl_Value_Slider(0,0,155,20);
    update_slider->type(FL_HOR_NICE_SLIDER);
    update_slider->bounds(1.0,25.0);
    update_slider->precision(0);
    update_slider->value((double) update_rate);
    update_slider->callback((Fl_Callback *) update_slider_cb);
    //
    // Finished adding widgets to the sliders Pack
    //
    sliders->end();
    //
    // A Pack group for the buttons.
    //
    Fl_Pack *buttons = new Fl_Pack(10,DIM+20,0,30);
    buttons->type(buttons->HORIZONTAL);
    buttons->spacing(3);
    //
    // Add the Go button
    //
    Fl_Button *go = new Fl_Button(0,0,2*BUTTON_WIDTH/3,30);
    go->label("Go");
    go->callback((Fl_Callback *) go_cb);
    //
    // Add the 1 Step button
    //
    Fl_Button *onestep = new Fl_Button(0,0,2*BUTTON_WIDTH/3,30);
    onestep->label("1 Step");
    onestep->callback((Fl_Callback *) onestep_cb);
    //
    // Add the Randomize button
    //
    Fl_Button *randomize = new Fl_Button(0,0,BUTTON_WIDTH,30);
    randomize->label("Randomize");
    randomize->callback((Fl_Callback *) randomize_cb);
    //
    // Add the TIFF Output button
    //
    Fl_Button *tiff = new Fl_Button(0,0,BUTTON_WIDTH,30);
    tiff->label("TIFF Output");
    tiff->callback((Fl_Callback *) save_tiff_cb);

    //
    // Add the About button
    //
    Fl_Button *about = new Fl_Button(0,0,2*BUTTON_WIDTH/3,30);
    about->label("About");
    about->callback((Fl_Callback *) show_window_cb, about_window);
    //
    // Add the Quit button
    //
    Fl_Button *quit = new Fl_Button(0,0,2*BUTTON_WIDTH/3,30);
    quit->label("Quit");
    quit->callback((Fl_Callback *) quit_cb);
    //
    // Add the N Out button
    //
    Fl_Button *nout = new Fl_Button(0,0,2*BUTTON_WIDTH/3,30);
    nout->label("N Out");
    nout->callback((Fl_Callback *) nout_cb,go);
    //
    // Finished adding buttons to the buttons Pack
    //
    buttons->end();
    //
    // Finished adding widgets to the main window.
    //
    main_win->end();

    main_win->show();
    return Fl::run();

    }
