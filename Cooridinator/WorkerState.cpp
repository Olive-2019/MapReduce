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

string WorkerState::signTask(WorkerStateEnum task, string inputFilePath) {
    try {
        //��¼����ʼʱ��
        start = time(NULL);
        /*��������*/
        rpc_client client(ip, port);// IP ��ַ���˿ں�
        /*�趨��ʱ 5s������Ĭ��Ϊ 3s����connect ��ʱ���� false���ɹ����� true*/
        bool has_connected = client.connect(5);
        /*û�н����������˳�����*/
        if (!has_connected) {
            throw exception("WorkerState::signTask connect timeout");
        }
        /*����Զ�̷��񣬷��ظ�����д���ļ�·��*/
        string outputFilePath = client.call<string>("runTask", (int)task, inputFilePath);// runTask Ϊ����ע��õķ�����������д����
        //cout << outputFilePath << endl;
        return outputFilePath;
    }
    /*�������Ӵ��󡢵��÷���ʱ�������Ե�������׳��쳣*/
    catch (const ::std::exception& e) {
        throw e;
    }
}
bool WorkerState::isTimeOut() {
	time_t now = time(NULL);
	double diff = difftime(now, start);
	if (diff > 10) {
        // ����10s�������worker������
		workerState = WorkerStateEnum::Dead;
		return true;
	}
	return false;
}