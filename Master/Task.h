#pragma once
#include <string>
using std::string;

class Task
{
public:
	virtual string run(string inputFilePath) {
		return inputFilePath;
	}
};

