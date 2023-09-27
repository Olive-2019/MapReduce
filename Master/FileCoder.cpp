#include "FileCoder.h"

FileCoder::FileCoder(string filePath, int blockSize) : filePath(filePath), blockSize(blockSize), inputEOF(false) {}

vector<pss> FileCoder::decodeInputData() {
	vector<pss> inputData;
	if (!inputFile.is_open()) inputFile.open(filePath, ios::in);
	if (!inputFile.is_open())
		throw exception("FileCoder::decodeInputData didn't open input file");
	string buf;
	int size = 0;
	while (getline(inputFile, buf)) {
		size += buf.size();
		inputData.push_back(pss(buf.substr(0, buf.find_first_of(' ')), buf.substr(buf.find_first_of(' ') + 1)));
		
		if (size > blockSize) break;
	}
	if (inputFile.eof()) {
		inputFile.close();
		inputEOF = true;
	}
	else inputEOF = false;
	return inputData;
}

pss FileCoder::decodeOneInputData() {
	if (!inputFile.is_open()) inputFile.open(filePath, ios::in);
	if (!inputFile.is_open())
		throw exception("FileCoder::decodeOneInputData didn't open input file");
	string buf;
	int size = 0;
	getline(inputFile, buf);
	pss inputData = pss(buf.substr(0, buf.find_first_of(' ')), buf.substr(buf.find_first_of(' ') + 1));
	if (inputFile.eof()) {
		inputFile.close();
		inputEOF = true;
	}
	else inputEOF = false;
	return inputData;
}
vector<pss> FileCoder::decodeOneKeyInputData() {
	vector<pss> oneKeyData;
	if (nextOneRow.first.size()) oneKeyData.push_back(nextOneRow);
	else {
		if (!isInputEOF())
			oneKeyData.push_back(decodeOneInputData());
	}
	string key = oneKeyData.back().first;
	pss curData = oneKeyData.back();
	while (curData.first == key && !isInputEOF()) {
		curData = decodeOneInputData();
		if (curData.first == key) oneKeyData.push_back(curData);
	}
	if (isInputEOF()) {
		if (curData.first != key) oneKeyData.push_back(curData);//虽然不应该加入，但是没办法，最后一次得手动检查一下是否需要分离
	}
	nextOneRow = curData;
	return oneKeyData;
}
bool FileCoder::isInputEOF() const {
	return inputEOF;
}

void FileCoder::writeOutputFile(const vector<pss>& outputData) {
	fstream outfile;
	outfile.open(filePath, ios::out | ios::app);
	if (!outfile.is_open()) throw exception("FileCoder::writeOutputFile open output file failed");
	for (auto it = outputData.begin(); it != outputData.end(); ++it) {
		string buf = it->first + " " + it->second + "\n";
		outfile.write(buf.c_str(), buf.size());
	}
	outfile.close();
}

void FileCoder::writeOneRow(pss outputData) {
	vector<pss> outputDataVector{ outputData };
	writeOutputFile(outputDataVector);
}
void FileCoder::writeMapperOutputFile(const string& outputFilePath) {
	string outputFilePathWithoutTmp = outputFilePath.substr(0, outputFilePath.find_last_of('.'));
	string outputFilePathWithData = outputFilePathWithoutTmp + ".data";
	if (rename(outputFilePath.c_str(), outputFilePathWithData.c_str()))
		throw exception("FileCoder::writeMapperOutputFile rename file failed");
}
