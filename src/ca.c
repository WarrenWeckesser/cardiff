/*
 *  camain.c
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/Frame.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/DrawingA.h>
#include <Xm/BulletinB.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeB.h>

/* #include "xtools.h" */

#include "ca.h"

extern double atof();

extern void QuitCB();
extern void RandomizeCB();
extern void Pattern1CB();
extern void Continue();
extern void StartCB();
extern void StopCB();

extern void Redraw();
/*
extern void Resize();
*/
extern void InitColorData();
extern void InitColors(unsigned bg);
void SetColors();


/**************************************************************/
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

int  update = 1;
int  ColorSelection = 0;
XColor RGBdata[MAXV+1];

GC      gcwhite;
GC      gcdraw;
Display *display;
Window  window;
Colormap my_cmap;

Widget  drawingarea;
Widget  shell2rc;
/*
Dimension ActualWidth, ActualHeight;
Pixmap  drawpm;
*/
int     UpdateMethod = 1;
int     initseed;

GC     color[MAXV+1];



/********************************************/

#define BUFFERSIZE  300

typedef XPoint PointBuffer[BUFFERSIZE];

static PointBuffer DrawBuffer[MAXV+1];
static int         NumPts[MAXV+1];

void Redraw(w, client_data, event)
Widget       w;
XtPointer    client_data;
XExposeEvent *event;
    {
    int i,j;

    if (!XtIsRealized(drawingarea))
        return;
    for (i = 0; i < MAXV+1; ++i)
        NumPts[i] = 0;
    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            int val;
            val = matrix[current][i][j];
            if (val > MAXV)
                printf("value too big...\n");
            DrawBuffer[val][NumPts[val]].x = i;
            DrawBuffer[val][NumPts[val]].y = j;
            ++NumPts[val];
            if (NumPts[val] == BUFFERSIZE)
                {
                XDrawPoints(display,window,color[val],
                            DrawBuffer[val],BUFFERSIZE,CoordModeOrigin);
                NumPts[val] = 0;
                }
            }
    for (i = 0; i < MAXV+1; ++i)
        if (NumPts[i] > 0)
            XDrawPoints(display,window,color[i],
                        DrawBuffer[i],NumPts[i],CoordModeOrigin);
    }

/*ARGSUSED*/
void QuitCB(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    exit(0);
    }


/*ARGSUSED*/
void RandomizeCB(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    int i;

    StopCB(w,NULL,NULL);
    InitMatrixRandom();
    Redraw(drawingarea,NULL,NULL);
    }

/*ARGSUSED*/
void Random2CB(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    int i;

    StopCB(w,NULL,NULL);
    InitMatrixRandom2();
    Redraw(drawingarea,NULL,NULL);
    }

/*ARGSUSED*/
void Random3CB(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    int i;

    StopCB(w,NULL,NULL);
    InitMatrixRandom3();
    Redraw(drawingarea,NULL,NULL);
    }

/*ARGSUSED*/
void Pattern1CB(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    int i;

    StopCB(w,NULL,NULL);
    InitMatrix1();
    Redraw(drawingarea,NULL,NULL);
    }

/*ARGSUSED*/
void Pattern2CB(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    int i;

    StopCB(w,NULL,NULL);
    InitMatrix2();
    Redraw(drawingarea,NULL,NULL);
    }

/*ARGSUSED*/
void Pattern3CB(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    int i;

    StopCB(w,NULL,NULL);
    InitMatrix3();
    Redraw(drawingarea,NULL,NULL);
    }

/*ARGSUSED*/
void Pattern4CB(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    int i;

    StopCB(w,NULL,NULL);
    InitMatrix4();
    Redraw(drawingarea,NULL,NULL);
    }

/*ARGSUSED*/
void Pattern5CB(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    int i;

    StopCB(w,NULL,NULL);
    InitMatrix5();
    Redraw(drawingarea,NULL,NULL);
    }

/*ARGSUSED*/
void Continue(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    int n;
    int result;
    int i;

    StopCB(w,NULL,NULL);
    n = (int) client_data;
    for (i = 0; i < n; ++i)
        {
        IterateTorus();
        }
    Redraw(drawingarea,NULL,NULL);
    }

/*ARGSUSED*/
Boolean DoBackground(XtPointer client_data)
    {
    int result;
    int i;

    for (i = 0; i < update; ++i)
        IterateTorus();
    Redraw(drawingarea,NULL,NULL);
    return FALSE;
    }

extern XtWorkProcId XtAddWorkProc();
static XtWorkProcId workid = (XtWorkProcId) NULL;

/*ARGSUSED*/
void StartCB(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    if (workid == (XtWorkProcId) NULL)
        workid = XtAddWorkProc(DoBackground,NULL);
    }

/*ARGSUSED*/
void StopCB(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    if (workid != (XtWorkProcId) NULL)
        {
        XtRemoveWorkProc(workid);
        workid = (XtWorkProcId) NULL;
        }
    }

/*ARGSUSED*/
/*
void MethodCB(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    int n;

    n = (int) client_data;
    UpdateMethod = n;
    }
*/

/*ARGSUSED*/
/*
void SelectColor(Widget w, XtPointer client_data,
                          XtPointer call_data)
    {
    int cs;

    cs = (int) client_data;
    if (cs != ColorSelection)
        {
        ColorSelection = cs;
        SetColors();
        Redraw(drawingarea,NULL,NULL);
        }
    }
*/

/*ARGSUSED*/
void gchanged(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    char   *str;

    str = XmTextGetString(w);
    g = atoi(str);
    XtFree(str);
    }

/*ARGSUSED*/
void k1changed(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    char   *str;

    str = XmTextGetString(w);
    k1 = atoi(str);
    XtFree(str);
    }

/*ARGSUSED*/
void k2changed(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    char   *str;

    str = XmTextGetString(w);
    k2 = atoi(str);
    XtFree(str);
    }

/*ARGSUSED*/
void updatechanged(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    char   *str;

    str = XmTextGetString(w);
    update = atoi(str);
    XtFree(str);
    }

/*ARGSUSED*/
/*
void Vchanged(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    char   *str;
    double tmp;

    str = XmTextGetString(w);
    tmp = atof(str);
    SetVel(tmp);
    XtFree(str);
    }
*/

/**************************************************************/

unsigned GetNamedColor(Widget w, char *colorname)
    {
    Display  *disp;
    int      scr;
    Colormap cmap;
    XColor   color,ignore;

    disp = XtDisplay(w);
    scr = DefaultScreen(disp);
    cmap = DefaultColormap(disp,scr);
    if (XAllocNamedColor(disp,cmap,colorname,&color,&ignore))
        return color.pixel;
    else
        {
        printf("Warning: Could not allocate color: %s\n",colorname);
        return(BlackPixel(disp,scr));
        }
    }

unsigned GetRGBColor(Widget w, int r, int g, int b)
    {
    Display  *disp;
    int      scr;
    Colormap cmap;
    XColor   color;

    disp = XtDisplay(w);
    scr = DefaultScreen(disp);
    cmap = DefaultColormap(disp,scr);
    color.red   = r;
    color.green = g;
    color.blue  = b;
    if (XAllocColor(disp,cmap,&color))
        return color.pixel;
    else
        {
        printf("Warning: Could not allocate RGB color: %d,%d,%d\n",
                                                        r,g,b);
        return(BlackPixel(disp,scr));
        }
    }



unsigned GetTrigColor(Widget w, double theta)
    {
    Display  *disp;
    int      scr;
    Colormap cmap;
    XColor   color;
    double   c;
    double   r,g,b;
    double   m;

    disp = XtDisplay(w);
    scr = DefaultScreen(disp);
    cmap = DefaultColormap(disp,scr);
    c = cos(theta);
    if (theta < PI)
        {
        r = -theta/PI + 1;
        g = theta/PI;
        }
    else
        {
        r = theta/PI - 1;
        g = -theta/PI + 2;
        }
    if (theta < 0.5*PI)
        b = 1.0;
    else if (theta < 1.5*PI)
        b = -theta/PI + 1.5;
    else
        b = 0.0;
/*
    r = (c+1)/2.0;
    r = r*r;
*/
/*
    g = (-c+1)/2.0;
    g = g*g;
*/
/*
    b = (cos(0.5*theta)+1)/2.0;
    b = b*b;
*/
    m = dmax3(r,g,b);
    color.red   = 65535*(r/m);
    color.green = 65535*(g/m);
    color.blue  = 65535*(b/m);
    if (XAllocColor(disp,cmap,&color))
        return color.pixel;
    else
        {
        printf("Warning: Could not allocate Trig color: %f\n",theta);
        return(BlackPixel(disp,scr));
        }
    }

void SetupGCs()
    {
    unsigned  bg;
    Arg       args[1];
    int       i;
    XGCValues gcv;

    InitColorData();
    /*
     *  Get background color of drawingarea
     */
    XtSetArg(args[0],XtNbackground,&bg);
    XtGetValues(drawingarea,args,1);

    InitColors(bg);
    gcv.foreground = GetNamedColor(drawingarea,"white");
    gcv.background = bg;
    gcwhite = XCreateGC(XtDisplay(drawingarea),XtWindow(drawingarea),
                         GCForeground | GCBackground, &gcv);
    gcv.foreground = bg;
    gcv.background = bg;
    gcdraw = XCreateGC(XtDisplay(drawingarea),XtWindow(drawingarea),
                         GCForeground | GCBackground, &gcv);
    }


void InitColors(unsigned bg)
    {
    int i;
    XGCValues gcv;

    for (i = 0; i <= MAXV; ++i)
        {
        gcv.background = bg;
        gcv.foreground = 
             GetRGBColor(drawingarea,
                         RGBdata[i].red,RGBdata[i].green,RGBdata[i].blue);
        color[i] = XCreateGC(XtDisplay(drawingarea),XtWindow(drawingarea),
                         GCForeground | GCBackground, &gcv);
        }
    }

void InitColorData()
    {
    int      i;

    for (i = 0; i <= MAXV; ++i)
        {
        if (ColorSelection == 0)
            {
            /* Color 0: black and white */
            RGBdata[i].red   = 60000-500*i;
            RGBdata[i].green = 60000-500*i;
            RGBdata[i].blue  = 60000-500*i;
            }
        else if (ColorSelection == 1)
            {
            /* Color 1: bluish/purplish */
            RGBdata[i].red   = 40000-250*i;
            RGBdata[i].green = 50000-500*i;
            RGBdata[i].blue  = 50000-200*i;
            }
        else if (ColorSelection == 2)
            {
            /* Color 2: reddish         */
            RGBdata[i].red   = 10000+400*i;
            RGBdata[i].green = 10000-100*i;
            RGBdata[i].blue  = 0;
            }
        else if (ColorSelection == 3)
            {
            /* Color 3: Blue/green      */
            RGBdata[i].red   = 0;
            RGBdata[i].green = 10000+250*i;
            RGBdata[i].blue  = 30000;
            }
        else if (ColorSelection == 4)
            {
            /* Color 4: ???      */
            RGBdata[i].red   = 15000+500*i;
            RGBdata[i].green = 10000+300*i;
            RGBdata[i].blue  = 0;
            }
        else if (ColorSelection == 5)
            {
            /* Color 5: ???      */
            RGBdata[i].red   = 15000+500*i;
            RGBdata[i].green = 0;
            RGBdata[i].blue  = 0;
            }
        }
    }


/*ARGSUSED*/
void SaveCB(w,client_data,call_data)
Widget    w;
XtPointer client_data,call_data;
    {
    FILE *fp;
    int i,j;

    fp = fopen("/tmp/image.ppm","w");
    if (fp == NULL)
        {
        printf("ERROR: Unable to open /tmp/image.ppm\n");
        return;
        }
    fprintf(fp,"P3\n");
    fprintf(fp,"# File: /tmp/image.ppm, generated by the program ca\n");
    fprintf(fp,"%5d %5d\n",DIM,DIM);
    fprintf(fp," 65535\n");
    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            int clr;
            clr = matrix[current][i][j];
            fprintf(fp,"%d %d %d\n",RGBdata[clr].red,
                                RGBdata[clr].green,
                                RGBdata[clr].blue);
            }
    fclose(fp);
    }

main(argc, argv)
    int     argc;
    char  **argv;
    {
    int     i;
    Widget  topLevel;
    Widget  MainWindow;
    Widget  box;
    Widget  buttons, buttonsFrame;
    Widget  MenuBar;
    Widget  FileButton, FileMenu;
    Widget  Save, Quit;
    Widget  Reset, SingleStep, Step100, Step1000;
    Widget  Start, Stop;
    Widget  ResetButton;
    Widget  ResetMenu, Random, Random2, Random3;
    Widget  Pattern1, Pattern2, Pattern3, Pattern4, Pattern5;
    Widget  IterateButton, IterateMenu;
    Widget  params, paramsFrame;
    Widget  gedit, k1edit, k2edit;
    Widget  updateedit;

    Widget  shell2;
    int     depth;
    Dimension     width, height;
    Arg     arg[3];
    char    buf[20];


    ColorSelection = 0;
    if (argc > 1)
        {
        ColorSelection = atoi(argv[1]);
        if ((ColorSelection < 0) || (ColorSelection > 5))
            {
            printf("Error: first argument must be 0 -- 5.\n");
            printf("0 will be used.\n");
            ColorSelection = 0;
            }
        }
    srandom(123);

    topLevel = XtInitialize( argv[0], "XCA", NULL, 0, &argc, argv);

    shell2 = XtCreateApplicationShell("Display",
                                      topLevelShellWidgetClass,
                                      NULL,0);

    i = 0;
    XtSetArg(arg[i],XmNresizeWidth, TRUE);  ++i;
    XtSetArg(arg[i],XmNresizeHeight, TRUE);  ++i;
    shell2rc = XtCreateManagedWidget("shell2rc",
                                     xmRowColumnWidgetClass,
                                     shell2,
                                     NULL, 0);


    MainWindow = XtCreateManagedWidget(
            "MainWindow",
            /* xmBulletinBoardWidgetClass,*/
            xmMainWindowWidgetClass,
            topLevel,
            NULL, 0);

    /*
     *  Create menu bar
     */
    MenuBar = XmCreateMenuBar(MainWindow,"MenuBar",NULL,0);
    XtManageChild(MenuBar);
    /*
     *  Add items to the menu bar
     */
    FileButton = XtVaCreateManagedWidget("FileButton",
                        xmCascadeButtonWidgetClass,
                        MenuBar, NULL);
    FileMenu = XmCreatePulldownMenu(MenuBar,
                        "FileMenu",NULL,0);
    Save       = MakeButton("Save", FileMenu, SaveCB, NULL);
    Quit       = MakeButton("Quit", FileMenu, QuitCB, NULL);
    XtVaSetValues(FileButton,XmNsubMenuId,FileMenu,NULL);

    ResetButton = XtVaCreateManagedWidget("ResetButton",
                        xmCascadeButtonWidgetClass,
                        MenuBar, NULL);
    ResetMenu = XmCreatePulldownMenu(MenuBar,
                        "ResetMenu",NULL,0);
    Random    = MakeButton("Random",   ResetMenu,RandomizeCB,NULL);
    Random2   = MakeButton("Random2",  ResetMenu,Random2CB,  NULL);
    Random3   = MakeButton("Random3",  ResetMenu,Random3CB,  NULL);
    Pattern1  = MakeButton("Pattern1", ResetMenu,Pattern1CB, NULL);
    Pattern2  = MakeButton("Pattern2", ResetMenu,Pattern2CB, NULL);
    Pattern3  = MakeButton("Pattern3", ResetMenu,Pattern3CB, NULL);
    Pattern4  = MakeButton("Pattern4", ResetMenu,Pattern4CB, NULL);
    Pattern5  = MakeButton("Pattern5", ResetMenu,Pattern5CB, NULL);
    XtVaSetValues(ResetButton,XmNsubMenuId,ResetMenu,NULL);

    IterateButton = XtVaCreateManagedWidget("IterateButton",
                        xmCascadeButtonWidgetClass,
                        MenuBar, NULL);
    IterateMenu = XmCreatePulldownMenu(MenuBar,
                        "IterateMenu",NULL,0);
    SingleStep = MakeButton("SingleStep",
                      IterateMenu, Continue, (XtPointer) 1);
    Step100    = MakeButton("Step100",
                      IterateMenu, Continue, (XtPointer) 100);
    Step1000   = MakeButton("Step1000",
                      IterateMenu, Continue, (XtPointer) 1000);
    XtVaSetValues(IterateButton,XmNsubMenuId,IterateMenu,NULL);
    
    /*
     */
    box = XtVaCreateManagedWidget(
            "box",
            xmBulletinBoardWidgetClass,
            MainWindow,NULL);
    /*
     *  Create other command buttons.
     */
    buttonsFrame = XtVaCreateManagedWidget(
                     "buttonsFrame",
                     xmFrameWidgetClass,
                     box, NULL);
    i = 0;
    XtSetArg(arg[i], XmNorientation, XmHORIZONTAL); i++;
    XtSetArg(arg[i], XmNpacking,     XmPACK_TIGHT); i++;
    buttons = XtCreateManagedWidget(
            "buttons",
            xmRowColumnWidgetClass,
            buttonsFrame,
            arg, i);

    Start      = MakeButton("Start",      buttons, StartCB,  NULL);
    Stop       = MakeButton("Stop",       buttons, StopCB,   NULL);
    sprintf(buf,"%d",update);
    updateedit = MakeEditField("update",3,3,buf,buttons,updatechanged,NULL);

    /*
     * Create the widgets for editing the system parameters.
     */
    paramsFrame = XtVaCreateManagedWidget(
                     "paramsFrame",
                     xmFrameWidgetClass,
                     box, NULL);
    i = 0;
    XtSetArg(arg[i], XmNpacking,     XmPACK_TIGHT); i++;
    params = XtCreateManagedWidget(
            "params",
            xmRowColumnWidgetClass,
            paramsFrame,
            arg, i);
    sprintf(buf,"%d",g);
    gedit = MakeEditField("g",3,3,buf,params,gchanged,NULL);
    sprintf(buf,"%d",k1);
    k1edit = MakeEditField("k1",3,3,buf,params,k1changed,NULL);
    sprintf(buf,"%d",k2);
    k2edit = MakeEditField("k2",3,3,buf,params,k2changed,NULL);


    /*
     *  Create widget in which to draw the matrix.
     */
    i = 0;
    XtSetArg(arg[i], XmNmarginWidth,  DIM);  i++;
    XtSetArg(arg[i], XmNmarginHeight, DIM); i++;
    drawingarea = XtCreateManagedWidget(
                    "drawingarea",
                     xmDrawingAreaWidgetClass,
                     shell2rc,
                     arg, i);
    XtAddCallback(drawingarea,XmNexposeCallback,Redraw,NULL);
/*
    XtAddCallback(drawingarea,XmNresizeCallback,Resize,NULL);
*/

    XtRealizeWidget(topLevel);
    XtRealizeWidget(shell2);
    display = XtDisplay(drawingarea);
    window = XtWindow(drawingarea);
    SetupGCs();

    XtMainLoop();
    }
