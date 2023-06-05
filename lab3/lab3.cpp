#include <iostream>
#include <windows.h>

struct Marker {
	int* array;
	int sizeArray;
	int numberThread;
	int numberNullElements = 0;
	int randomNumber;
	HANDLE hStartEvent;
	HANDLE hCantWorkEvent;
	HANDLE hContinueWorkEvent;
	HANDLE hEndWorkEvent;
	CRITICAL_SECTION* cs;
	Marker(int* a, int length, int num, HANDLE startEvent, HANDLE cantWorkEvent, HANDLE continueWorkEvent, HANDLE endWorkEvent, CRITICAL_SECTION* section) {
		sizeArray = length;
		array = a;
		numberThread = num;
		hStartEvent = startEvent;
		hCantWorkEvent = cantWorkEvent;
		hContinueWorkEvent = continueWorkEvent;
		hEndWorkEvent = endWorkEvent;
		cs = section;
	}
};

void output(int* a, int n) {
	for (int i = 0; i < n; i++) {
		std::cout << a[i] << " ";
	}
	std::cout << '\n';
}

bool findByValue(int* a, int n, int x) {
	for (int i = 0; i < n; i++) {
		if (a[i] == x) {
			return true;
		}
	}
	return false;
}

DWORD WINAPI markerFunc(LPVOID param1) {
	Marker* param = (Marker*)param1;
	WaitForSingleObject(param->hStartEvent, INFINITE);
	EnterCriticalSection(param->cs);
	std::cout << "Thread marker " << param->numberThread << " runs" << std::endl << std::endl;
	LeaveCriticalSection(param->cs);
	srand(param->numberThread);
	while (true) {
		EnterCriticalSection(param->cs);
		param->randomNumber = rand();
		param->randomNumber %= param->sizeArray;
		if (param->array[param->randomNumber] == 0) {
			Sleep(5);
			param->array[param->randomNumber] = param->numberThread;
			param->numberNullElements++;
			LeaveCriticalSection(param->cs);
			Sleep(5);
		}
		else {
			LeaveCriticalSection(param->cs);
			std::cout << "Impossible to continue!" << std::endl;
			std::cout << "Thread number: " << param->numberThread << std::endl;
			std::cout << "Marked elements: " << param->numberNullElements << std::endl;
			std::cout << "Impossible to mark element: " << param->randomNumber << std::endl << std::endl;
			SetEvent(param->hCantWorkEvent);
			HANDLE* events = new HANDLE[2];
			events[0] = param->hContinueWorkEvent;
			events[1] = param->hEndWorkEvent;
			DWORD dwValue = WaitForMultipleObjects(2, events, false, INFINITE);
			delete[] events;
			dwValue -= WAIT_OBJECT_0;
			if (dwValue == 1) {
				for (int i = 0; i < param ->sizeArray/*param->numberNullElements*/; i++) {
					if (param->array[i] == param->numberThread) {
						param->array[i] = 0;
					}
				}
				break;
			}
		}
	}
	std::cout << "Thread marker " << param->numberThread << " was stopped" << std::endl;
	return 0;
}

int main() {
	setlocale(LC_ALL, "rus");
	int n;
	int count;
	std::cout << "Input array capacity: ";
	std::cin >> n;
	int* arr = new int[n];
	for (int i = 0; i < n; i++) {
		arr[i] = 0;
	}
	std::cout << "Input marker threads count: ";
	std::cin >> count;
	int* num = new int[count];
	CRITICAL_SECTION* cs = new CRITICAL_SECTION();
	InitializeCriticalSection(cs);
	HANDLE hStartEvent = CreateEvent(NULL, true, false, NULL);
	if (hStartEvent == NULL) {
		return GetLastError();
	}
	HANDLE* hCantWorkEvent = new HANDLE[count];
	HANDLE hContinueWorkEvent = CreateEvent(NULL, true, false, NULL);
	if (hContinueWorkEvent == NULL) {
		return GetLastError();
	}
	HANDLE* hEndWorkEvent = new HANDLE[count];
	for (int i = 0; i < count; i++) {
		hCantWorkEvent[i] = CreateEvent(NULL, true, false, NULL);
		if (hCantWorkEvent[i] == NULL) {
			return GetLastError();
		}
		hEndWorkEvent[i] = CreateEvent(NULL, true, false, NULL);
		if (hEndWorkEvent[i] == NULL) {
			return GetLastError();
		}
	}
	HANDLE* hMarkerThread = new HANDLE[count];
	DWORD* IDMarkerThread = new DWORD[count];
	Marker** param1 = new Marker * [count];
	for (int i = 0; i < count; i++) {
		param1[i] = new Marker(arr, n, i + 1, hStartEvent, hCantWorkEvent[i], hContinueWorkEvent, hEndWorkEvent[i], cs);
		hMarkerThread[i] = CreateThread(NULL, 0, markerFunc, (void*)param1[i], NULL, &IDMarkerThread[i]);
		if (hMarkerThread[i] == NULL) {
			return GetLastError();
		}
	}
	SetEvent(hStartEvent);
	for (int i = 0; i < count; i++) {
		for (int j = 0; j < count; j++) {
			if (!findByValue(num, count, j + 1)) {
				ResetEvent(hCantWorkEvent[j]);
			}
		}
		WaitForMultipleObjects(count, hCantWorkEvent, true, INFINITE);
		output(arr, n);
		std::cout << "Input thread number you want to be stopped:" << std::endl;
		std::cin >> num[i];
		SetEvent(hEndWorkEvent[num[i] - 1]);
		WaitForSingleObject(hMarkerThread[num[i] - 1], INFINITE);
		std::cout << "Array:\n";
		output(arr, n);
		SetEvent(hContinueWorkEvent);
		ResetEvent(hContinueWorkEvent);
	}
	WaitForMultipleObjects(count, hMarkerThread, true, INFINITE);

	DeleteCriticalSection(cs);
	delete cs;
	delete[] arr;
	delete[] num;
	CloseHandle(hStartEvent);
	CloseHandle(hContinueWorkEvent);
	for (int i = 0; i < count; i++) {
		delete param1[i];
		CloseHandle(hCantWorkEvent[i]);
		CloseHandle(hEndWorkEvent[i]);
		CloseHandle(hMarkerThread[i]);
	}
	delete[] param1;
	delete[] hMarkerThread;
	delete[] IDMarkerThread;
	delete[] hCantWorkEvent;
	delete[] hEndWorkEvent;

	system("pause");
	return 0;
}
