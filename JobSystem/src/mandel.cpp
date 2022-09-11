#include <iostream>

#include <omp.h>

struct Timer
{
    Timer()
    {
        m_startTime = omp_get_wtime();
    }
    ~Timer()
    {
        m_endTime = omp_get_wtime();
        m_timeTaken = m_endTime - m_startTime;
        std::cout << "Time taken " << m_timeTaken << std::endl;
    }

    double m_startTime = 0.0;
    double m_endTime = 0.0;
    double m_timeTaken = 0.0;
};


#define NPOINTS 1000
#define MXITR 1000


struct d_complex 
{
    double r = 0.0; 
    double i = 0.0;
};

struct d_complex c;
int numoutside = 0;

void testpoint(d_complex);

int main() 
{
    Timer timer;

    //int i, j;
    //double area, error, eps = 1.0e-5;
    double eps = 1.0e-5;
    #pragma omp parallel for default(none) private (c) firstprivate(eps)
    for (int i = 0; i < NPOINTS; i++) 
    {
        for (int j = 0; j < NPOINTS; j++) 
        {
            c.r = -2.0 + 2.5 * (double)(i) / (double)(NPOINTS) + eps;
            c.i = 1.125 * (double)(j) / (double)(NPOINTS) + eps;
            testpoint(c);
        }
    }
    double area = 2.0 * 2.5 * 1.125 * (double)(NPOINTS * NPOINTS - numoutside) / (double) (NPOINTS * NPOINTS);
    double error = area / (double)NPOINTS;
    std::cout << "area " << area << std::endl;
    std::cout << "error " << error << std::endl;
}

void testpoint(d_complex c)
{
    struct d_complex z;
    int iter;
    double temp;

    z = c;
    for (iter = 0; iter < MXITR; iter++) 
    {
        temp = (z.r * z.r) - (z.i * z.i) + c.r;
        z.i = z.r * z.i * 2 + c.i;
        z.r = temp;
        if ((z.r * z.r + z.i * z.i) > 4.0) 
        {
            #pragma omp atomic
            numoutside++;
            break;
        }
    }
}