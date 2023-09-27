#include "WorkerState.h"


WorkerState::WorkerState(string ip, int workerServerPort, int workerStopContorllerPort, int workerID) : ip(ip), workerServerPort(workerServerPort),
workerStopContorllerPort(workerStopContorllerPort), workerState(WorkerStateEnum::Idle), workerID(workerID), start(0), taskID(-1) {}
WorkerStateEnum WorkerState::getState() const {
	return workerState;
}

bool WorkerState::setState(WorkerStateEnum state) {
	// if (workerState != WorkerStateEnum::Idle) return false;
	workerState = state;
	return true;
}

string WorkerState::signTask(WorkerStateEnum task, string inputFilePath, int workerID, int taskID, int otherTaskNum) {
    try {
        if (workerState != task) {
            throw exception("WorkerState::signTask logical error");
        }
        this->taskID = taskID;
        //��¼����ʼʱ��
        start = time(NULL);
        //��������
        rpc_client client(ip, workerServerPort);// IP ��ַ���˿ں�
        //�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true
        bool has_connected = client.connect(5);
        //û�н����������˳�����
        if (!has_connected) {
            throw exception("WorkerState::signTask connect timeout");
        }
        //����Զ�̷��񣬷��ظ�����д���ļ�·��
        string outputFilePath = client.call<string>("runTask", (int)task, inputFilePath, workerID, otherTaskNum);// runTask Ϊ����ע��õķ�����������д����
        cout << "WorkerState::signTask " << outputFilePath << endl;
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
	if (diff > 100) {
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
    if (diff > 30) {
        // ����3s�������worker������
        workerState = WorkerStateEnum::Dead;
        return true;
    }
    return false;
}

void WorkerState::stopTask() {
    workerState = WorkerStateEnum::Idle;
    taskID = -1;
    //����worker��stop������
    try {
        //��������
        rpc_client client(ip, workerStopContorllerPort);// IP ��ַ���˿ں�
        //�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true
        bool has_connected = client.connect(5);
        //û�н����������˳�����
        if (!has_connected) {
            throw exception("WorkerState::stopTask connect timeout");
        }
        //����Զ�̷���
        client.call<void>("stopTask");// stopTask Ϊ����ע��õķ�����������д����
        
    }
    //�������Ӵ��󡢵��÷���ʱ�������Ե�������׳��쳣
    catch (const ::std::exception& e) {
        cout << e.what() << endl;
        throw e;
    }
}
