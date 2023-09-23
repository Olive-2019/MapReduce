#pragma once
#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <exception>
#include "WorkerState.h"
#include "WorkerStateEnum.h"
#include "../rest_rpc/include/rest_rpc.hpp"

using std::pair;
using std::map;
using std::vector;
using std::string;
using std::mutex;
using std::exception;

class Cooridinator {
	
	// worker的数量
	int workersNum;
	// worker连接池
	vector<WorkerState> workersList;
	// reduce任务数量
	int nReduce;
	// map任务数量
	int nMap;
	// worker最大数量
	int workerCap;
	// 访问workers list的互斥锁
	mutex workersListLock;
public:
	//有参构造函数，nMap和nReduce是工厂类指定的
	Cooridinator(int nMap, int nReduce);
	//Cooridinator运行过程，如分配任务等
	void run(string inputFilePath);
	//注册workers
	bool registerWorker(string ip, int port);
	// 获取一个空闲的worker，并将其占用，如果不存在可以获取的worker，则返回-1
	int getIdleWorker(WorkerStateEnum workerState);
	// 将输入文件分成nMap个map的输入文件
	vector<string> spiltInputFile(string inputFilePath);
};

