#ifndef RESOURCE_H
#define RESOURCE_H

#include <iostream>

using namespace std;

class Resource{

public:

	~Resource();
	Resource(const unsigned int handle, const string& name, const string& path);

	string GetFileName();
	string GetName();
	string GetPath();

	unsigned long GetRefCount();
	

private:

	string filename;
	unsigned int handle;
	string name;
	string path;
	unsigned long refCount;

}



#endif 
