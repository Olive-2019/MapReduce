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
public:
	//�вι��캯����nMap��nReduce�ǹ�����ָ����
	Cooridinator(int nMap, int nReduce);
	//Cooridinator���й��̣�����������
	void run(string inputFilePath);
	//ע��workers
	bool registerWorker(string ip, int port);
	// ��ȡһ�����е�worker��������ռ�ã���������ڿ��Ի�ȡ��worker���򷵻�-1
	int getIdleWorker(WorkerStateEnum workerState);
	// �������ļ��ֳ�nMap��map�������ļ�
	vector<string> spiltInputFile(string inputFilePath);
};

