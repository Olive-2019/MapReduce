#include "FileSpliter.h"
#include <istream>
#include <fstream>
#include <exception>
#include <string>
using std::exception;
using std::ifstream;
using std::ios;
using std::fstream;
using std::to_string;

FileSpliter::FileSpliter(string inputFilePath, int blockSize) : inputFilePath(inputFilePath), 
blockSize(blockSize) {}
vector<string> FileSpliter::split() {
	vector<string> outputFilePaths;
	ifstream infile(inputFilePath, ios::in);
	if (!infile.is_open()) 
		throw exception("FileSpliter::FileSpliter didn't open input file");
	string buf, curOutputFilePath;
	int blockLeft = -1, outputFileNo = 0, lineNo = 0;
	fstream outfile;
	while (getline(infile, buf)) {
		if (blockLeft < 0) {
			outputFileNo++;
			outfile.close();
			curOutputFilePath = "../Files/mr_input_" + to_string(outputFileNo) + ".data";
			blockLeft = blockSize;
			outfile.open(curOutputFilePath, ios::out | ios::app);
			if (!outfile.is_open())
				throw exception("FileSpliter::FileSpliter didn't open output file " + outputFileNo);
			outputFilePaths.push_back(curOutputFilePath);
		}
		buf = to_string(lineNo) + " " + buf  + "\n";
		int buffSize = buf.size();
		blockLeft -= buffSize;
		outfile.write(buf.c_str(), buf.size());
		lineNo++;
	}
	if (blockLeft > 0 && blockLeft < blockSize) outfile.close();
	return outputFilePaths;
}