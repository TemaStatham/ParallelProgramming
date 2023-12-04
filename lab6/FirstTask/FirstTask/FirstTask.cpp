#include <iostream>
#include <omp.h>
#define NUM_THREADS 4

void CalculateSyncPI(const int times)
{
    double pi = 0;
    double start, end;

    start = omp_get_wtime();

    for (int i = 0; i < times; ++i)
    {
        pi = pi + 4 * pow(-1, i) / (2 * i + 1);
    }
    end = omp_get_wtime();
    printf("Approximation: %.15f\n", pi);
    printf("Time: %.6f\n\n", end - start);
}

void CalculateFalsePI(const int times)
{
    double start, end;
    double step = 1.0 / static_cast<double>(times);
    double pi = 0;

    start = omp_get_wtime();
#pragma omp parallel for
    for (int i = 0; i < times; ++i)
    {
        double x = (i + 0.5) * step;
        pi += 4.0 / (1.0 + x * x);
    }

    pi *= step;

    end = omp_get_wtime();
    printf("Approximation: %.15f\n", pi);
    printf("Time: %.6f\n\n", end - start);
}

void CalculateAtomicPI(const int times)
{
    double pi = 0;
    double start, end;

    start = omp_get_wtime();
#pragma omp parallel for
    for (int i = 0; i < times; ++i)
    {
        double term = 4 * pow(-1, i) / (2 * i + 1);

#pragma omp atomic
        pi += term;
    }
    end = omp_get_wtime();

    printf("Approximation: %.15f\n", pi);
    printf("Time: %.6f\n\n", end - start);
}

void CalculateReductionPI(const int times)
{
    double pi = 0;
    double start, end;

    start = omp_get_wtime();
#pragma omp parallel reduction(+:pi)
    {
#pragma omp for
        for (int i = 0; i < times; ++i)
        {
            pi = pi + 4 * pow(-1, i) / (2 * i + 1);
        }
    }
    end = omp_get_wtime();
    printf("Approximation: %.15f\n", pi);
    printf("Time: %.6f\n\n", end - start);
}

void PrivateX()
{
    omp_set_num_threads(NUM_THREADS);
    int x = 44;
#pragma omp parallel for private(x) num_threads(NUM_THREADS)
    for (int i = 0; i <= 10; i++) {
        x = i;
        printf("Thread number: %d x: %d\n", omp_get_thread_num(), x);
    }
    printf("x is %d\n\n", x);
}

void FirstPrivateX()
{
    omp_set_num_threads(NUM_THREADS);
    int x = 44;
#pragma omp parallel for firstprivate(x) num_threads(NUM_THREADS)
    for (int i = 0; i <= 10; i++) {
        x += i;
        printf("Thread number: %d x: %d\n", omp_get_thread_num(), x);
    }
    printf("x is %d\n\n", x);
}

void LastPrivateX()
{
    omp_set_num_threads(NUM_THREADS);
    int x = 44;
#pragma omp parallel for lastprivate(x) num_threads(NUM_THREADS)
    for (int i = 0; i <= 10; i++) {
        x += i;
        printf("Thread number: %d x: %d\n", omp_get_thread_num(), x);
    }
    printf("x is %d\n\n", x);
}


int main()
{
    const int times = 10'000'000;

    CalculateSyncPI(times);
    CalculateFalsePI(times);
    CalculateAtomicPI(times);
    CalculateReductionPI(times);

    PrivateX();
    FirstPrivateX();
    LastPrivateX();

    return 0;
}
