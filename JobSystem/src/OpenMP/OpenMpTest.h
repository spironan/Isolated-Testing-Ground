#pragma once

#include <omp.h>
#include <cstdio>
#include <iostream>

static long num_steps = 100000000;

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
class OpenMpTest
{
public:
    void fn()
    {
        #pragma omp parallel
        {
            int id = omp_get_thread_num();
            printf("hello (%d)", id);
            printf("world (%d) \n", id);
        }
    }

    void calculatePi_Serial()
    {
        Timer timer;

        double step = 0;
        
        //int i = 0;
        double x = 0, pi = 0, sum = 0;
        step = 1.0 / num_steps;

        for (int i = 0; i < num_steps; ++i)
        {
            x = (i + 0.5) * step;
            sum = sum + 4.0 / (1.0 + x * x);
        }

        pi = step * sum;

        std::cout << "pi value is " << pi << std::endl;
    }

    void calculatePi_Ex2()
    {
        Timer timer;

        static constexpr int desired_num_threads = 2;
        double step = 1.0 / num_steps;
        
        // ugly solution to false sharing
        // works but really?
        // can we solve this more elegantly?
        static constexpr int padding = 8;

        int nthreads = 0;
        double pi = 0.0, sum[desired_num_threads][padding] = { 0.0 };

        // we request openMP for our desired number of threads.
        // BUT THAT DOES NOT MEAN THEY'll give us that amount
        omp_set_num_threads(desired_num_threads);
        #pragma omp parallel
        {
            int id = omp_get_thread_num();
            int nthrds = omp_get_num_threads();

            // therefore we have to check to verify that the 
            // code works for no matter how many threads
            if (id == 0) 
                nthreads = nthrds;

            double x = 0.0;

            // cyclic distribution
            for (int i = id; i < num_steps; i += nthrds)
            {
                x = (i + 0.5) * step;
                sum[id][0] += 4.0 / (1.0 + x * x);
            }
        }

        for(int i = 0; i < nthreads; ++i)
            pi += step * sum[i][0];

        std::cout << "pi value is " << pi << std::endl;
        std::cout << "no of threads " << nthreads << std::endl;
    }

    void calculatePi_Ex3()
    {
        Timer timer;

        static constexpr int desired_num_threads = 2;
        double step = 1.0 / num_steps;

        int nthreads = 0;
        double pi = 0.0;

        // we request openMP for our desired number of threads.
        // BUT THAT DOES NOT MEAN THEY'll give us that amount
        omp_set_num_threads(desired_num_threads);
        #pragma omp parallel
        {
            int id = omp_get_thread_num();
            int nthrds = omp_get_num_threads();

            // therefore we have to check to verify that the 
            // code works for no matter how many threads
            if (id == 0)
                nthreads = nthrds;

            double x = 0.0;
            // use sum as a local instead to prevent false sharing!
            double sum = 0.0;
            // cyclic distribution
            for (int i = id; i < num_steps; i += nthrds)
            {
                x = (i + 0.5) * step;
                sum += 4.0 / (1.0 + x * x);
            }
            
            // method using mutex
            //#pragma omp critical
            //    pi += step * sum;

            // method using atomic
            sum = sum * step;
            #pragma omp atomic
                pi += sum;
        }

        std::cout << "pi value is " << pi << std::endl;
        std::cout << "no of threads " << nthreads << std::endl;
    }

    void calculatePi_Ex4()
    {
        Timer timer;

        double step = 1.0 / num_steps;

        double pi = 0.0;
        double sum = 0.0;
        
        double x = 0.0;
        #pragma omp parallel for reduction(+:sum) private(x) schedule(runtime)
        for (int i = 0; i < num_steps; ++i)
        {
            x = (i + 0.5) * step;
            sum = sum + 4.0 / (1.0 + x * x);
        }
        
        pi = step * sum;

        std::cout << "pi value is " << pi << std::endl;
    }


    // Sharing, parallel for (implicit barrier) + no wait
    // implied parallel region
    void Ex5_WorkSharingConstruct()
    {
        //#pragma omp master // means only the master do this
        //#pragma omp barrier // if you require the master to finish than put a barrier

        //#pragma omp single  // first thread that reach here will go off and do some work
        // implicit barrer after single.
        // #pragma omp single nowait means other threads don't need to wait.

        #pragma omp parallel
        {
            #pragma omp sections
            {
                #pragma omp section
                    x_calc();
                #pragma omp section
                    y_calc();
                #pragma omp section
                    z_calc();
            }
        }
    }

    void Ex6_RuntimeRoutine()
    {
        //omp_set_num_threads();
        omp_get_num_threads();
        omp_get_thread_num();
        omp_get_max_threads();  // i won't give you more threads than this.
        // am i calling this function in a parallel region?
        omp_in_parallel();

        // get number of actual cores on the system.
        omp_get_num_procs();
        // set the dynamic mode to dynamic or not
        //omp_set_dynamic();
        // am i in dynamic mode
        omp_get_dynamic();

        int num_threads;
        omp_set_dynamic(0); // turn off dynamic mode
        omp_set_num_threads(omp_get_num_procs());   // 1 thread per processor
        
        // getting num threads outside of parallel region will always net you 1.

        #pragma omp parallel
        {
            int id = omp_get_thread_num();
            #pragma omp single
                num_threads = omp_get_num_threads();
                dolotsofstuff(id);
        }
    }

    void ex7_environment_var()
    {
        /* https://www.youtube.com/watch?v=WXqicIq68-s&list=PLLX-Q6B8xqZ8n8bwjGdzBJ25X2utwnoEG&index=16
        OMP_NUM_THREADS -> int_literal
        OMP_STACKSIZE
        OMP_WAIT_POLICY -> ACTIVE|PASSIVE
        OMP_PROC_BIND -> TRUE|FALSE*/
    }
    
    //data environment
    // Changing storage attribute
    void ex8_changingVar()
    {
        double x = 0;
        #pragma omp parallel private(x) // private doesn't initialize the value.
        {
        }
        
        #pragma omp parallel shared(x)  // this is the default
        {
        }

        double initializedVal = 1;
        // create the private copy but initialize it to the global variable's value.
        #pragma omp parallel for firstprivate(initializedVal)
        for(int i = 0; i < 10; ++i)
        {
        }

        // retain the last value of that variable when we exit the construct
        // and copy the value to its global scope
        double globalVal = 5;
        #pragma omp parallel for lastprivate(globalVal)
        for (int i = 0; i < 10; ++i)
        {
        }
        // globalVal is now set to the value of the last term
        

        #pragma omp parallel for firstprivate(globalVal) lastprivate(globalVal)
        for (int i = 0; i < 10; ++i)
        {
        }

        // only legal in fortran not in c
        /*#pragma omp parallel default(private)
        {
        }*/
        // this is compiler default
        #pragma omp parallel default(shared)
        {
        }
        // this forces us to declare each and everyattribute
        // good for debugging
        #pragma omp parallel default(none)
        {
        }

    }

    void calculatePi_parallelWhenPossible()
    {
        Timer timer;

        double step = 0;
        #pragma parallel for private(x) reduction(+: sum)
        //int i = 0;
        double x = 0, pi = 0, sum = 0;
        step = 1.0 / num_steps;

        for (int i = 0; i < num_steps; ++i)
        {
            x = (i + 0.5) * step;
            sum = sum + 4.0 / (1.0 + x * x);
        }

        pi = step * sum;

        std::cout << "pi value is " << pi << std::endl;
    }

    private:
        void x_calc() {};
        void y_calc() {};
        void z_calc() {};
        void dolotsofstuff(int id) { };
};
