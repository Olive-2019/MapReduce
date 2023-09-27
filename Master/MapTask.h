#pragma once
#include "Task.h"
#include <sstream>

using std::stringstream;

class MapTask : public Task {
    // ����keyֵ��partitionֵ
    int partitionValue(string key);
    // �ָ��ַ���
    vector<string> stringSplit(const string& str, char delim) const;
    // ������ֵ���ͬ��partition region��
    vector<vector<pss>> shuffle(vector<pss> curOutputData);
public:
    MapTask(int workerID, int blockSize) : Task(workerID, blockSize){}
    // ִ��map����
    string run(string inputFilePath, int otherTaskNum);
    // �û������Map����
    vector<pss> userDefinedTask(const vector<pss>& inputKV);

};

