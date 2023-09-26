#include "MapTask.h"
string MapTask::run(string inputFilePath, int nReduce) {
	// 调用父类的run做统一操作
	Task::run(inputFilePath, nReduce);
	// 存放临时文件名
	vector<string> tmpOutputFilesPath;
	// 注入临时文件名，方便停机
	for (int partitionID = 0; partitionID < nReduce; ++partitionID)
		tmpOutputFilesPath.push_back("../Files/mr_" + to_string(taskID) + "_" + to_string(partitionID) + ".tmp");
	// 文件输入输出管理
	FileCoder fileCoder(inputFilePath, blockSize);
	// 缓冲区
	vector<pss> outputKV;
	// 没读完文件就循环，一次读入约blocksize左右大小的数据
	while (!fileCoder.isInputEOF()) {
		// 检查是否需要停机，停机就删除所有临时文件并退出
		if (Task::stopTaskRunner(tmpOutputFilesPath)) return "exit";
		vector<pss> inputKV = fileCoder.decodeMapperInputData(), tmpOutputKV;
		tmpOutputKV = userDefinedTask(inputKV);
		// 缓冲区溢出
		if (sizeof(tmpOutputKV) + sizeof(outputKV) > blockSize) {
			// 溢出前先进行shuffle操作，即将所有数据根据key值分到不同的partition region
			vector<vector<pss>> curOutputPartitionData = shuffle(outputKV);
			// 缓冲区溢出到临时文件中
			for (int partitionID = 0; partitionID < nReduce; ++partitionID)
			   fileCoder.writeMapperOutputTmpFile(tmpOutputFilesPath[partitionID], 
				   curOutputPartitionData[partitionID]);
			// 写完刷新缓冲区
			outputKV.clear();
		}
		// 继续往缓冲区写
		outputKV.insert(outputKV.end(), tmpOutputKV.begin(), tmpOutputKV.end());
	}
	// 最后把缓冲区刷新一下
	if (outputKV.size()) {
		vector<vector<pss>> curOutputPartitionData = shuffle(outputKV);
		for (int partitionID = 0; partitionID < nReduce; ++partitionID)
			fileCoder.writeMapperOutputTmpFile(tmpOutputFilesPath[partitionID],
				curOutputPartitionData[partitionID]);
	}
	// 最后检查一次是否需要停机，停机就删除所有临时文件并退出
	if (Task::stopTaskRunner(tmpOutputFilesPath)) return "exit";
	// 将临时文件转成正式的data文件
	for (int partitionID = 0; partitionID < nReduce; ++partitionID)
		fileCoder.writeMapperOutputFile(tmpOutputFilesPath[partitionID]);
	// 调用父类的结束任务后处理函数
	Task::afterRun();
	return "over";
}
int MapTask::partitionValue(string key) {
	int ans = 0;
	for (int i = 0; i < key.size(); ++i) ans += key[i], ans %= nReduce;
	return ans;
}
vector<vector<pss>> MapTask::shuffle(vector<pss> outputData) {
	vector<vector<pss>> curOutputPartitionData(nReduce, vector<pss>());
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
// 用WorkCount做一下
vector<pss> MapTask::userDefinedTask(const vector<pss>& inputKV) {
	vector<pss> outputKV;
	for (auto it = inputKV.begin(); it != inputKV.end(); ++it) {
		string inputline = it->second;
		vector<string> cur = stringSplit(inputline, ' ');
		for (auto curIt = cur.begin(); curIt != cur.end(); ++curIt) 
			outputKV.push_back(pss(*curIt, "1"));
	}
	return outputKV;
}
