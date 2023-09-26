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
	// �ֲ�����
	vector<pss> decodeMapperInputData();
	// �����ļ��Ƿ��ȡ���
	bool isInputEOF() const;
	// д��mapper�������ʱ�ļ�����������ˢ��
	void writeMapperOutputTmpFile(const string& outputFilePath, const vector<pss>& outputData);
	// ��mapper�������ʱ�ļ�ת��Ϊ��ʽ��data�ļ�
	void writeMapperOutputFile(const string& outputFilePath);
};

