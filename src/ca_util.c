
#include <cstdlib>
#include <math.h>

long random(void);


int max(int x,int y)
    {
    if (x > y)
        return x;
    else
        return y;
    }

int min(int x,int y)
    {
    if (x > y)
        return y;
    else
        return x;
    }

int irnd(int max)
    {
    int a;

    a = rand() % max;
    return((int) a);
    }

double dmin(double a,double b)
    {
    return( (a < b) ? a : b);
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
