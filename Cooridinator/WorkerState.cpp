#include "WorkerState.h"


WorkerState::WorkerState(string ip, int port, int workerID) : ip(ip), port(port),
workerState(WorkerStateEnum::Idle), workerID(workerID), start(0){}
WorkerStateEnum WorkerState::getState() {
	return workerState;
}

bool WorkerState::setState(WorkerStateEnum state) {
	if (workerState != WorkerStateEnum::Idle) return false;
	workerState = state;
	return true;
}

string WorkerState::signTask(WorkerStateEnum task, string inputFilePath, int workerID) {
    try {
        //记录任务开始时间
        start = time(NULL);
        //建立连接
        rpc_client client(ip, port);// IP 地址，端口号
        //设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true
        bool has_connected = client.connect(5);
        //没有建立连接则退出程序
        if (!has_connected) {
            throw exception("WorkerState::signTask connect timeout");
        }
        //调用远程服务，返回该任务写出文件路径
        string outputFilePath = client.call<string>("runTask", (int)task, inputFilePath, workerID);// runTask 为事先注册好的服务名，后面写参数
        return outputFilePath;
    }
    //遇到连接错误、调用服务时参数不对等情况会抛出异常
    catch (const ::std::exception& e) {
        cout << e.what() << endl;
        throw e;
    }
}
bool WorkerState::isTimeOut() {
	time_t now = time(NULL);
	double diff = difftime(now, start);
	if (diff > 10) {
        // 超过10s
		workerState = WorkerStateEnum::TimeOut;
		return true;
	}
	return false;
}
void WorkerState::heartBreak() {
    lastTime = time(NULL);
    cout << "WorkerState::heartBreak lastTime:" << lastTime << endl;
}
bool WorkerState::isDead() {
    time_t now = time(NULL);
    double diff = difftime(now, lastTime);
    if (diff > 3) {
        // 超过3s，给这个worker判死刑
        workerState = WorkerStateEnum::Dead;
        return true;
    }
    return false;
}
