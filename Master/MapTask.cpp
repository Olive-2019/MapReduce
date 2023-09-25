#include "MapTask.h"
string MapTask::run(string inputFilePath) {
	Task::run(inputFilePath);
	return "MapTask " + inputFilePath;
}