#pragma once
#include "Task.h"
class ReduceTask :
    public Task
{
public:
    ReduceTask(int workerID) : Task(workerID) {}
    string run(string inputFilePath);
};

