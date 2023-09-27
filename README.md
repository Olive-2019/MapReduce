# MapReduce
本项目使用C++实现轻量级的MapReduce框架，由于没有使用共享文件系统，故目前仍只能在单机上运行
依赖rest_rpc的rpc调用实现节点间通信
# 使用方法
在Master项目中的MapTask和ReduceTask中userDefinedTask分别定义map操作和reduce操作
pss其实就是pair<string, string>的简写，也就是一个KV键值对，第一个是key，第二个是value
## 例子
### 倒排索引
#### Map
```c++
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
```
#### Reduce
```c++
pss ReduceTask::userDefinedTask(vector<pss> inputKV) {
	string key = inputKV[0].first;
	string line;
	for (pss oneWord : inputKV) {
		line += oneWord.second;
		line += ',';
	}
	line.pop_back();
	return pss(key, line);
}
```
### 单词计数
#### Map
```c++
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
```
#### Reduce
```c++
pss ReduceTask::userDefinedTask(vector<pss> inputKV) {
	string key = inputKV[0].first;
	int cnt = inputKV.size();
	return pss(key, to_string(cnt));
}
```
# 项目结构
整个MapReduce项目分为两个模块，分别是Coordinator和Worker，Coordinator负责分配任务和节点调度的工作，Worker负责执行任务
## Cooridinator
Cooridinator中有三个类，分别是Cooridinator、FileIO和WorkerState
1. Coordinator的功能主要在于调度worker和运行mapreduce的逻辑，前者逻辑在于scheduleTask，后者逻辑在于run
2. FileIO的功能在于文件读写，为Cooridinator屏蔽掉底层文件读写的代码，支持逻辑层面的文件操作
3. WorkerState的功能在于对单个Worker的状态管理和任务分配
### 任务调度逻辑scheduleTask
实现逻辑：循环访问task
* 若当前task未分配，则寻找一个worker分配
**	若当前没有空闲worker，则轮询所有worker工作状态，若已完成，则将其恢复到idle状态
***		若轮询worker后仍没有worker空闲，则休眠一段时间后再来一遍
**	若有空闲则分配
* 若已分配，则查看完成情况
**	若已完成，则加入到输出文件路径队列里
**	若未完成，则检查worker状态，若dead或timeout，则为其寻找下一个idle worker
### Cooridinator运行逻辑run
1. 将输入文件按照blockSize大小分块，块数即mapper数量
2. 将输入文件分配给不同的mapper，并调度worker执行mapper任务
3. 执行reduce任务
4. 将不同reduce任务输出汇总成总输出文件
## Worker
Worker中有两个模块，第一个是文件管理的FileCoder类，功能在逻辑上等同于Cooridinator的FileIO，但是功能更为丰富，提供基于KV键值对的逻辑文件读写功能；另一个模块是Task模块，包括父类Task，子类MapTask和ReduceTask。
父类Task提供了心跳信号的管理功能和taskID的解码功能，子类在run方法中分别调用父类方法即完成了心跳信号管理.
子类MapTask功能在于执行节点的map任务，大体逻辑是
1. 读入输入文件，并运行用户定义的map任务映射
2. 当缓冲区满时，先执行shuffle操作，将所有数据根据key值分到不同的partition region（给不同的reduce任务）
3. 将缓冲区溢出到不同region的临时文件中，刷新缓冲区
4. 继续读入数据并完成上述操作，直到文件处理完成


子类ReduceTask在于执行节点的reduce任务，大体逻辑是
1. 执行shuffle操作，将来自不同map任务的中间文件进行外部归并排序，生成有序的中间文件
2. 按key值读入有序中间文件并传递给用户定义的reduce函数，根据用户逻辑对其进行处理
3. 将用户逻辑输出的结果写入当前reduce结点的输出文件中