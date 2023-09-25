#pragma once
#include <string>
#include <thread>
using std::string;
using std::thread;
#include "../rest_rpc/include/rest_rpc.hpp"
#include <exception>
#include <windows.h>
using std::exception;
using namespace rest_rpc;
class Task
{
	int workerID;
	bool stop;
public:
	Task(int workerID);
	virtual string run(string inputFilePath);
	void heartBreak(int workerID);
};

