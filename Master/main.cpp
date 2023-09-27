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

// taskType 用于标记任务类型
// inputFilePath 在map任务中用于指定输入路径和当前任务编号，在reduce任务中仅用于指定任务编号（任务编号通过解码获取）
// workerID主要用于发送心跳信号
// otherTaskNum用于生成文件路径
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
	server.run();//启动服务端
}
void runStopTaskContorller(int port) {
	rpc_server server(port, 10);
	server.register_handler("stopTask", stopTask);
	server.run();//启动服务端
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