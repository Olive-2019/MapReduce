#include "FileCoder.h"

FileCoder::FileCoder(string filePath, int blockSize) : filePath(filePath), blockSize(blockSize), inputEOF(false) {}

vector<pss> FileCoder::decodeMapperInputData() {
	vector<pss> inputData;
	if (!inputFile.is_open()) inputFile.open(filePath, ios::in);
	if (!inputFile.is_open())
		throw exception("FileCoder::decodeMapperInputData didn't open input file");
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

bool FileCoder::isInputEOF() const {
	return inputEOF;
}

void FileCoder::writeMapperOutputTmpFile(const string& outputFilePath, const vector<pss>& outputData) {
	fstream outfile;
	outfile.open(outputFilePath, ios::out | ios::app);
	if (!outfile.is_open()) throw exception("FileCoder::writeMapperOutputTmpFile open output file failed");
	for (auto it = outputData.begin(); it != outputData.end(); ++it) {
		string buf = it->first + " " + it->second + "\n";
		outfile.write(buf.c_str(), buf.size());
	}
	outfile.close();
}
void FileCoder::writeMapperOutputFile(const string& outputFilePath) {
	string outputFilePathWithoutTmp = outputFilePath.substr(0, outputFilePath.find_last_of('.'));
	string outputFilePathWithData = outputFilePathWithoutTmp + ".data";
	if (rename(outputFilePath.c_str(), outputFilePathWithData.c_str()))
		throw exception("FileCoder::writeMapperOutputFile rename file failed");
}