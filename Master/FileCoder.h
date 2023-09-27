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

	// �ֲ���������
	vector<pss> decodeInputData();
	// ����һ��
	pss decodeOneInputData();
	// ��ȡ�����ļ���ĳ���������һ�ζ�ȡ��ʱ����һ�����ʶ��������������ݣ���һ����������������ҽ���һ��
	vector<pss> decodeOneKeyInputData();

	// �����ļ��Ƿ��ȡ���
	bool isInputEOF() const;

	// д���ļ�
	void writeOutputFile(const vector<pss>& outputData);
	// ��mapper�������ʱ�ļ�ת��Ϊ��ʽ��data�ļ�������
	void writeMapperOutputFile(const string& outputFilePath);

	// ������¼���
	void writeOneRow(pss outputData);
};

