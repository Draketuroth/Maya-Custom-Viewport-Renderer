#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <iostream>

using namespace std;

template <typename T>

class ResourceManager {

public:

	~ResourceManager();

	void(*) CreateResource(T**, const unsigned int, const string& name, const string& path);

	unsigned int Add(const string& name, const string& path);

	void EmptyList();

	T* GetElement(const int handle);
	T* GetElement(const string& name, const string& path);
	vector<T*>* GetList();

private:

	stack<unsigned int> resourceHandles;
	vector<T*>* resourceList;
};



#endif
