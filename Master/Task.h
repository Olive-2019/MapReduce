#pragma once
#include <string>
#include "../rest_rpc/include/rest_rpc.hpp"
#include <exception>
#include <windows.h>
#include <thread>
#include <cstdio>

using std::remove;
using std::string;
using std::thread;
using std::exception;
using namespace rest_rpc;

class Task
{
	int workerID;
	bool stopHeartBreak, stopTask;

	// ���㣺�Ƿ���Ҫֹͣ����
	bool stopTaskRunner(string tmpFilePath);
public:
	Task(int workerID);
	virtual string run(string inputFilePath);
	// ���������ź�
	void heartBreak(int workerID);
	// ��ֹ����ӿ�
	void stopTaskAPI();
};

