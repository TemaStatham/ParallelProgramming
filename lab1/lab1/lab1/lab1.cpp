//#include "stdafx.h"
#include <windows.h>
#include <string>
#include <iostream>
#include <vector>

const int N = 10;

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	// 
	// ?? тут выполняется полезная работа потока
	//
	int data = reinterpret_cast<int>(lpParam);
	std::cout << data << std::endl;
	ExitThread(0); // функция устанавливает код завершения потока в 0
}

//using namespace System;

int _tmain(int argc, TCHAR* argv[])
{
	// создание двух потоков

	HANDLE* handles = new HANDLE[N];

	std::vector<int>a(N);

	for (int i = 1; i <= N; i++) {
		//Object^ O = i;
		a[i-1] = i;
		int i_ = i;

		handles[i - 1] = CreateThread(NULL, 0, &ThreadProc, (LPVOID)i_, CREATE_SUSPENDED, NULL);
		if (NULL == handles[i]) {
			std::cout << "error" <<std::endl;
		}
	}
	// запуск двух потоков

	for (int i = 1; i <= N; i++) {
		ResumeThread(handles[i-1]);
	}

	// ожидание окончания работы двух потоков
	WaitForMultipleObjects(N, handles, true, INFINITE);
	return 0;
}

int main(int argc, TCHAR* argv[]) {
	_tmain(argc, argv);
}
