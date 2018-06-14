/////////////////////////////////////////////////////////////
// Fredrik Linde TA15
// UD1414 Assignment 1: Circular Buffer in Shared Memory
// Mutex.h
/////////////////////////////////////////////////////////////

#ifndef MUTEX_H
#define MUTEX_H

#include <windows.h>

class Mutex {

public:

	// Creates a named mutex object with default name
	Mutex(const char* name = "myMutex");

	// Releases ownership of the mutex object
	~Mutex();

	// Locks a certain ection of code and protects shared memory from simultaneous access
	void Lock(DWORD milliseconds = INFINITE);

	// Same as in the destructor, it releases ownership of the mutex object
	void Unlock();

private:

	HANDLE mutexHandle;
};

#endif
