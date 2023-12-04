#include <windows.h>
#include <string>
#include <iostream>
#include "tchar.h"
#include <fstream>

HANDLE Mutex;

int ReadFromFile() {
    WaitForSingleObject(Mutex, INFINITE);
    std::fstream myfile("balance.txt", std::ios_base::in);
    int result;
    myfile >> result;
    myfile.close();
    ReleaseMutex(Mutex);

    return result;
}

void WriteToFile(int data) {
    WaitForSingleObject(Mutex, INFINITE);
    std::fstream myfile("balance.txt", std::ios_base::out);
    myfile << data << std::endl;
    myfile.close();
    ReleaseMutex(Mutex);
}

int GetBalance() {
    int balance = ReadFromFile();
    return balance;
}

void Deposit(int money) {
    WaitForSingleObject(Mutex, INFINITE);
    int balance = ReadFromFile();
    balance += money;
    WriteToFile(balance);
    ReleaseMutex(Mutex);

    printf("Balance after deposit: %d\n", balance);
}

void Withdraw(int money) {
    WaitForSingleObject(Mutex, INFINITE);
    int balance = ReadFromFile();
    if (balance < money) {
        printf("Cannot withdraw money, balance lower than %d\n", money);
    }
    else {
        balance -= money;
        WriteToFile(balance);
        printf("Balance after withdraw: %d\n", balance);
    }
    ReleaseMutex(Mutex);
}

DWORD WINAPI DoDeposit(CONST LPVOID lpParameter)
{
    Deposit((int)lpParameter);
    ExitThread(0);
}

DWORD WINAPI DoWithdraw(CONST LPVOID lpParameter)
{
    Withdraw((int)lpParameter);
    ExitThread(0);
}

int _tmain(int argc, _TCHAR* argv[])
{
    Mutex = CreateMutex(NULL, FALSE, NULL);

    WriteToFile(0);

    SetProcessAffinityMask(GetCurrentProcess(), 1);
    HANDLE* handles = new HANDLE[50];

    for (int i = 0; i < 50; i++) {
        handles[i] = (i % 2 == 0)
            ? CreateThread(NULL, 0, &DoDeposit, (LPVOID)230, CREATE_SUSPENDED, NULL)
            : CreateThread(NULL, 0, &DoWithdraw, (LPVOID)1000, CREATE_SUSPENDED, NULL);
        ResumeThread(handles[i]);
    }

    // Ожидание окончания работы всех потоков
    WaitForMultipleObjects(50, handles, TRUE, INFINITE);
    printf("Final Balance: %d\n", GetBalance());

    getchar();

    CloseHandle(Mutex);

    return 0;
}
