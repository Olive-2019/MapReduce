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
    //�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true
    bool has_connected = client.connect(5);
    //û�н����������˳�����
    if (!has_connected) {
        throw exception("Task::heartBreak connect timeout");
    }
    //����Զ�̷��񣬷��ظ�����д���ļ�·��
    string outputFilePath = client.call<string>("heartBreak", workerID);

}
