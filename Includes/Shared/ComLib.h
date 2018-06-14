/////////////////////////////////////////////////////////////
// UD1414 Assignment 1: Circular Buffer in Shared Memory
// Fredrik Linde TA15
// Comlib.h
/////////////////////////////////////////////////////////////

#ifndef COMLIB_H
#define COMLIB_H

#include <iostream>
#include <windows.h>
#include <string>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <string>
#include <stdlib.h>
#include <iomanip>

#include "Mutex.h"
#include "Messages.h"

// STRUCTS AND ENUMS FOR MESSAGE SENDING

enum TYPE { PRODUCER, CONSUMER };
enum MSG_TYPE { NORMAL, DUMMY };

struct Header
{
	MSG_TYPE msgId;		// 4 Bytes
	size_t msgSeq;		// 8 Bytes
	size_t msgLength;	// 8 Bytes
};

using namespace std;

class ComLib
{

public:

	// COMLIB

	// Create a ComLib
	ComLib(const std::string& mapFileName, const size_t& buffSize, TYPE type);
	ComLib();

	// Disconnect and destroy all resources 
	~ComLib();

	// File mapping is the association of a file's contents with a portion of the virtual address space of a process.
	void CreateSharedMemory(const size_t& buffSize);

	// A file view is the portion of virtual address space that a process uses to access the file's contents.
	void* CreateFileView(HANDLE filemap, const size_t& buffSize);

	// Initialize the pointers to the buffer
	void InitializeByteBufferPointers();

	// Update the memory and check free memory size
	void UpdateFreeMemory();

	// Create padding if message is not exactly a multipel of 64
	size_t CreatePadding(size_t msgSize);

	// Close file mapping handle and unmap the file view
	void Unmap();

	// Initialize and check status
	bool connect();
	bool isConnected();

	// Returns "true" if data was sent successfully.
	// Returns "false" if for any reason the data could not be sent.
	bool send(const void* msg, const size_t length);
	
	//returns: "true" if a message was received.
	//"false" if there is nothing to read, next message size is equal to zero
	bool recv(char* &msg, size_t& length);

	bool interpret(char* &msg);

	// Return the length of the next message
	size_t nextSize();

	// Mutex wrapped in class to lock critical code while reading
	Mutex myMutex;

private:

	// MAPPING VARIABLES
	HANDLE MapFile;		// Mapping file
	void* FileView;		// File View of the shared memory
	char* cBuffer;		// The Circular Buffer

	size_t counter = 0;
	size_t msgSize;

	size_t* head;	// Head pointer to head offset in the circular buffer
	size_t* tail;	// Tail pointer to tail offset in the circular buffer

	size_t memSize;		// Total memory size
	size_t freeMem;		// Free memory size

	LPCTSTR mapObjName;		// Name of the mapping file

};

#endif
