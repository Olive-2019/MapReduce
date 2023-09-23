#include "Cooridinator.h"
Cooridinator::Cooridinator(int nMap, int nReduce) : nMap(nMap), nReduce(nReduce), 
workersNum(0), workerCap(50){

}

void Cooridinator::run(string inputFilePath) {
	vector<string> spiltedInputFilesPath = spiltInputFile(inputFilePath);
	// 选worker分配map任务
	for (int i = 0; i < nMap; ++i) {
		int workerID = getIdleWorker(WorkerStateEnum::Map);
		if (workerID < 0) {
			throw exception("workers is not enough！");
		}
		cout << workersList[i].signTask(WorkerStateEnum::Map, spiltedInputFilesPath[i]) << endl;
	}
	//
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

vector<string> Cooridinator::spiltInputFile(string inputFilePath) {
	// 此处仅测试使用，实际需要文件划分
	vector<string> spiltedFile{ inputFilePath };
	return spiltedFile;
}
