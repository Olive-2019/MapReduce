#include "ReduceTask.h"

string ReduceTask::run(string inputFilePath, int nReduce) {
	Task::run(inputFilePath, nReduce);
	return "ReduceTask " + inputFilePath;
}