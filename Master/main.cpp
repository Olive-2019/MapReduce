#include "../rest_rpc/include/rest_rpc.hpp"
#include <string>
#include <chrono>
#include <iostream>
#include "Task.h"
#include "MapTask.h"
#include "ReduceTask.h"
#include "WorkerStateEnum.h"
#include "FileCoder.h"
using namespace rest_rpc;
using namespace rest_rpc::rpc_service;
using std::string;
using namespace std;

Task* taskRunner;

void stopTask(rpc_conn conn) {
	if (!taskRunner) return;
	taskRunner->stopTaskAPI();
	taskRunner = NULL;
}

// taskType ���ڱ����������
// inputFilePath ��map����������ָ������·���͵�ǰ�����ţ���reduce�����н�����ָ�������ţ�������ͨ�������ȡ��
// workerID��Ҫ���ڷ��������ź�
// otherTaskNum���������ļ�·��
string runTask(rpc_conn conn, int taskType, string inputFilePath, int workerID, int otherTaskNum) {
	taskRunner = NULL;
	int blockSize = 16;
	if (taskType == (int)WorkerStateEnum::Map) taskRunner = new MapTask(workerID, blockSize);
	else if (taskType == (int)WorkerStateEnum::Reduce) taskRunner = new ReduceTask(workerID, blockSize);
	if (!taskRunner) return "taskType error";
	string outputFilePath = taskRunner->run(inputFilePath, otherTaskNum);
	delete taskRunner;
	taskRunner = NULL;
	return outputFilePath;
}
void runServer(int port) {
	rpc_server server(port, 10);
	server.register_handler("runTask", runTask);
	server.run();//���������
}
void runStopTaskContorller(int port) {
	rpc_server server(port, 10);
	server.register_handler("stopTask", stopTask);
	server.run();//���������
}

int main() {
	int serverPort = 9000;
	int stopControllerPort = 8999;
    try {
		thread stopController(runStopTaskContorller, stopControllerPort);
        runServer(serverPort);
		stopController.join(); 
		/*
		string filePath = "mr_input_1.data";
		ifstream inputFile;
		inputFile.open(filePath, ios::in);
		if (!inputFile.is_open())
			cout << "can't open this file" << endl;*/
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    

	return 0;
}