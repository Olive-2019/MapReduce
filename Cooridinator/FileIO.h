#pragma once
#include <cstdlib>
#include <istream>
#include <fstream>
#include <exception>
#include <string>
#include <map>
#include <vector>

using std::rename;
using std::exception;
using std::ifstream;
using std::ios;
using std::fstream;
using std::to_string;
using std::string;
using std::map;
using std::pair;
using std::vector;
class FileIO
{
	string filePath;
	const int blockSize;
public:
	FileIO(string filePath, int blockSize);
	FileIO():blockSize(0){}
	// 合并不同文件，返回汇总文件的路径
	void mergeFile(const vector<string>& inputFilesPath, const string& outputFilePath);
	// 将输入文件划分成一行行数据
	vector<string> splitInputFile();
};

