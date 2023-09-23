#pragma once
#include "Task.h"
class ReduceTask :
    public Task
{
public:
    string run(string inputFilePath);
};

