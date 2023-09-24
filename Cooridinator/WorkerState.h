#pragma once

#include <ctime>
#include <string>
#include <iostream>
#include <exception>
using std::exception;
using std::cout;
using std::endl;
using std::string;
#include "../rest_rpc/include/rest_rpc.hpp"
#include "WorkerStateEnum.h"
using namespace rest_rpc;
using namespace stdext;
class WorkerState
{
	//��־��ǰworker״̬
	WorkerStateEnum workerState;
	// worker ip
	string ip;
	// worker �˿ں�
	int port;
	// ��ǰ����Ŀ�ʼʱ��
	time_t start;
	// �ϴ�����ʱ��
	time_t lastTime;
	// ��ǰworker��id
	int workerID;
public:
	WorkerState(string ip, int port, int workerID);
	//��ȡworker״̬
	WorkerStateEnum getState();
	//����worker��״̬
	bool setState(WorkerStateEnum state);
	//����worker������������ģʽ
	string signTask(WorkerStateEnum task, string inputFilePath, int workerID);
	//�鿴�����Ƿ�ʱ
	bool isTimeOut();
	// ��鵱ǰworker�Ƿ�����
	bool isDead();
	// ����������Ϣ
	void heartBreak();
};

