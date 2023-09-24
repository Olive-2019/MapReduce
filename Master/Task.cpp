#include "Task.h"
#include "../rest_rpc/include/rest_rpc.hpp"
#include <exception>
using std::exception;
using namespace rest_rpc;
Task::Task(int workerID) : workerID (workerID) {}

void Task::heartBreak() {
    string ip = "127.0.0.1";
    int port = 8998;
    rpc_client client(ip, port);
    //设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true
    bool has_connected = client.connect(5);
    //没有建立连接则退出程序
    if (!has_connected) {
        throw exception("Task::heartBreak connect timeout");
    }
    //调用远程服务，返回该任务写出文件路径
    string outputFilePath = client.call<string>("heartBreak", workerID);

}
