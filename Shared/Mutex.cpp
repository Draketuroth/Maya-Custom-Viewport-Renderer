/////////////////////////////////////////////////////////////
// Fredrik Linde TA15
// UD1414 Assignment 1: Circular Buffer in Shared Memory
// Mutex.cpp
/////////////////////////////////////////////////////////////

#include "Mutex.h"

Mutex::Mutex(const char* name) {

	// args(lpMutexAttributes, bInitialOwner, lpName)
	mutexHandle = CreateMutex(nullptr, false, (LPCTSTR)name);
}

Mutex::~Mutex() {

	ReleaseMutex(mutexHandle);
}

void Mutex::Lock(DWORD milliseconds) {

	WaitForSingleObject(mutexHandle, milliseconds);
}

void Mutex::Unlock() {

	ReleaseMutex(mutexHandle);
}