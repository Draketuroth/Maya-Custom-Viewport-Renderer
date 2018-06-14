/////////////////////////////////////////////////////////////
// Fredrik Linde TA15
// UD1414 Assignment 1: Circular Buffer in Shared Memory
// Comlib.cpp
/////////////////////////////////////////////////////////////

#include "ComLib.h"

ComLib::ComLib(const std::string& mapFileName, const size_t& buffSize, TYPE type) {

	// Set the mapping file name from input
	mapObjName = (LPCTSTR)mapFileName.c_str();

	// We open the memory and set the control buffer in the beginning, containing the pointers. 
	// I must define a real beginning which is the total offset in byte size of the two pointers
	// Only I will know of this offset in memory
	CreateSharedMemory(buffSize);

	msgSize = 200;

}

ComLib::ComLib() {

	
}

ComLib::~ComLib() {


}

void ComLib::CreateSharedMemory(const size_t& buffSize) {

	size_t ctrlBuffSize = sizeof(size_t) * 2;

	// Get the total memory in bytes (same as the buffer size)
	memSize = buffSize;

	MapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,		// use paging file. Pass INVALID_HANDLE_VALUE to CreateFileMapping so that the memory is backed by the paging file and you can set any size you need.
		NULL,						// default security
		PAGE_READWRITE,				// read/write access
		0,							// maximum object size (high-order DWORD)
		buffSize + ctrlBuffSize,	// maximum object size (low-order DWORD)
		mapObjName);				// name of mapping object


	// Check if file map is NULL
	if (MapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());

			exit(EXIT_FAILURE);
	}

	// File map already exists, we only want a view and cast it to a char*
	if (GetLastError() == ERROR_ALREADY_EXISTS) {

		// Create the view (will return void) and convert that char pointer
		FileView = CreateFileView(MapFile, buffSize);
		cBuffer = (char*)FileView;

		// Get the pointer addresses 
		head = (size_t*)(cBuffer);
		tail = ((size_t*)(cBuffer) + 1);
	}

	// File map doesn't already exist, which means this process is first. Only initialize pointers in this case
	else {

		// Create the view (will return void) and convert that char pointer
		FileView = CreateFileView(MapFile, buffSize);
		cBuffer = (char*)FileView;

		// Get the pointer addresses 
		head = (size_t*)(cBuffer);
		tail = ((size_t*)(cBuffer) + 1);

		// Initialize pointers only if this process is the first
		InitializeByteBufferPointers();
	}
}

void* ComLib::CreateFileView(HANDLE filemap, const size_t& buffSize) {

	// A file view can consist of all or only part of the file mapping object.
	// When multiple processes use the same file mapping object to create views for a local file, the data is coherent. That is, the views contain identical copies of the file on disk.

	void* fileView;

	fileView = (LPTSTR)MapViewOfFile(filemap,   // handle to map object
		FILE_MAP_ALL_ACCESS,				  // read/write permission
		0,
		0,
		buffSize);

	if (fileView == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(MapFile);

		return NULL;
	}

	return fileView;
}

void ComLib::InitializeByteBufferPointers() {

	// Address to head and tail pointer (PUT THESE FIRST IN SHARED MEMORY)
	// The control buffer at the start of the memory is made up by two size_t pointers

	*head = sizeof(size_t) * 2;
	*tail = sizeof(size_t) * 2;
}

void ComLib::UpdateFreeMemory() {

	// Is head in front of tail?
	if (*head > *tail) {

		freeMem = (memSize - *head + *tail) - (sizeof(size_t) * 2);

	}

	// Is tail in front of head?
	else if (*tail > *head) {

		freeMem = (*tail - *head) - (sizeof(size_t) * 2);
	}

	// Otherwise...? Is it full or is it empty? 
	// If we can assume that the consumer can never get past the producer, it will be the same as the full memory
	else {

		freeMem = memSize;
	}
}

size_t ComLib::CreatePadding(size_t msgSize) {

	size_t padding = 0;

	// Get the multiplyer by dividing the total message size in bytes by 64
	size_t mul = (size_t)ceil((msgSize / 64.0f));

	// Calculate the padding for the header pointer
	size_t pad = (64 * mul) - msgSize;

	// Offset the offset value in the head pointer
	padding = msgSize + pad;

	//cout << padding << endl;

	return padding;
}

void ComLib::Unmap() {

	UnmapViewOfFile(FileView);
	CloseHandle(MapFile);
}

bool ComLib::connect() {

	return true;
}

bool ComLib::isConnected() {

	return true;
}

bool ComLib::send(const void* msg, const size_t length) {

	// Lock and enter critical code
	myMutex.Lock();

	///////////////////////////////////////////////////
	// Step 1. Get size of next message to send
	///////////////////////////////////////////////////

	// Must also pad the next message to get the correct size
	size_t messageSize = sizeof(Header) + length;

	if (messageSize % 64 != 0) {

		messageSize = CreatePadding(messageSize);

	}

	///////////////////////////////////////////////////
	// Step 2. Check free space
	///////////////////////////////////////////////////

	UpdateFreeMemory();

	///////////////////////////////////////////////////
	// Step 3. Check if message fits into free memory
	// Otherwise return false
	///////////////////////////////////////////////////

	// We don't want the producer to land on the same offset as the consumer, 
	// so we want message size to be a bit smaller than the free memory
	if (messageSize < freeMem) {

		////////////////////////////////////////////////////////////////
		// Step 4. Determine whether to write normal or dummy message
		////////////////////////////////////////////////////////////////

		// The only case we can loop
		if (*tail > *head || memSize - *head >= messageSize) {

			// Incremate message counter
			counter++;

			// Create a header for the NORMAL message
			Header h = { MSG_TYPE::NORMAL, counter, length };

			// Copy header to memory
			memcpy(&cBuffer[*head], &h, sizeof(Header));

			// Copy payload message to memory
			memcpy(&cBuffer[*head] + sizeof(Header), msg, length);

			//cout << "Message size:" << length << endl;

			// Offset the head offset
			*head += messageSize % memSize;

			// If head is now zero, add the offset from the control buffer
			if (*head == 0) {

				*head += sizeof(size_t) * 2;
			}

			myMutex.Unlock();
			return true;
		}

		// Here we must create a dummy message. We will add a header message, given that the message is smaller than the tail offset
		else if (messageSize < *tail) {

			//cout << "Message size was smaller than tail" << endl;

			// Create a header for the DUMMY message
			// Dummy message size must be larger than 0
			Header h = { MSG_TYPE::DUMMY, 0, 1 };

			// Copy header to memory
			memcpy(&cBuffer[*head], &h, sizeof(Header));

			// Set the head to the beginning of the buffer
			*head = sizeof(size_t) * 2;

			myMutex.Unlock();
			// Send the actual message
			send(msg, length);

		}

		// There is no place for message, consumer must read memory
		else {

			//cout << "Coult not send message, no place for dummy or normal" << endl;

			myMutex.Unlock();
			return false;
		}

	}

	else {

		//cout << "Coult not send message at all" << endl;

		myMutex.Unlock();
		return false;
	}

}

bool ComLib::recv(char* &msg, size_t& length) {
	
	// Lock and enter critical code
	myMutex.Lock();

	///////////////////////////////////////////////////
	// Step 1. Consume as long as there is free memory
	///////////////////////////////////////////////////
	
	// As long as tail stands on the same offset as the head, it means that the buffer is empty and that the producer must write more messages
	// before the consumer tries to read anything. We must not allow length to be b

	if (*tail != *head && length != 0){

		///////////////////////////////////////////////////
		// Step 2. Interpret the message type
		///////////////////////////////////////////////////
		size_t type = ((Header*)(cBuffer + (*tail)))->msgId;

		if (type == 0){	// This is a normal message

			// Get the sequence number 
			size_t seq = ((Header*)(cBuffer + (*tail)))->msgSeq;

			// Copy message from the buffer into the char pointer
			memcpy(msg, &cBuffer[*tail] + sizeof(Header), length);

			// Tail must now be added the offset of the entire message, since we only wrote the length of the message 
			size_t messageSize = sizeof(Header) + length;

			if (messageSize % 64 != 0) {

				messageSize = CreatePadding(messageSize);

			}

			// Offset the tail offset
			*tail += messageSize % memSize;

			// If tail is now zero, add the offset from the control buffer
			if (*tail == 0) {

				*tail += sizeof(size_t) * 2;
			}

			UpdateFreeMemory();

			myMutex.Unlock();
			return true;


		}

		else if(type == 1) {	// This is a dummy message

			//cout << "Read dummy message and start over" << endl;

			// This was a dummy message, start reading from the beginning of the buffer
			*tail = 2 * sizeof(size_t);

			UpdateFreeMemory();

			myMutex.Unlock();
			return false;

		}

		else {

			//cout << "Invalid message type" << endl;

			myMutex.Unlock();
			return false;

		}
	}

	else {

		//cout << "On exit consumer" << endl;
		myMutex.Unlock();
		return false;
	}
}

size_t ComLib::nextSize() {

	///////////////////////////////////////////////////
	// Step 1. Get the header for the next message
	///////////////////////////////////////////////////

	// Only return the length of the next message if the tail and head offsets are different
	if(*tail != *head){

		Header* h = (Header*)(cBuffer + (*tail));

		///////////////////////////////////////////////////
		// Step 2. Get the length of the message
		///////////////////////////////////////////////////

		size_t dataBlock = h->msgLength;

		//cout << "Next size is: " << dataBlock << endl;

		return dataBlock;

	}

	// Otherwise, return 0
	else {

		return 0;
	}
}