#include "Task.h"
#include <iostream>
using std::cout;
using std::endl;
Task::Task(int workerID, int blockSize) : workerID (workerID), stopTask(false), 
               stopHeartBreak(false), blockSize(blockSize) {}
bool Task::stopTaskRunner(const vector<string>& tmpFilesPath) {
    if (stopTask)
        // ɾ���м��ļ�
        removeTmpFiles(tmpFilesPath);
    return stopTask;
}
void Task::removeTmpFiles(const vector<string>& tmpFilesPath) {
    for (int i = 0; i < tmpFilesPath.size(); ++i)
        if (remove(tmpFilesPath[i].c_str())) throw exception("Task::stopTaskRunner delete tmp file failed");
}
string Task::run(string inputFilePath, int otherTaskNum) {
    taskID = 0;
    // ��ȡtaskID
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
    //�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true
    bool has_connected = client.connect(5);
    //û�н����������˳�����
    if (!has_connected) throw exception("Task::heartBreak connect timeout");
    
    //����Զ�̷��񣬷���������Ϣ
    while (!stopHeartBreak) {
        Sleep(1000);
        client.call<>("heartBreak", workerID);
    }
}
void Task::stopTaskAPI() {
    stopTask = true;
}