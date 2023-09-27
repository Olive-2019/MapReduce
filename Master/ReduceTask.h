#pragma once
#include "Task.h"
#include <queue>

using std::priority_queue;

class ReduceTask :
    public Task
{
    // ��ȡ�����ļ�·��
    vector<string> getInputFilesPath();
    // �������ļ����������򣬲��ŵ�һ���ļ�����������ļ���·��
    string shuffle();
    // ������������ļ����й鲢���򣬲������һ���ļ���
    string mergeSortShuffle(vector<string> inputFilesPath);
    // ����������ʱ�ļ��������û���������
    vector<pss> readTmpFileAndRunReducer(string tmpFile);
public:
    ReduceTask(int workerID, int blockSize) : Task(workerID, blockSize) {}
    // ����ִ��
    string run(string inputFilePath, int otherTaskNum);
    // �û����������
    pss userDefinedTask(vector<pss> inputKV);
};

