#pragma once
#include "Task.h"
class ReduceTask :
    public Task
{
public:
    ReduceTask(int workerID, int blockSize) : Task(workerID, blockSize) {}
    string run(string inputFilePath, int nReduce);
};

