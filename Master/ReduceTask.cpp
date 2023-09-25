#include "ReduceTask.h"

string ReduceTask::run(string inputFilePath) {
	Task::run(inputFilePath);
	return "ReduceTask " + inputFilePath;
}