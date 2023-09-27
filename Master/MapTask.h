#pragma once
#include "Task.h"
#include <sstream>

using std::stringstream;

class MapTask : public Task {
    // 返回key值的partition值
    int partitionValue(string key);
    // 分割字符串
    vector<string> stringSplit(const string& str, char delim) const;
    // 将输出分到不同的partition region里
    vector<vector<pss>> shuffle(vector<pss> curOutputData);
public:
    MapTask(int workerID, int blockSize) : Task(workerID, blockSize){}
    // 执行map任务
    string run(string inputFilePath, int otherTaskNum);
    // 用户定义的Map操作
    vector<pss> userDefinedTask(const vector<pss>& inputKV);

};

