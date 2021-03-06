/*
Laboratory work on the topic "WinAPI. Semaphores, Mutexes, Events, Critical sections"
Variant: A  = d*B(MO*MK)-e*Sort(z)
Author: Poshtatska Kateryna IO-51

*/

//The specifications of main program Lab2

#include "stdafx.h"
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <ctime>

using namespace std;

void TF1(void);
void TF2(void);
void TF3(void);
void TF4(void);

HANDLE IEvent[3]; //Events for Input Sync

HANDLE SEvent[4]; 

HANDLE SortE1 = CreateEvent(0, 1, 0, 0); //Event for Sort Sync
HANDLE SortE = CreateEvent(0, 1, 0, 0); //Event for Sort Sync

HANDLE OSem; //Semaphore for Output Sync

CRITICAL_SECTION CS; //Critical section for shared resource 

HANDLE Mut1; //Mutex for shared resource  

HANDLE Sem1; //Semaphore for shared resource 
HANDLE Sem2; //Semaphore for shared resource 

int n = 20;// size
int P = 4; //processors count
int H = n / P; //subsize

			   //shared resources and global variables
int d;
int e;
int* A = new int[n];
int* B = new int[n];
int* Z = new int[n];
int** MO = new int*[n];
int** MK = new int*[n];
int** MR = new int*[n];


//Main method of Lab2
int main(void)
{
	cout << "Lab2 started" << endl;

	DWORD TidA, TidB, TidC, TidD;
	HANDLE hThreadA, hThreadB, hThreadC, hThreadD;

	/*
	Variable n using for rhe size of all matrix and vectors in program.
	data is an exeplar of class Data with parametrs n and p.
	*/
	for (int i = 0; i < n; i++)
	{
		MO[i] = new int[n];
		MK[i] = new int[n];
		MR[i] = new int[n];
	}

	//Announcement of threads with their parametrs. Getting their handles
	hThreadA = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF1, NULL, 0, &TidA);
	hThreadB = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF2, NULL, 0, &TidB);
	hThreadC = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF3, NULL, 0, &TidC);
	hThreadD = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF4, NULL, 0, &TidD);

	//Setting priority of threads
	SetThreadPriority(hThreadA, THREAD_PRIORITY_LOWEST);
	SetThreadPriority(hThreadB, THREAD_PRIORITY_NORMAL);
	SetThreadPriority(hThreadC, THREAD_PRIORITY_HIGHEST);
	SetThreadPriority(hThreadD, THREAD_PRIORITY_HIGHEST);

	//Announcement of events with their parametrs. Getting their handles
	for (int i = 0; i < 3; i++) {
		IEvent[i] = CreateEvent(0, 1, 0, 0);
		SEvent[i] = CreateEvent(0, 1, 0, 0);
	}
	SEvent[3] = CreateEvent(0, 1, 0, 0);

	//initialize of critical section
	InitializeCriticalSection(&CS);

	//Announcement of mutex with it's parametrs. Getting it's handle
	Mut1 = CreateMutex(0, 0, 0);

	//Announcement of semaphore with it's parametrs. Getting it's handle
	Sem1 = CreateSemaphore(0, 1, 1, 0);
	Sem2 = CreateSemaphore(0, 1, 1, 0);
	OSem = CreateSemaphore(0, 0, 3, 0);

	//Waiting the end of running thread
	WaitForSingleObject(hThreadA, INFINITE);
	WaitForSingleObject(hThreadB, INFINITE);
	WaitForSingleObject(hThreadC, INFINITE);
	WaitForSingleObject(hThreadD, INFINITE);

	//Closing the descriptors
	CloseHandle(hThreadA);
	CloseHandle(hThreadB);
	CloseHandle(hThreadC);
	CloseHandle(hThreadD);

	for (int i = 0; i < 3; i++) {
		CloseHandle(IEvent[i]);
		CloseHandle(SEvent[i]);
	}
	CloseHandle(SEvent[3]);

	CloseHandle(SortE);
	CloseHandle(SortE1);
	CloseHandle(Sem1);
	CloseHandle(Sem2);
	CloseHandle(OSem);
	CloseHandle(Mut1);
	DeleteCriticalSection(&CS);

	cout << "Lab2 finished" << endl;

	system("pause");
}

//Thread function TF1 
void TF1(void) {
	int d1;
	int e1;
	int* B1;
	int* X = Z;
	int** MK1;
	cout << "TF1 started" << endl;
	//srand((unsigned)time(NULL));
	for (int i = 0; i < n; i++) {
		Z[i] = 1;
	}
	Z[0] = -1;
	Z[1] = 2;
	Z[2] = 1;
	Z[3] = -3;

	cout << "Z = " << endl;
	for (int i = 0; i < n; i++) {
		cout << Z[i] << " ";
	}

	e = 1;

	//Input sync
	SetEvent(IEvent[0]);
	WaitForMultipleObjects(3, IEvent, TRUE, INFINITE);

	bool Finished;
	int Temp = 0;
	for (int i = 0; i < H; i++) {
		Finished = true;
		for (int j = 1; j < (H - i); j++) {
			if (Z[j - 1] > Z[j]) {
				Temp = Z[j - 1];
				Z[j - 1] = Z[j];
				Z[j] = Temp;
				Finished = false;
			}
		}
		if (Finished) {
			break;
		}
	}

	SetEvent(SEvent[0]);
	WaitForMultipleObjects(4, SEvent, TRUE, INFINITE);
	X = new int[n];
	int num = 0;
	int* C = new int[2] {0, H};
	for (int i = 0; i < 2*H; i++) {
		Temp = INT_MAX;
		for (int j = 0; j < 2; j++) {
			if ((C[j] - H*j)<H) {
				if (Z[C[j]] < Temp) {
					num = j;
					Temp = Z[C[j]];
				}
			}
		}
		X[i] = Temp;
		C[num]++;
	}
	for (int i = 0; i < 2*H; i++) {
		Z[i] = X[i];
	}

	WaitForSingleObject(SortE1, INFINITE);

	X = new int[n];
	num = 0;
	C = new int[P] {0, 2 * H};
	for (int i = 0; i < n; i++) {
		Temp = INT_MAX;
		for (int j = 0; j < 2; j++) {
			if ((C[j] - 2*H * j)<2*H) {
				if (Z[C[j]] < Temp) {
					num = j;
					Temp = Z[C[j]];
				}
			}
		}
		X[i] = Temp;
		C[num]++;
	}
	for (int i = 0; i < n; i++) {
		Z[i] = X[i];
	}

	SetEvent(SortE);

	//Coping shared resource
	WaitForSingleObject(Sem1, INFINITE);
	d1 = d;
	ReleaseSemaphore(Sem1, 1, 0);

	WaitForSingleObject(Sem2, INFINITE);
	MK1 = MK;
	ReleaseSemaphore(Sem2, 1, 0);

	WaitForSingleObject(Mut1, INFINITE);
	e1 = e;
	ReleaseMutex(Mut1);

	EnterCriticalSection(&CS);
	B1 = B;
	LeaveCriticalSection(&CS);

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < n; j++) {
			MR[i][j] = 0;
			for (int k = 0; k < n; k++) {
				MR[i][j] += MO[i][k] * MK1[k][j];
			}
			MR[i][j] *= d1;
		}
	}

	for (int i = 0; i < H; i++)
	{
		A[i] = 0;
		for (int j = 0; j < n; j++)
		{
			A[i] += MR[i][j] * B1[j];
		}
		A[i] -= e1 * Z[i];
	}

	//Output sync
	WaitForSingleObject(OSem, INFINITE);
	WaitForSingleObject(OSem, INFINITE);
	WaitForSingleObject(OSem, INFINITE);
	cout << "A = " << endl;
	for (int i = 0; i < n; i++) {
		cout << A[i] << " ";
	}
	cout << endl;
	cout << "\nTF1 finished" << endl;
}

//Thread function TF2
void TF2(void) {
	int d2;
	int e2;
	int* B2;
	int** MK2;
	cout << "TF2 started" << endl;
	for (int i = 0; i < n; i++) {
		B[i] = 1;
		for (int j = 0; j < n; j++) {
			MK[i][j] = 1;
		}
	}

	//Input sync
	SetEvent(IEvent[1]);
	WaitForMultipleObjects(3, IEvent, TRUE, INFINITE);

	bool Finished;
	int Temp = 0;
	for (int i = 0; i < H; i++) {
		Finished = true;
		for (int j = 1; j<(H - i); j++) {
			if (Z[j - 1 + H] > Z[j + H]) {
				Temp = Z[j - 1 + H];
				Z[j - 1 + H] = Z[j + H];
				Z[j + H] = Temp;
				Finished = false;
			}
		}
		if (Finished) {
			break;
		}
	}

	SetEvent(SEvent[1]);

	WaitForSingleObject(SortE, INFINITE);

	//Coping shared resource
	WaitForSingleObject(Sem1, INFINITE);
	d2 = d;
	ReleaseSemaphore(Sem1, 1, 0);

	WaitForSingleObject(Sem2, INFINITE);
	MK2 = MK;
	ReleaseSemaphore(Sem2, 1, 0);

	WaitForSingleObject(Mut1, INFINITE);
	e2 = e;
	ReleaseMutex(Mut1);

	EnterCriticalSection(&CS);
	B2 = B;
	LeaveCriticalSection(&CS);

	for (int i = H; i < 2 * H; i++) {
		for (int j = 0; j < n; j++) {
			MR[i][j] = 0;
			for (int k = 0; k < n; k++) {
				MR[i][j] += MO[i][k] * MK2[k][j];
			}
			MR[i][j] *= d2;
		}
	}

	for (int i = H; i < 2 * H; i++)
	{
		A[i] = 0;
		for (int j = 0; j < n; j++)
		{
			A[i] += MR[i][j] * B2[j];
		}
		A[i] -= e2 * Z[i];
	}
	//Output sync
	ReleaseSemaphore(OSem, 1, 0);
	cout << "TF2 finished" << endl;
}

//Thread function TF3
void TF3(void) {
	int d3;
	int e3;
	int* B3;
	int** MK3;
	cout << "TF3 started" << endl;

	//Input sync
	WaitForMultipleObjects(3, IEvent, TRUE, INFINITE);

	bool Finished;
	int Temp = 0;
	for (int i = 0; i < H; i++) {
		Finished = true;
		for (int j = 1; j < (H - i); j++) {
			if (Z[j - 1 + 2 * H] > Z[j + 2 * H]) {
				Temp = Z[j - 1 + 2 * H];
				Z[j - 1 + 2 * H] = Z[j + 2 * H];
				Z[j + 2 * H] = Temp;
				Finished = false;
			}
		}
		if (Finished) {
			break;
		}
	}

	SetEvent(SEvent[2]);
	WaitForMultipleObjects(4, SEvent, TRUE, INFINITE);

	int* X = new int[n];
	int num = 0;
	int* C = new int[2]{ 2*H, 3*H };
	for (int i = n / 2; i < n; i++) {
		Temp = INT_MAX;
		for (int j = 0; j < 2; j++) {
			if ((C[j] - H * (j+2))<H) {
				if (Z[C[j]] < Temp) {
					num = j;
					Temp = Z[C[j]];
				}
			}
		}
		X[i] = Temp;
		C[num]++;
	}
	for (int i = n / 2; i < n; i++) {
		Z[i] = X[i];
	}

	SetEvent(SortE1);

	WaitForSingleObject(SortE, INFINITE);

	//Coping shared resource
	WaitForSingleObject(Sem1, INFINITE);
	d3 = d;
	ReleaseSemaphore(Sem1, 1, 0);

	WaitForSingleObject(Sem2, INFINITE);
	MK3 = MK;
	ReleaseSemaphore(Sem2, 1, 0);

	WaitForSingleObject(Mut1, INFINITE);
	e3 = e;
	ReleaseMutex(Mut1);

	EnterCriticalSection(&CS);
	B3 = B;
	LeaveCriticalSection(&CS);

	for (int i = 2 * H; i < 3 * H; i++) {
		for (int j = 0; j < n; j++) {
			MR[i][j] = 0;
			for (int k = 0; k < n; k++) {
				MR[i][j] += MO[i][k] * MK3[k][j];
			}
			MR[i][j] *= d3;
		}
	}

	for (int i = 2 * H; i < 3 * H; i++)
	{
		A[i] = 0;
		for (int j = 0; j < n; j++)
		{
			A[i] += MR[i][j] * B3[j];
		}
		A[i] -= e3 * Z[i];
	}
	//Output sync
	ReleaseSemaphore(OSem, 1, 0);
	cout << "TF3 finished" << endl;
}

//Thread function TF4
void TF4(void) {
	int d4;
	int e4;
	int* B4;
	int** MK4;
	cout << "TF4 started" << endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			MO[i][j] = 1;
		}
	}
	d = 1;

	//Input sync
	SetEvent(IEvent[2]);
	WaitForMultipleObjects(3, IEvent, TRUE, INFINITE);

	bool Finished;
	int Temp = 0;
	for (int i = 0; i < H; i++) {
		Finished = true;
		for (int j = 1; j < (H - i); j++) {
			if (Z[j - 1 + 3 * H] > Z[j + 3 * H]) {
				Temp = Z[j - 1 + 3 * H];
				Z[j - 1 + 3 * H] = Z[j + 3 * H];
				Z[j + 3 * H] = Temp;
				Finished = false;
			}
		}
		if (Finished) {
			break;
		}
	}

	SetEvent(SEvent[3]);

	WaitForSingleObject(SortE, INFINITE);

	//Coping shared resource
	WaitForSingleObject(Sem1, INFINITE);
	d4 = d;
	ReleaseSemaphore(Sem1, 1, 0);

	WaitForSingleObject(Sem2, INFINITE);
	MK4 = MK;
	ReleaseSemaphore(Sem2, 1, 0);

	WaitForSingleObject(Mut1, INFINITE);
	e4 = e;
	ReleaseMutex(Mut1);

	EnterCriticalSection(&CS);
	B4 = B;
	LeaveCriticalSection(&CS);

	for (int i = 3 * H; i < 4 * H; i++) {
		for (int j = 0; j < n; j++) {
			MR[i][j] = 0;
			for (int k = 0; k < n; k++) {
				MR[i][j] += MO[i][k] * MK4[k][j];
			}
			MR[i][j] *= d4;
		}
	}

	for (int i = 3 * H; i < 4 * H; i++)
	{
		A[i] = 0;
		for (int j = 0; j < n; j++)
		{
			A[i] += MR[i][j] * B4[j];
		}
		A[i] -= e4 * Z[i];
	}
	//Output sync
	ReleaseSemaphore(OSem, 1, 0);
	cout << "TF4 finished" << endl;
}