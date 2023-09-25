#include "WorkerState.h"


WorkerState::WorkerState(string ip, int port, int workerID) : ip(ip), port(port),
workerState(WorkerStateEnum::Idle), workerID(workerID), start(0){}
WorkerStateEnum WorkerState::getState() const {
	return workerState;
}

bool WorkerState::setState(WorkerStateEnum state) {
	if (workerState != WorkerStateEnum::Idle) return false;
	workerState = state;
	return true;
}

string WorkerState::signTask(WorkerStateEnum task, string inputFilePath, int workerID, int taskID) {
    try {
        if (workerState != task) throw exception("WorkerState::signTask logical error");
        this->taskID = taskID;
        //��¼����ʼʱ��
        start = time(NULL);
        //��������
        rpc_client client(ip, port);// IP ��ַ���˿ں�
        //�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true
        bool has_connected = client.connect(5);
        //û�н����������˳�����
        if (!has_connected) {
            throw exception("WorkerState::signTask connect timeout");
        }
        //����Զ�̷��񣬷��ظ�����д���ļ�·��
        string outputFilePath = client.call<string>("runTask", (int)task, inputFilePath, workerID);// runTask Ϊ����ע��õķ�����������д����
        return outputFilePath;
    }
    //�������Ӵ��󡢵��÷���ʱ�������Ե�������׳��쳣
    catch (const ::std::exception& e) {
        cout << e.what() << endl;
        throw e;
    }
}
int WorkerState::getTaskID() const {
    return taskID;
}
bool WorkerState::isTimeOut() {
	time_t now = time(NULL);
	double diff = difftime(now, start);
	if (diff > 10) {
        // ����10s
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
        // ����3s�������worker������
        workerState = WorkerStateEnum::Dead;
        return true;
    }
    return false;
}
int WorkerState::getTaskID() {
    return taskID;
}
