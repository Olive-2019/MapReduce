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
string runTask(rpc_conn conn, int taskType, string inputFilePath) {
	Task* taskRunner = NULL;
	if (taskType == (int)WorkerStateEnum::Map) taskRunner = new MapTask();
	else if (taskType == (int)WorkerStateEnum::Reduce) taskRunner = new ReduceTask();
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
	//runServer();
    try {
        runServer(port);
        //registerWorker("127.0.0.1", 9000);
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    

	return 0;
}