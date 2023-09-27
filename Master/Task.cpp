#include "Task.h"
#include <iostream>
using std::cout;
using std::endl;
Task::Task(int workerID, int blockSize) : workerID (workerID), stopTask(false), 
               stopHeartBreak(false), blockSize(blockSize) {}
bool Task::stopTaskRunner(const vector<string>& tmpFilesPath) {
    if (stopTask)
        // 删除中间文件
        removeTmpFiles(tmpFilesPath);
    return stopTask;
}
void Task::removeTmpFiles(const vector<string>& tmpFilesPath) {
    for (int i = 0; i < tmpFilesPath.size(); ++i)
        if (remove(tmpFilesPath[i].c_str())) throw exception("Task::stopTaskRunner delete tmp file failed");
}
string Task::run(string inputFilePath, int otherTaskNum) {
    taskID = 0;
    // 提取taskID
    for (int i = 0; i < inputFilePath.size(); ++i) {
        if (inputFilePath[i] <= '9' && inputFilePath[i] >= '0') taskID *= 10, taskID += (inputFilePath[i] - '0');
        else if (taskID) break;
    }
    this->otherTaskNum = otherTaskNum;
    stopHeartBreak = false;
    heartBreakThread = new thread(&Task::heartBreak, this, workerID);
    
    return "";
}
void Task::afterRun() {
    stopHeartBreak = true;
    try {
        heartBreakThread->join();
        delete heartBreakThread;
        heartBreakThread = NULL;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}
void Task::heartBreak(int workerID) {
    string ip = "127.0.0.1";
    int port = 8998;
    rpc_client client(ip, port);
    //设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true
    bool has_connected = client.connect(5);
    //没有建立连接则退出程序
    if (!has_connected) throw exception("Task::heartBreak connect timeout");
    
    //调用远程服务，返回心跳信息
    while (!stopHeartBreak) {
        Sleep(1000);
        client.call<>("heartBreak", workerID);
    }
}
void Task::stopTaskAPI() {
    stopTask = true;
}