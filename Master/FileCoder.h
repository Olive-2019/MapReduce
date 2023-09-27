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
typedef pair<pss, int> pssi;
struct cmp {
	bool operator() (const pssi& a, const pssi& b) {
		if (a.first.first == b.first.first) return false;
		return a.first.first > b.first.first;
	}
};
class FileCoder
{
	fstream inputFile;
	const string filePath;
	const int blockSize;
	bool inputEOF;
	pss nextOneRow;
public:
	FileCoder(string filePath, int blockSize);

	// 分步读出数据
	vector<pss> decodeInputData();
	// 读出一行
	pss decodeOneInputData();
	// 读取有序文件的某个键，最后一次读取的时候有一定概率读到两个键的数据，后一个键的数据在最后，且仅有一个
	vector<pss> decodeOneKeyInputData();

	// 输入文件是否读取完毕
	bool isInputEOF() const;

	// 写出文件
	void writeOutputFile(const vector<pss>& outputData);
	// 将mapper输出的临时文件转换为正式的data文件，更名
	void writeMapperOutputFile(const string& outputFilePath);

	// 单条记录输出
	void writeOneRow(pss outputData);
};

