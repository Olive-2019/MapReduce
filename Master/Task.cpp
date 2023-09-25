#include "Task.h"
#include <iostream>
using std::cout;
using std::endl;
Task::Task(int workerID) : workerID (workerID), stopTask(false), stopHeartBreak(false) {}
bool Task::stopTaskRunner(string tmpFilePath) {
    if (stopTask) 
        // ɾ���м��ļ�
        if (remove(tmpFilePath.c_str())) throw exception("Task::stopTaskRunner delete tmp file failed");
    return stopTask;
}
string Task::run(string inputFilePath) {
    stopHeartBreak = false;
    thread thread(&Task::heartBreak,this, workerID);
    Sleep(500);//��װ��ִ������
    stopHeartBreak = true;
    try {
        thread.join();
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    cout << "Task::run end" << endl;
    return "Task: " + inputFilePath;
}
void Task::heartBreak(int workerID) {
    string ip = "127.0.0.1";
    int port = 8998;
    rpc_client client(ip, port);
    //�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true
    bool has_connected = client.connect(5);
    //û�н����������˳�����
    if (!has_connected) throw exception("Task::heartBreak connect timeout");
    
    //����Զ�̷��񣬷��ظ�����д���ļ�·��
    while (!stopHeartBreak) {
        Sleep(100);
        string outputFilePath = client.call<string>("heartBreak", workerID);
    }
}
void Task::stopTaskAPI() {
    stopTask = true;
}