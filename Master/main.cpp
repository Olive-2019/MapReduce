#include "../rest_rpc/include/rest_rpc.hpp"
#include <string>
#include <chrono>
#include <iostream>
#include "Task.h"
#include "MapTask.h"
#include "ReduceTask.h"
#include "WorkerStateEnum.h"
using namespace rest_rpc;
using namespace rest_rpc::rpc_service;
using std::string;
using namespace std;

void hello(rpc_conn conn, string name) {
	cout << "hello " << name << endl;
	/*����Ϊ void �������ͣ�������ú󲻸�Զ�̿ͻ��˷�����Ϣ*/
	//return ("Hello " + name); /*���ظ�Զ�̿ͻ��˵�����*/
}
string runTask(rpc_conn conn, int taskType, string inputFilePath, int workerID) {
	Task* taskRunner = NULL;
	if (taskType == (int)WorkerStateEnum::Map) taskRunner = new MapTask(workerID);
	else if (taskType == (int)WorkerStateEnum::Reduce) taskRunner = new ReduceTask(workerID);
	if (!taskRunner) return "error";
	return taskRunner->run(inputFilePath);
}
void runServer(int port) {
	rpc_server server(port, 1);
	server.register_handler("runTask", runTask);
	server.run();//���������
}
int main() {
	int port = 9000;
    try {
        runServer(port);
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    

	return 0;
}