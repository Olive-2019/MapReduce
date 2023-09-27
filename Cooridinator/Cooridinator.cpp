#include "Cooridinator.h"
Cooridinator::Cooridinator(int nReduce) : nReduce(nReduce), workersNum(0), workerCap(50), nMap(0){

}

void Cooridinator::run(string inputFilePath) {
	// 将输入文件分块，并分装成key-value形式
	vector<string> mapperInputFilesPath = spiltInputFile(inputFilePath);
	// 分配mapper任务
	vector<string> mapperOutputFilesPath = scheduleTask(mapperInputFilesPath, WorkerStateEnum::Map);
	cout << "mapper task over" << endl;
}
/*
* 任务分配
* 实现逻辑：循环访问task
* 若当前task未分配，则寻找一个worker分配
*	若当前没有空闲worker，则轮询所有worker工作状态，若已完成，则将其恢复到idle状态
*		若轮询worker后仍没有worker空闲，则休眠一段时间后再来一遍
*	若有空闲则分配
* 若已分配，则查看完成情况
*	若已完成，则加入到输出文件路径队列里
*	若未完成，则检查worker状态，若dead或timeout，则为其寻找下一个idle worker
*/
vector<string> Cooridinator::scheduleTask(const vector<string>& inputFilesPath, WorkerStateEnum taskType) {
	// 存放每一个task的输出文件路径
	vector<string> outputFilesPath;
	// 存放每一个worker的输出文件路径
	vector<shared_future<string>> workerOutputFilesPathFuture(workersList.size());
	// 当前剩余task的id
	set<int> tasksID;
	// taskID和workerID映射容器
	map<int, vector<int>> tasksIdToWorkersID;
	// 注入taskID 初始化taskID和workerID映射容器
	for (int i = 0; i < inputFilesPath.size(); ++i) {
		tasksID.insert(i);
		tasksIdToWorkersID[i] = vector<int>();
	}
	//任务分配流程
	while (tasksID.size()) {
		for (set<int>::iterator taskID = tasksID.begin(); taskID != tasksID.end(); ) {
			// 已分配worker
			if (tasksIdToWorkersID[*taskID].size() > 0) {
				bool isFinish = false;
				// 轮询该任务分配的worker节点
				for (int i = 0; i < tasksIdToWorkersID[*taskID].size(); ++i) {
					int workerID = tasksIdToWorkersID[*taskID][i];
					string curOutputFilePath;
					try {
						// 最后一次不知道为什么不能返回值
						curOutputFilePath = checkWorker(workerOutputFilesPathFuture[workerID], workerID, tasksID);
						//cout << "Cooridinator::scheduleTask " << curOutputFilePath << endl;
					}
					catch (exception e) {
						cout << e.what() << endl;
					}
					
					// 有其中一个任务结束
					if (curOutputFilePath.size()) {
						// 输出写入
						outputFilesPath.push_back(curOutputFilePath);
						// 更新标志位
						isFinish = true;
						break;
					}
					// 任务没结束
					// 1. 任务超时 2. 当前任务未分配三个worker及以上 3. 当前有idle worker 则多分配一个worker给当前task
					if (workersList[workerID].isTimeOut() && tasksIdToWorkersID[*taskID].size() < 3) {
						int newWorkerID = getIdleWorker(taskType);
						if (newWorkerID >= 0) {
							workerOutputFilesPathFuture[newWorkerID] = async(&WorkerState::signTask, &workersList[newWorkerID],
								WorkerStateEnum::Map, inputFilesPath[*taskID], newWorkerID, *taskID, nReduce);
							tasksIdToWorkersID[*taskID].push_back(newWorkerID);
						}
					}
					
				}
				// 该任务结束
				if (isFinish) {
					// tasksID没有东西就直接返回了 
					if (!tasksID.size()) break;
					// 如果任务结束，则停止已分配的所有worker任务
					for (int i = 0; i < tasksIdToWorkersID[*taskID].size(); ++i) stopWorker(tasksIdToWorkersID[*taskID][i]);
					// 清空当前任务的worker列表
					tasksIdToWorkersID[*taskID].clear();
					// 更新剩余task列表
					taskID = deleteTask(tasksID, taskID);
				}
				else {
					taskID++;// 准备下一个检查task
				}
			}
			// 未分配worker 
			else {
				int workerID = getIdleWorker(taskType);
				// 没有空闲worker
				if (workerID < 0) {
					// 轮询worker
					vector<string> tmpOutputFilesPath = poolingWorker(workerOutputFilesPathFuture, tasksID);
					// 轮询后有空闲worker
					if (tmpOutputFilesPath.size()) {
						// 将轮询出来的新文件加进来
						outputFilesPath.insert(outputFilesPath.end(),
							tmpOutputFilesPath.begin(), tmpOutputFilesPath.end());
						// 找空闲worker
						workerID = getIdleWorker(taskType);
						if (workerID < 0) throw exception("Cooridinator::scheduleTask logical error");
						// 分配任务
						workerOutputFilesPathFuture[workerID] = async(&WorkerState::signTask, 
							&workersList[workerID], taskType, inputFilesPath[*taskID], workerID, *taskID, nReduce);
						tasksIdToWorkersID[*taskID].push_back(workerID);
						// 准备下一个检查task
						taskID++;
					}
					else {
						// 既然worker都忙着，就先休息一会儿
						Sleep(100);
					}
				}
				else {
					// 有空闲worker
					if (workerID < 0) throw exception("Cooridinator::scheduleTask logical error");
					// 分配任务
					workerOutputFilesPathFuture[workerID] = async(&WorkerState::signTask,
						&workersList[workerID], taskType, inputFilesPath[*taskID], workerID, *taskID, nReduce);
					tasksIdToWorkersID[*taskID].push_back(workerID);
					// 准备下一个检查task
					taskID++;
				}
			}
			
		}
	}
	return outputFilesPath;
}


set<int>::iterator Cooridinator::deleteTask(set<int>& leftTask, set<int>::iterator deletedTask) {
	set<int>::iterator nextTask = deletedTask;
	nextTask++;
	leftTask.erase(deletedTask);
	return nextTask;
}

vector<string> Cooridinator::poolingWorker(const vector<shared_future<string>>& futureOutputFilesPath, set<int>& tasks) {
	vector<string> outputFilesPath;
	for (int i = 0; i < futureOutputFilesPath.size(); ++i) {
		string filePath = checkWorker(futureOutputFilesPath[i], i, tasks);
		if (filePath.size()) outputFilesPath.push_back(filePath);
	}
	return outputFilesPath;
}

string Cooridinator::checkWorker(const shared_future<string>& futureOutputFilePath, int workerID, set<int>& tasks) {
	// 只处理执行任务的节点
	if (workersList[workerID].getState() == WorkerStateEnum::Idle ||
		workersList[workerID].getState() == WorkerStateEnum::Dead ||
		workersList[workerID].getState() == WorkerStateEnum::TimeOut) return "";
	// 就在这里无法返回
	if (futureOutputFilePath._Is_ready()) {
		// 如果已经完成，将其设置为Idle，并读出输出文件路径
		setWorkerIdle(workerID);
		tasks.erase(workersList[workerID].getTaskID());
		return futureOutputFilePath.get();
	}
	
	return "";
}

bool Cooridinator::registerWorker(string ip, int workerServerPort, int workerStopControllerPort) {
	workersListLock.lock();
	if (workersList.size() < workerCap) {
		WorkerState worker(ip, workerServerPort, workerStopControllerPort, workersList.size());
		workersList.push_back(worker);
		workersListLock.unlock();
		return true;
	}
	workersListLock.unlock();
	return false;
}
int Cooridinator::getIdleWorker(WorkerStateEnum workerState) {
	workersListLock.lock();
	int IdleWorkerID = -1;
	for (int i = 0; i < workersList.size(); ++i) {
		if (workersList[i].getState() == WorkerStateEnum::Idle) {
			IdleWorkerID = i;
			workersList[i].setState(workerState);
			break;
		}
	}
	workersListLock.unlock();
	return IdleWorkerID;
}
void Cooridinator::setWorkerIdle(int workerID) {
	workersListLock.lock();
	workersList[workerID].setState(WorkerStateEnum::Idle);
	workersListLock.unlock();
}

void Cooridinator::stopWorker(int workerID) {
	workersListLock.lock();
	workersList[workerID].stopTask();
	workersListLock.unlock();
}

vector<string> Cooridinator::spiltInputFile(string inputFilePath) {
	vector<string> spiltedFile = FileSpliter(inputFilePath, 64).split();
	return spiltedFile;
}
void Cooridinator::heartBreak(int workerID) {
	workersListLock.lock();
	workersList[workerID].heartBreak();
	workersListLock.unlock();
}

