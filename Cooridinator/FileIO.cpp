#include "FileIO.h"
FileIO::FileIO(string filePath, int blockSize) :
	filePath(filePath), blockSize(blockSize) {}


void FileIO::mergeFile(const vector<string>& inputFilesPath, const string& outputFilePath) {
	fstream outfile;
	outfile.open(outputFilePath, ios::out | ios::app);
	if (!outfile.is_open()) throw exception("FileIO::mergeFile open output file failed");
	string buf;
	for (auto inputFilePath : inputFilesPath) {
		fstream inputFile(inputFilePath, ios::in);
		while (getline(inputFile, buf)) {
			buf += '\n';
			outfile.write(buf.c_str(), buf.size());
		}
		inputFile.close();
	}
	outfile.close();
}
vector<string> FileIO::splitInputFile() {
	vector<string> outputFilePaths;
	ifstream infile(filePath, ios::in);
	if (!infile.is_open())
		throw exception("FileIO::splitInputFile didn't open input file");
	string buf, curOutputFilePath;
	int blockLeft = -1, outputFileNo = 0, lineNo = 0;
	fstream outfile;
	while (getline(infile, buf)) {
		if (blockLeft < 0) {
			outputFileNo++;
			outfile.close();
			curOutputFilePath = "../Files/mr_input_" + to_string(outputFileNo) + ".data";
			blockLeft = blockSize;
			outfile.open(curOutputFilePath, ios::out);
			if (!outfile.is_open())
				throw exception("FileIO::splitInputFile didn't open output file " + outputFileNo);
			outputFilePaths.push_back(curOutputFilePath);
		}
		buf = to_string(lineNo) + " " + buf + "\n";
		int buffSize = buf.size();
		blockLeft -= buffSize;
		outfile.write(buf.c_str(), buf.size());
		lineNo++;
	}
	if (blockLeft > 0 && blockLeft < blockSize) outfile.close();
	return outputFilePaths;
}