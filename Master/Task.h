#pragma once
#include <string>
#include "../rest_rpc/include/rest_rpc.hpp"
#include "FileCoder.h"
#include <exception>
#include <windows.h>
#include <thread>
#include <cstdio>
#include <vector>
#include <map>
#include <algorithm>

using std::map;
using std::remove;
using std::string;
using std::thread;
using std::exception;
using std::vector;
using std::pair;
using namespace rest_rpc;



class Task
{
protected:
	
	int workerID, otherTaskNum, taskID, blockSize;
	bool stopHeartBreak, stopTask;
	// �����kv��ֵ��
	map<string, string> inputKV;
	// �����kv��ֵ��
	map<string, string> outputKV;
	// �����߳�ָ��
	thread* heartBreakThread;
	// ���㣺�Ƿ���Ҫֹͣ����
	bool stopTaskRunner(const vector<string>& tmpFilesPath);
	// ɾ����ʱ�ļ�
	void removeTmpFiles(const vector<string>& tmpFilesPath);
	
public:
	Task(int workerID, int blockSize);
	// �������� map�������ǵ��ļ�����reduce�������Ƕ��ļ��������ļ����м���'-'���
	// ����run��һЩ��ʼ���Ĳ�����Ҳfork�������߳�
	virtual string run(string inputFilePath, int otherTaskNum);
	// ���н����Ĳ���������ֹͣ�������̣����ո��̵߳�
	void afterRun();
	// ���ݷ��࣬map ���к���� reduce ����ǰ����
	// virtual void shuffle(){}
	
	// ���������ź�
	void heartBreak(int workerID);
	// ��ֹ����ӿ�
	void stopTaskAPI();
};

