#include "Cooridinator.h"
Cooridinator::Cooridinator(int nReduce) : nReduce(nReduce), workersNum(0), workerCap(50), nMap(0){

}

void Cooridinator::run(string inputFilePath) {
	// �������ļ��ֿ飬����װ��key-value��ʽ
	vector<string> mapperInputFilesPath = spiltInputFile(inputFilePath);
	// ����mapper����
	vector<string> mapperOutputFilesPath = scheduleTask(mapperInputFilesPath, WorkerStateEnum::Map);
	for (int i = 0; i < mapperOutputFilesPath.size(); ++i) cout << mapperInputFilesPath[i] << endl;
}
/*
* �������
* ʵ���߼���ѭ������task
* ����ǰtaskδ���䣬��Ѱ��һ��worker����
*	����ǰû�п���worker������ѯ����worker����״̬��������ɣ�����ָ���idle״̬
*		����ѯworker����û��worker���У�������һ��ʱ�������һ��
*	���п��������
* ���ѷ��䣬��鿴������
*	������ɣ�����뵽����ļ�·��������
*	��δ��ɣ�����worker״̬����dead��timeout����Ϊ��Ѱ����һ��idle worker
*/
vector<string> Cooridinator::scheduleTask(const vector<string>& inputFilesPath, WorkerStateEnum taskType) {
	// ���ÿһ��task������ļ�·��
	vector<string> outputFilesPath;
	// ���ÿһ��worker������ļ�·��
	vector<future<string>> workerOutputFilesPathFuture(workersList.size());
	// ��ǰʣ��task��id
	set<int> tasksID;
	// taskID��workerIDӳ������
	map<int, vector<int>> tasksIdToWorkersID;
	// ע��taskID ��ʼ��taskID��workerIDӳ������
	for (int i = 0; i < inputFilesPath.size(); ++i) {
		tasksID.insert(i);
		tasksIdToWorkersID[i] = vector<int>();
	}
	//�����������
	while (tasksID.size()) {
		for (set<int>::iterator taskID = tasksID.begin(); taskID != tasksID.end(); ) {
			// �ѷ���worker
			if (tasksIdToWorkersID[*taskID].size() > 0) {
				bool isFinish = false;
				// ��ѯ����������worker�ڵ�
				for (int i = 0; i < tasksIdToWorkersID[*taskID].size(); ++i) {
					int workerID = tasksIdToWorkersID[*taskID][i];
					string curOutputFilePath = checkWorker(workerOutputFilesPathFuture[workerID], workerID);
					// ������һ���������
					if (!curOutputFilePath.size()) {
						// ���д��
						outputFilesPath.push_back(curOutputFilePath);
						// ���±�־λ
						isFinish = true;
						break;
					}
					// ����û����
					// 1. ����ʱ 2. ��ǰ����δ��������worker������ 3. ��ǰ��idle worker ������һ��worker����ǰtask
					if (workersList[workerID].isTimeOut() && tasksIdToWorkersID[*taskID].size() < 3) {
						int newWorkerID = getIdleWorker(taskType);
						if (newWorkerID >= 0) {
							workerOutputFilesPathFuture[newWorkerID] = async(&WorkerState::signTask, &workersList[newWorkerID],
								WorkerStateEnum::Map, inputFilesPath[*taskID], newWorkerID, *taskID);
							tasksIdToWorkersID[*taskID].push_back(newWorkerID);
						}
					}
					
				}
				// ���������
				if (isFinish) {
					// ��������������ֹͣ�ѷ��������worker����
					for (int i = 0; i < tasksIdToWorkersID[*taskID].size(); ++i) stopWorker(tasksIdToWorkersID[*taskID][i]);
					// ����ʣ��task�б�
					taskID = deleteTask(tasksID, taskID);
					// ��յ�ǰ�����worker�б�
					tasksIdToWorkersID[*taskID].clear();
				}
				else taskID++;// ׼����һ�����task
			}
			// δ����worker 
			else {
				int workerID = getIdleWorker(taskType);
				// û�п���worker
				if (workerID < 0) {
					// ��ѯworker
					vector<string> tmpOutputFilesPath = poolingWorker(workerOutputFilesPathFuture, tasksID);
					// ��ѯ���п���worker
					if (tmpOutputFilesPath.size()) {
						// ����ѯ���������ļ��ӽ���
						outputFilesPath.insert(outputFilesPath.end(),
							tmpOutputFilesPath.begin(), tmpOutputFilesPath.end());
						// �ҿ���worker
						workerID = getIdleWorker(taskType);
						if (workerID < 0) throw exception("Cooridinator::scheduleTask logical error");
						// ��������
						workerOutputFilesPathFuture[workerID] = async(&WorkerState::signTask, 
							&workersList[workerID], taskType, inputFilesPath[*taskID], workerID, *taskID);
						tasksIdToWorkersID[*taskID].push_back(workerID);
						// ׼����һ�����task
						taskID++;
					}
					else {
						// ��Ȼworker��æ�ţ�������Ϣһ���
						Sleep(100);
					}
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

vector<string> Cooridinator::poolingWorker(const vector<future<string>>& futureOutputFilesPath, set<int>& tasks) {
	vector<string> outputFilesPath;
	for (int i = 0; i < futureOutputFilesPath.size(); ++i) {
		string filePath = checkWorker(futureOutputFilesPath[i], i, tasks);
		if (!filePath.size()) outputFilesPath.push_back(filePath);
	}
	return outputFilesPath;
}

string Cooridinator::checkWorker(const future<string>& futureOutputFilePath, int workerID, set<int>& tasks) {
	// ֻ����ִ������Ľڵ�
	if (workersList[workerID].getState() == WorkerStateEnum::Idle ||
		workersList[workerID].getState() == WorkerStateEnum::Dead ||
		workersList[workerID].getState() == WorkerStateEnum::TimeOut) return "";
	//ֻ��1s
	future_status status = futureOutputFilePath.wait_for(seconds(1));
	// ������Ѿ���ɵģ���������ΪIdle������������ļ�·��
	if (status == future_status::ready) {
		setWorkerIdle(workerID);
		tasks.erase(workersList[workerID].getTaskID());
		return futureOutputFilePath._Get_value();
	}
	return "";
}

bool Cooridinator::registerWorker(string ip, int port) {
	workersListLock.lock();
	if (workersList.size() < workerCap) {
		WorkerState worker(ip, port, workersList.size());
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
	workersList[workerID].stop();
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

