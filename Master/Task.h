#pragma once
#include <string>
#include "../rest_rpc/include/rest_rpc.hpp"
#include "FileCoder.h"
#include <exception>
#include <windows.h>
#include <thread>
#include <cstdio>
#include <vector>
#include <map>
#include <algorithm>

using std::map;
using std::remove;
using std::string;
using std::thread;
using std::exception;
using std::vector;
using std::pair;
using namespace rest_rpc;



class Task
{
protected:
	
	int workerID, otherTaskNum, taskID, blockSize;
	bool stopHeartBreak, stopTask;
	// 读入的kv键值对
	map<string, string> inputKV;
	// 输出的kv键值对
	map<string, string> outputKV;
	// 心跳线程指针
	thread* heartBreakThread;
	// 检查点：是否需要停止进程
	bool stopTaskRunner(const vector<string>& tmpFilesPath);
	// 删除临时文件
	void removeTmpFiles(const vector<string>& tmpFilesPath);
	
public:
	Task(int workerID, int blockSize);
	// 运行任务 map的输入是单文件名，reduce的输入是多文件名，多文件名中间以'-'间隔
	// 父类run做一些初始化的操作，也fork出心跳线程
	virtual string run(string inputFilePath, int otherTaskNum);
	// 运行结束的操作，包括停止心跳进程，回收该线程等
	void afterRun();
	// 数据分类，map 运行后调用 reduce 运行前调用
	// virtual void shuffle(){}
	
	// 发送心跳信号
	void heartBreak(int workerID);
	// 终止任务接口
	void stopTaskAPI();
};

