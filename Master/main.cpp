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

string runTask(rpc_conn conn, int taskType, string inputFilePath, int workerID, int nReduce) {
	taskRunner = NULL;
	int blockSize = 16;
	if (taskType == (int)WorkerStateEnum::Map) taskRunner = new MapTask(workerID, blockSize);
	else if (taskType == (int)WorkerStateEnum::Reduce) taskRunner = new ReduceTask(workerID, blockSize);
	if (!taskRunner) return "taskType error";
	string outputFilePath = taskRunner->run(inputFilePath, nReduce);
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
		stopController.join();/*
		map<string, string> inputData;
		FileCoder fileCoder("../Files/mr_input_1.data", 300);
		while (!fileCoder.isInputEOF()) {
			inputData = fileCoder.decodeMapperInputData();
		}*/
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    

	return 0;
}