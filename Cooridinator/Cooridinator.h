#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include <mutex>
#include <exception>
#include <thread>
#include "WorkerState.h"
#include "WorkerStateEnum.h"
#include "FileSpliter.h"
#include "../rest_rpc/include/rest_rpc.hpp"

using std::set;
using std::map;
using std::pair;
using std::vector;
using std::string;
using std::to_string;
using std::exception;

using std::thread;
using std::mutex;
//using std::future;
using std::shared_future;
using std::async;
using std::future_status;
using std::chrono::seconds;

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




	// 任务调度，返回的是所有任务的输出文件路径
	vector<string> scheduleTask(const vector<string>& inputFile, WorkerStateEnum taskType);
	// 从剩余任务列表中删除该任务，并返回下一个任务的迭代器
	set<int>::iterator deleteTask(set<int>& leftTask, set<int>::iterator deletedTask);


	// 轮询worker，更新状态，并返回任务完成的输出路径
	vector<string> poolingWorker(const vector<shared_future<string>>& futureOutputFilesPath, set<int>& tasks);
	// 查询Worker，更新状态，并返回输出路径
	string checkWorker(const shared_future<string>& futureOutputFilePath, int workerID, set<int>& tasks);
	// 获取一个空闲的worker，并将其占用，如果不存在可以获取的worker，则返回-1
	int getIdleWorker(WorkerStateEnum workerState);
	// 设置worker为idle状态
	void setWorkerIdle(int workerID);
	// 停止worker的工作状态
	void stopWorker(int workerID);

	// 将输入文件分成nMap个map的输入文件
	vector<string> spiltInputFile(string inputFilePath);
	// 返回reduce taskID的字符串
	vector<string> getReduceTasksID() const;

public:
	//有参构造函数，nMap和nReduce是工厂类指定的
	Cooridinator(int nReduce);
	//Cooridinator运行过程，如分配任务等
	void run(string inputFilePath);
	// 心跳信息
	void heartBreak(int workerID);
	// 注册workers
	bool registerWorker(string ip, int workerServerPort, int workerStopControllerPort);
};

