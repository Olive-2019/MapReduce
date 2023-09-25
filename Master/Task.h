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

	// 检查点：是否需要停止进程
	bool stopTaskRunner(string tmpFilePath);
public:
	Task(int workerID);
	virtual string run(string inputFilePath);
	// 发送心跳信号
	void heartBreak(int workerID);
	// 终止任务接口
	void stopTaskAPI();
};

