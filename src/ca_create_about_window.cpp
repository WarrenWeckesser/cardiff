
#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>

#include "ca.h"

void close_window_cb(Fl_Widget *, void *);

using namespace std;

#define WIN_WIDTH  320
#define WIN_HEIGHT 200
#define BTN_WIDTH  60
#define BTN_HEIGHT 28

Fl_Window *create_about_window()
    {
    Fl_Double_Window *about_window = new Fl_Double_Window(WIN_WIDTH,WIN_HEIGHT,"About Cardiff");

    Fl_Pack* p1 = new Fl_Pack(0,10,WIN_WIDTH,0);
    p1->spacing(4);
    Fl_Box* ab1 = new Fl_Box(0,0,0,30,VERSION);
    ab1->labelfont(FL_HELVETICA_BOLD);
    Fl_Box* ab2 = new Fl_Box(0,0,0,30,"A Reaction-Diffusion Cellular Automaton");

    Fl_Pack* p2 = new Fl_Pack(0,0,0,0);
    p2->spacing(1);
    Fl_Box* ab3 = new Fl_Box(0,0,0,18,"Copyright (c) 2005, 2006  Warren Weckesser");
    ab3->labelsize(12);
    Fl_Box* ab4 = new Fl_Box(0,0,0,18,"Department of Mathematics");
    ab4->labelsize(12);
    Fl_Box* ab5 = new Fl_Box(0,0,0,18,"Colgate University");
    ab5->labelsize(12);
    Fl_Box* ab6 = new Fl_Box(0,0,0,18,"Hamilton, NY, USA");
    ab6->labelsize(12);
    p2->end();
    p1->end();
    Fl_Button* abb = new Fl_Button((WIN_WIDTH-BTN_WIDTH)/2,WIN_HEIGHT-BTN_HEIGHT-5,BTN_WIDTH,BTN_HEIGHT,"Close");
    abb->labelsize(12);
    abb->callback(close_window_cb,about_window);
    about_window->end();
    return about_window;
    }
