#include "ReduceTask.h"

string ReduceTask::run(string inputFilePath, int otherTaskNum) {
	Task::run(inputFilePath, otherTaskNum);
	// reduce 任务输出文件路径
	string outputFilePath = "../Files/mr_output_" + to_string(taskID) + ".data";
	// 数据文件排序，并生成新临时文件
	string shuffledInputDataPath = shuffle();
	// 读入有序文件并运行reduce任务
	vector<pss> reduceResult = readTmpFileAndRunReducer(shuffledInputDataPath);
	// 删除有序中间文件
	removeTmpFiles(vector<string>{shuffledInputDataPath});
	// 写出结果文件
	FileCoder fileCoder(outputFilePath, blockSize);
	fileCoder.writeOutputFile(reduceResult);
	Task::afterRun();
	return outputFilePath;
}
pss ReduceTask::userDefinedTask(vector<pss> inputKV) {
	string key = inputKV[0].first;
	int cnt = inputKV.size();
	return pss(key, to_string(cnt));
}
vector<string> ReduceTask::getInputFilesPath() {
	vector<string> inputFilesPath;
	for (int i = 1; i <= otherTaskNum; ++i) {
		string inputFilePath = "../Files/mr_" + to_string(i) + '_' + to_string(taskID) + ".data";
		inputFilesPath.push_back(inputFilePath);
	}
	return inputFilesPath;
}
string ReduceTask::shuffle() {
	vector<string> inputFilesPath = getInputFilesPath();
	
	// 对单个文件分别排序
	for (int i = 0; i < inputFilesPath.size(); ++i) {
		FileCoder inputFileCoder(inputFilesPath[i], blockSize);
		vector<pss> curInputData;
		while (!inputFileCoder.isInputEOF()) {
			vector<pss> tmp = inputFileCoder.decodeInputData();
			curInputData.insert(curInputData.end(), tmp.begin(), tmp.end());
		}
		sort(curInputData.begin(), curInputData.end());
		inputFilesPath[i] += ".tmp";
		FileCoder outputFileCoder(inputFilesPath[i], blockSize);
		outputFileCoder.writeOutputFile(curInputData);
	}
	// 归并排序
	string outputFilePath = mergeSortShuffle(inputFilesPath);
	// 删除中间文件
	removeTmpFiles(inputFilesPath);
	return outputFilePath;
}

string ReduceTask::mergeSortShuffle(vector<string> inputFilesPath) {
	string outputFilePath = "../Files/mr_output_" + to_string(taskID) + ".tmp";
	vector<FileCoder> filesCoder;
	for (int i = 0; i < inputFilesPath.size(); ++i) filesCoder.push_back(FileCoder(inputFilesPath[i], blockSize));
	priority_queue<pssi, vector<pssi>, cmp> pq;
	/**
	* 循环，将pq读空，如果i的空了，就不补
	*/
	FileCoder outputFile(outputFilePath, blockSize);
	for (int i = 0; i < inputFilesPath.size(); ++i) 
		if (!filesCoder[i].isInputEOF())
			pq.push(pssi(filesCoder[i].decodeOneInputData(), i));
	while (pq.size()) {
		pssi cur = pq.top();
		int index = cur.second;
		pss outputData = cur.first;
		pq.pop();
		if (!outputData.first.size()) continue;
		outputFile.writeOneRow(outputData);
		if (!filesCoder[index].isInputEOF()) 
			pq.push(pssi(filesCoder[index].decodeOneInputData(), index));
	}
	return outputFilePath;
}

vector<pss> ReduceTask::readTmpFileAndRunReducer(string tmpFile) {
	// 打开有序临时文件
	FileCoder fileCoder(tmpFile, blockSize);
	vector<pss> reduceResult;
	while (!fileCoder.isInputEOF()) {
		vector<pss> inputData = fileCoder.decodeOneKeyInputData();
		reduceResult.push_back(userDefinedTask(inputData));
	}
	return reduceResult;
}
