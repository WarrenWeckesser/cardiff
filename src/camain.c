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

#include "xtools.h"

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

#define DIM  400
#define MAXV 100

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

int max(x,y)
int x,y;
    {
    if (x > y)
        return x;
    else
        return y;
    }

int min(x,y)
int x,y;
    {
    if (x > y)
        return y;
    else
        return x;
    }

/********************************************/

void InitMatrixRandom(void)
    {
    int  i,j;
    long r;

    current = 0;
    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            r = random();
            matrix[0][i][j] = (int) (r-(r/MAXV)*MAXV);
            if (matrix[0][i][j] > MAXV)
                printf("Hey!  This don't work!\n");
            }
    }

int irnd(int max)
    {
    long a;

    a = random();
    a = a - (a/max)*max;
    return((int) a);
    }

double dmin(double a,double b)
    {
    return( (a < b) ? a : b);
    }

double dist(int x1, int y1, int x2, int y2, int max)
    {
    double dx,dx1,dx2,dx3;
    double dy,dy1,dy2,dy3;

    dx1 = fabs(x1-x2);
    dx2 = fabs(x1-x2+max);
    dx3 = fabs(x1-x2-max);
    dx = dmin(dx1,dmin(dx2,dx3));
    dy1 = fabs(y1-y2);
    dy2 = fabs(y1-y2+max);
    dy3 = fabs(y1-y2-max);
    dy = dmin(dy1,dmin(dy2,dy3));
    return(sqrt(dx*dx + dy*dy));
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

        
void Iterate(void)
    {
    int new;
    int i,j;

    new = 1 - current;
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

    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            if (matrix[current][i][j] == MAXV)
                matrix[new][i][j] = 0;
            else
                {
                if (matrix[current][i][j] == 0)
                    matrix[new][i][j] = (int)((float)K[i][j]/k1 + (float)I[i][j]/k2);
/*
                    matrix[new][i][j] = K[i][j]/k1 + I[i][j]/k2;
*/
                else
                    matrix[new][i][j] = min((int)((float)S[i][j]/I[i][j] + g), MAXV);
                }
            }
    current = new;
    }
        
void IterateTorus(void)
    {
    int new;
    int i,j;

    new = 1 - current;
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

    for (i = 0; i < DIM; ++i)
        for (j = 0; j < DIM; ++j)
            {
            if (matrix[current][i][j] == MAXV)
                matrix[new][i][j] = 0;
            else
                {
                if (matrix[current][i][j] == 0)
                    matrix[new][i][j] = (int)((float)K[i][j]/k1 + (float)I[i][j]/k2);
/*
                    matrix[new][i][j] = K[i][j]/k1 + I[i][j]/k2;
*/
                else
                    matrix[new][i][j] = min((int)((float)S[i][j]/I[i][j] + g), MAXV);
                }
            }
    current = new;
    }

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


/*****
void Resize(w, client_data, event)
Widget       w;
XtPointer    client_data;
XExposeEvent *event;
    {
    int       i;
    Arg       arg[4];
    Dimension width,height,mw,mh;

    i = 0;
    XtSetArg(arg[i], XmNwidth, &width);   ++i;
    XtSetArg(arg[i], XmNheight, &height); ++i;
    XtSetArg(arg[i], XmNmarginWidth, &mw); ++i;
    XtSetArg(arg[i], XmNmarginHeight, &mh); ++i;
    XtGetValues(shell2rc, arg, i);
    if (width < 20 || height < 20)
        {
        ActualWidth = 0;
        ActualHeight = 0;
        }
    else
        {
        ActualWidth = width-2*mw;
        ActualHeight = height-2*mh;
        }
    Redraw(drawingarea,NULL,NULL);
    }
 *****/

/*********************************************************************/


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

double dmax3(double x, double y, double z)
    {
    double m;
    m = x;
    if (y > m)
        m = y;
    if (z > m)
        m = z;
    return m;
    }

#define PI 3.141592654

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
