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

typedef pair<string, string> pss;

class FileCoder
{
	fstream inputFile;
	const string filePath;
	const int blockSize;
	bool inputEOF;
public:
	FileCoder(string filePath, int blockSize);
	// 分步读出
	vector<pss> decodeMapperInputData();
	// 输入文件是否读取完毕
	bool isInputEOF() const;
	// 写出mapper输出的临时文件，即缓冲区刷出
	void writeMapperOutputTmpFile(const string& outputFilePath, const vector<pss>& outputData);
	// 将mapper输出的临时文件转换为正式的data文件
	void writeMapperOutputFile(const string& outputFilePath);
};

