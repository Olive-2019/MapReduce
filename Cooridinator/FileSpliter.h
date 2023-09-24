#pragma once
#include <string>
#include <vector>
using std::string;
using std::vector;

class FileSpliter
{
	string inputFilePath;
	int blockSize;
public:
	FileSpliter(string inputFilePath, int blockSize);
	vector<string> split();
	
};

