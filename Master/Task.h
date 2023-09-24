#pragma once
#include <string>
using std::string;

class Task
{
	int workerID;
public:
	Task(int workerID);
	virtual string run(string inputFilePath) {
		return inputFilePath;
	}
	void heartBreak();
};

