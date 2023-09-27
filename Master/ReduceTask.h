#pragma once
#include "Task.h"
#include <queue>

using std::priority_queue;

class ReduceTask :
    public Task
{
    // 获取输入文件路径
    vector<string> getInputFilesPath();
    // 对输入文件进行外排序，并放到一个文件里，返回有序文件的路径
    string shuffle();
    // 对于已有序的文件进行归并排序，并输出到一个文件中
    string mergeSortShuffle(vector<string> inputFilesPath);
    // 读入有序临时文件并运行用户定义任务
    vector<pss> readTmpFileAndRunReducer(string tmpFile);
public:
    ReduceTask(int workerID, int blockSize) : Task(workerID, blockSize) {}
    // 任务执行
    string run(string inputFilePath, int otherTaskNum);
    // 用户定义的任务
    pss userDefinedTask(vector<pss> inputKV);
};

