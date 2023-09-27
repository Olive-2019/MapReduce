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
	// �ϲ���ͬ�ļ������ػ����ļ���·��
	void mergeFile(const vector<string>& inputFilesPath, const string& outputFilePath);
	// �������ļ����ֳ�һ��������
	vector<string> splitInputFile();
};

