#include "BMP.h"
#include <iostream>
#include <vector>
#include <windows.h>
#include <chrono>
#include <Windows.h>
#include <stdio.h>
#include <map>
#include <fstream>

using namespace std;

const int KERNEL_SIZE = 5;
const int COUNT_TREADS = 3;
DWORD timeOfStart;
SYSTEMTIME sysTime;

struct paramsForBlur
{
    paramsForBlur() = default;

    int height = 0;
    int start = 0;
    int end = 0;
    std::vector<std::vector<Pixel>>& image;
    std::vector<std::vector<Pixel>>& blurredImage;

    int countThread;
    FILE* pFile;

    paramsForBlur& operator=(paramsForBlur& other)
    {
        if (this == &other) return *this;

        height = other.height;
        start = other.start;
        end = other.end;
        image = other.image;
        blurredImage = other.blurredImage;
        countThread = other.countThread;

        return *this;
    }
};

void blurOfTheSite(paramsForBlur& p)
{
    for (int y = 0; y < p.height; ++y)
    {
        for (int x = p.start; x < p.end; ++x)
        {
            int totalRed = 0, totalGreen = 0, totalBlue = 0;
            int count = 0;

            for (int ky = -KERNEL_SIZE; ky <= COUNT_TREADS; ++ky)
            {
                for (int kx = -COUNT_TREADS; kx <= COUNT_TREADS; ++kx)
                {
                    int newX = x + kx;
                    int newY = y + ky;

                    if (newX >= 0 && newX < p.end && newY >= 0 && newY < p.height)
                    {
                        totalRed += p.image[newY][newX].red;
                        totalGreen += p.image[newY][newX].green;
                        totalBlue += p.image[newY][newX].blue;
                        ++count;
                    }
                }
            }

            // Усреднение цветов пикселей
            int avgRed = totalRed / count;
            int avgGreen = totalGreen / count;
            int avgBlue = totalBlue / count;

            p.blurredImage[y][x].set(avgRed, avgGreen, avgBlue);

            SYSTEMTIME curTime;
            GetSystemTime(&curTime);

            int firstTime = sysTime.wSecond * 1000 + sysTime.wMilliseconds;
            int secondTime = curTime.wSecond * 1000 + curTime.wMilliseconds;

            auto dur = secondTime - firstTime;

            fprintf(p.pFile, "%d %d\n", p.countThread, dur);
        }
    }
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
    paramsForBlur* data = reinterpret_cast<paramsForBlur*>(lpParam);

    blurOfTheSite(*data);

    ExitThread(0); // функция устанавливает код завершения потока в 0
}

void applyBlur(std::vector<std::vector<Pixel>>& image, int kernelSize, FILE* pFile)
{
    int height = image.size();
    int width = image[0].size();

    std::vector<std::vector<Pixel>> blurredImage(height, std::vector<Pixel>(width));

    HANDLE* handles = new HANDLE[COUNT_TREADS];
    int columnsPerThread = width / COUNT_TREADS;
    std::vector<paramsForBlur> params;
    params.reserve(COUNT_TREADS);

    for (int i = 0; i < COUNT_TREADS; i++)
    {
        int startColumn = i * columnsPerThread;
        int endColumn = startColumn + columnsPerThread;

        if (endColumn > width)
        {
            endColumn = width;
        }

        paramsForBlur p{ height, startColumn, endColumn, image, blurredImage, i, pFile };
        params.push_back(p);
    }

    int a = 1;
    std::vector<int> mask{ 1, 0, 0 };
    for (int i = 0; i < COUNT_TREADS; i++)
    {
        handles[i] = CreateThread(NULL, 0, &ThreadProc, &params[i], CREATE_SUSPENDED, NULL);
        if (NULL == handles[i])
        {
            std::cout << "error" << std::endl;
        }

        SetThreadPriority(handles[i], mask[i]);
    }

    for (int i = 0; i < COUNT_TREADS; i++)
    {
        ResumeThread(handles[i]);
    }

    WaitForMultipleObjects(COUNT_TREADS, handles, true, INFINITE);

    image = blurredImage;
}

int main()
{
    setlocale(LC_ALL, "rus");

    FILE* pFile;
    fopen_s(&pFile, "logs.txt", "w");

    HANDLE process = GetCurrentProcess();
    DWORD_PTR processAffinityMask = 1;//1,2,7,15
    BOOL success = SetProcessAffinityMask(process, processAffinityMask);

    if (!success)
    {
        std::cout << "set procces aff mask error" << std::endl;
        return 1;
    }

    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);

    system("pause");

    timeOfStart = GetTickCount64();
    GetSystemTime(&sysTime);

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    auto startTime = std::chrono::high_resolution_clock::now();

    Bitmap image;
    vector <vector <Pixel> > bmp;
    Pixel rgb;

    image.open("Car.bmp");
    bool validBmp = image.isImage();

    if (validBmp != true)
    {
        cout << "example.bmp is not image" << endl;
        return 1;
    }

    bmp = image.toPixelMatrix();

    applyBlur(bmp, KERNEL_SIZE, pFile);

    image.fromPixelMatrix(bmp);
    image.save("output.bmp");

    auto endTime = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "Время выполнения программы: " << duration.count() << " мс" << std::endl;
    return 0;
}
