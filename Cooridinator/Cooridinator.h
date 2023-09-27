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
	
	// worker������
	int workersNum;
	// worker���ӳ�
	vector<WorkerState> workersList;
	// reduce��������
	int nReduce;
	// map��������
	int nMap;
	// worker�������
	int workerCap;
	// ����workers list�Ļ�����
	mutex workersListLock;




	// ������ȣ����ص����������������ļ�·��
	vector<string> scheduleTask(const vector<string>& inputFile, WorkerStateEnum taskType);
	// ��ʣ�������б���ɾ�������񣬲�������һ������ĵ�����
	set<int>::iterator deleteTask(set<int>& leftTask, set<int>::iterator deletedTask);


	// ��ѯworker������״̬��������������ɵ����·��
	vector<string> poolingWorker(const vector<shared_future<string>>& futureOutputFilesPath, set<int>& tasks);
	// ��ѯWorker������״̬�����������·��
	string checkWorker(const shared_future<string>& futureOutputFilePath, int workerID, set<int>& tasks);
	// ��ȡһ�����е�worker��������ռ�ã���������ڿ��Ի�ȡ��worker���򷵻�-1
	int getIdleWorker(WorkerStateEnum workerState);
	// ����workerΪidle״̬
	void setWorkerIdle(int workerID);
	// ֹͣworker�Ĺ���״̬
	void stopWorker(int workerID);

	// �������ļ��ֳ�nMap��map�������ļ�
	vector<string> spiltInputFile(string inputFilePath);
	// ����reduce taskID���ַ���
	vector<string> getReduceTasksID() const;

public:
	//�вι��캯����nMap��nReduce�ǹ�����ָ����
	Cooridinator(int nReduce);
	//Cooridinator���й��̣�����������
	void run(string inputFilePath);
	// ������Ϣ
	void heartBreak(int workerID);
	// ע��workers
	bool registerWorker(string ip, int workerServerPort, int workerStopControllerPort);
};

