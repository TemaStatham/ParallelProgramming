#include "BMP.h"
#include <iostream>
#include <vector>
#include <windows.h>
#include <chrono>
#include <Windows.h>

//https://docs.google.com/spreadsheets/d/1aS7KNp8pTzuhQiGYq7jv51Eky0Rk868KVmetO-JdM0o/edit#gid=0

using namespace std;

const int kernelSize = 5;
const int countOfThread = 16;

struct paramsForBlur
{
    paramsForBlur() = default;

    int height = 0;
    int start = 0;
    int end = 0;
    std::vector<std::vector<Pixel>>& image;
    std::vector<std::vector<Pixel>>& blurredImage;

    /*void update(int h, int s, int e, std::vector<std::vector<Pixel>>& i, std::vector<std::vector<Pixel>>&b)
    {
        height = h;
        start = s;
        end = e;
        image = i;
        blurredImage = b;
    }*/

    paramsForBlur& operator=(paramsForBlur& other)
    {
        if (this == &other) return *this;

        height = other.height;
        start = other.start;
        end = other.end;
        image = other.image;
        blurredImage = other.blurredImage;

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

            for (int ky = -kernelSize; ky <= kernelSize; ++ky)
            {
                for (int kx = -kernelSize; kx <= kernelSize; ++kx)
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
        }
    }
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
    // 
    // ?? тут выполняется полезная работа потока
    //
    paramsForBlur* data = reinterpret_cast<paramsForBlur*>(lpParam);
  
    blurOfTheSite(*data);

    ExitThread(0); // функция устанавливает код завершения потока в 0
}

void applyBlur(std::vector<std::vector<Pixel>>& image, int kernelSize)
{
    int height = image.size();
    int width = image[0].size();

    std::vector<std::vector<Pixel>> blurredImage(height, std::vector<Pixel>(width));

    HANDLE* handles = new HANDLE[countOfThread];
    int columnsPerThread = width / countOfThread;
    std::vector<paramsForBlur> params;
    params.reserve(countOfThread);
    for (int i = 0; i < countOfThread; i++)
    {
        int startColumn = i * columnsPerThread;
        int endColumn = startColumn + columnsPerThread;

        if (endColumn > width)
        {
            endColumn = width;
        }

        paramsForBlur p{ height, startColumn, endColumn, image, blurredImage };
        params.push_back(p);
    }
   
    for (int i = 0; i < countOfThread; i++)
    {
        handles[i] = CreateThread(NULL, 0, &ThreadProc, &params[i], CREATE_SUSPENDED, NULL);

        if (NULL == handles[i])
        {
            std::cout << "error" << std::endl;
        }
    }

    for (int i = 0; i < countOfThread; i++)
    {
        ResumeThread(handles[i]);
        Sleep(200);
    }

    WaitForMultipleObjects(countOfThread, handles, true, INFINITE);
    
    image = blurredImage;
}

int main()
{
    HANDLE process = GetCurrentProcess();
    DWORD_PTR processAffinityMask = 15;//1,2,7,15
    BOOL success = SetProcessAffinityMask(process, processAffinityMask);

    if (!success)
    {
        std::cout << "set procces aff mask error" << std::endl;
        return 1;
    }

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    auto startTime = std::chrono::high_resolution_clock::now();

	Bitmap image;
	vector <vector <Pixel> > bmp;
	Pixel rgb;

	image.open("Airplane.bmp");
    bool validBmp = image.isImage();

    if (validBmp != true)
    {
        cout << "example.bmp is not image" << endl;
        return 1;
    }

    bmp = image.toPixelMatrix();

    applyBlur(bmp, kernelSize);

    image.fromPixelMatrix(bmp);
    image.save("output.bmp");

    auto endTime = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "Время выполнения программы: " << duration.count() << " мс" << std::endl;
    return 0;
}
