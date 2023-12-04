#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>

std::ofstream logs("logs.txt");
DWORD timeOfStart;
SYSTEMTIME sysTime;
const int COUNT_TREADS = 2;


DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
    for (int i = 0; i < 20; i++)
    {
        Sleep(10);
        SYSTEMTIME curTime;
        GetSystemTime(&curTime);

        int firstTime = sysTime.wSecond * 1000 + sysTime.wMilliseconds;
        int secondTime = curTime.wSecond * 1000 + curTime.wMilliseconds;

        std::string dur = std::to_string(secondTime - firstTime);

        (int(lpParam) == 2) ? (logs << std::to_string(2) << "|" << dur << std::endl)
            : (logs << std::to_string(1) << "|" << dur << std::endl);
    }

    ExitThread(0);
}

int main()
{
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

    HANDLE* handles = new HANDLE[COUNT_TREADS];

    for (int i = 0; i < COUNT_TREADS; i++)
    {
        int ThreadCount = i + 1;
        handles[i] = CreateThread(NULL, 0, &ThreadProc, (LPVOID)ThreadCount, CREATE_SUSPENDED, NULL);

        SetThreadPriority(handles[i], i);
    }

    for (int i = 0; i < COUNT_TREADS; i++)
    {
        ResumeThread(handles[i]);
    }

    WaitForMultipleObjects(COUNT_TREADS, handles, true, INFINITE);
    return 0;

}
