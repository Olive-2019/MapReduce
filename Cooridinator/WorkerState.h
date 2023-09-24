#pragma once

#include <ctime>
#include <string>
#include <iostream>
#include <exception>
using std::exception;
using std::cout;
using std::endl;
using std::string;
#include "../rest_rpc/include/rest_rpc.hpp"
#include "WorkerStateEnum.h"
using namespace rest_rpc;
using namespace stdext;
class WorkerState
{
	//标志当前worker状态
	WorkerStateEnum workerState;
	// worker ip
	string ip;
	// worker 端口号
	int port;
	// 当前任务的开始时间
	time_t start;
	// 上次心跳时间
	time_t lastTime;
	// 当前worker的id
	int workerID;
public:
	WorkerState(string ip, int port, int workerID);
	//获取worker状态
	WorkerStateEnum getState();
	//设置worker的状态
	bool setState(WorkerStateEnum state);
	//分配worker任务，阻塞函数模式
	string signTask(WorkerStateEnum task, string inputFilePath, int workerID);
	//查看任务是否超时
	bool isTimeOut();
	// 检查当前worker是否死了
	bool isDead();
	// 设置心跳信息
	void heartBreak();
};

