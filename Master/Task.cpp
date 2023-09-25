#include "Task.h"
#include <iostream>
using std::cout;
using std::endl;
Task::Task(int workerID) : workerID (workerID) {}
string Task::run(string inputFilePath) {
    stop = false;
    thread thread(&Task::heartBreak,this, workerID);
    Sleep(500);//假装在执行任务
    stop = true;
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
    //设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true
    bool has_connected = client.connect(5);
    //没有建立连接则退出程序
    if (!has_connected) throw exception("Task::heartBreak connect timeout");
    
    //调用远程服务，返回该任务写出文件路径
    while (!stop) {
        Sleep(100);
        string outputFilePath = client.call<string>("heartBreak", workerID);
    }
    

}
