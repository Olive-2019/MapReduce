#pragma once
#include "Task.h"
class MapTask :
    public Task
{
public:
    MapTask(int workerID) : Task(workerID){}
    string run(string inputFilePath);
};

