#pragma once
using namespace std;

class Threadpool
{
private:
	static const UINT MaxThreads = 16;

	unsigned int ThreadsCount = 0;

	vector<thread> Wokers;

public:

	Threadpool(unsigned int AllocCount);
	~Threadpool();

};

