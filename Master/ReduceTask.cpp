#include "ReduceTask.h"

string ReduceTask::run(string inputFilePath, int otherTaskNum) {
	Task::run(inputFilePath, otherTaskNum);
	// reduce ��������ļ�·��
	string outputFilePath = "../Files/mr_output_" + to_string(taskID) + ".data";
	// �����ļ����򣬲���������ʱ�ļ�
	string shuffledInputDataPath = shuffle();
	// ���������ļ�������reduce����
	vector<pss> reduceResult = readTmpFileAndRunReducer(shuffledInputDataPath);
	// ɾ�������м��ļ�
	removeTmpFiles(vector<string>{shuffledInputDataPath});
	// д������ļ�
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
	
	// �Ե����ļ��ֱ�����
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
	// �鲢����
	string outputFilePath = mergeSortShuffle(inputFilesPath);
	// ɾ���м��ļ�
	removeTmpFiles(inputFilesPath);
	return outputFilePath;
}

string ReduceTask::mergeSortShuffle(vector<string> inputFilesPath) {
	string outputFilePath = "../Files/mr_output_" + to_string(taskID) + ".tmp";
	vector<FileCoder> filesCoder;
	for (int i = 0; i < inputFilesPath.size(); ++i) filesCoder.push_back(FileCoder(inputFilesPath[i], blockSize));
	priority_queue<pssi, vector<pssi>, cmp> pq;
	/**
	* ѭ������pq���գ����i�Ŀ��ˣ��Ͳ���
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
	// ��������ʱ�ļ�
	FileCoder fileCoder(tmpFile, blockSize);
	vector<pss> reduceResult;
	while (!fileCoder.isInputEOF()) {
		vector<pss> inputData = fileCoder.decodeOneKeyInputData();
		reduceResult.push_back(userDefinedTask(inputData));
	}
	return reduceResult;
}
