#include "MapTask.h"
#include <iostream>
using namespace std;
string MapTask::run(string inputFilePath, int otherTaskNum) {
	// ���ø����run��ͳһ����
	Task::run(inputFilePath, otherTaskNum);
	// �����ʱ�ļ���
	vector<string> tmpOutputFilesPath;
	// ע����ʱ�ļ���������ͣ��
	for (int partitionID = 0; partitionID < otherTaskNum; ++partitionID)
		tmpOutputFilesPath.push_back("../Files/mr_" + to_string(taskID) + "_" + to_string(partitionID) + ".tmp");
	// �ļ������������
	FileCoder fileCoder(inputFilePath, blockSize);
	// ������
	vector<pss> outputKV;
	// û�����ļ���ѭ����һ�ζ���Լblocksize���Ҵ�С������
	while (!fileCoder.isInputEOF()) {
		// ����Ƿ���Ҫͣ����ͣ����ɾ��������ʱ�ļ����˳�
		if (Task::stopTaskRunner(tmpOutputFilesPath)) return "exit";
		vector<pss> inputKV = fileCoder.decodeInputData(), tmpOutputKV;
		tmpOutputKV = userDefinedTask(inputKV);
		// ���������
		if (sizeof(tmpOutputKV) + sizeof(outputKV) > blockSize) {
			// ���ǰ�Ƚ���shuffle�����������������ݸ���keyֵ�ֵ���ͬ��partition region
			vector<vector<pss>> curOutputPartitionData = shuffle(outputKV);
			// �������������ʱ�ļ���
			for (int partitionID = 0; partitionID < otherTaskNum; ++partitionID) {
				FileCoder outputFileCoder(tmpOutputFilesPath[partitionID], blockSize);
				outputFileCoder.writeOutputFile(curOutputPartitionData[partitionID]);
			}
			   
			// д��ˢ�»�����
			outputKV.clear();
		}
		// ������������д
		outputKV.insert(outputKV.end(), tmpOutputKV.begin(), tmpOutputKV.end());
	}
	
	if (taskID == 3) {
		cout << "here" << endl;
	}

	// ���ѻ�����ˢ��һ��
	if (outputKV.size()) {
		vector<vector<pss>> curOutputPartitionData = shuffle(outputKV);
		for (int partitionID = 0; partitionID < otherTaskNum; ++partitionID) {
			FileCoder outputFileCoder(tmpOutputFilesPath[partitionID], blockSize);
			outputFileCoder.writeOutputFile(curOutputPartitionData[partitionID]);
		}
	}
	// �����һ���Ƿ���Ҫͣ����ͣ����ɾ��������ʱ�ļ����˳�
	if (Task::stopTaskRunner(tmpOutputFilesPath)) return "exit";

	/*if (taskID == 3) {
		cout << "here" << endl;
	}*/

	// ����ʱ�ļ�ת����ʽ��data�ļ�
	for (int partitionID = 0; partitionID < otherTaskNum; ++partitionID)
		fileCoder.writeMapperOutputFile(tmpOutputFilesPath[partitionID]);
	// ���ø���Ľ������������
	Task::afterRun();
	cout << "MapTask::run over" << endl;
	return "over";
}
int MapTask::partitionValue(string key) {
	int ans = 0;
	for (int i = 0; i < key.size(); ++i) ans += key[i], ans %= otherTaskNum;
	return ans;
}
vector<vector<pss>> MapTask::shuffle(vector<pss> outputData) {
	vector<vector<pss>> curOutputPartitionData(otherTaskNum, vector<pss>());
	for (auto it = outputData.begin(); it != outputData.end(); ++it) 
		curOutputPartitionData[partitionValue(it->first)].push_back(*it);
	return curOutputPartitionData;
}
vector<string> MapTask::stringSplit(const string& str, char delim) const{
	stringstream ss(str);
	string item;
	vector<string> elems;
	while (getline(ss, item, delim)) 
		if (!item.empty()) elems.push_back(item);
	return elems;
}
// ��WorkCount��һ��
vector<pss> MapTask::userDefinedTask(const vector<pss>& inputKV) {
	vector<pss> outputKV;
	for (auto it = inputKV.begin(); it != inputKV.end(); ++it) {
		string inputline = it->second;
		vector<string> cur = stringSplit(inputline, ' ');
		for (auto curIt = cur.begin(); curIt != cur.end(); ++curIt) 
			outputKV.push_back(pss(*curIt, it->first));
	}
	return outputKV;
}
