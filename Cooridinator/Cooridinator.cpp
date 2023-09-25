#include "Cooridinator.h"
Cooridinator::Cooridinator(int nReduce) : nReduce(nReduce), workersNum(0), workerCap(50), nMap(0){

}

void Cooridinator::run(string inputFilePath) {
	vector<string> spiltedInputFilesPath = spiltInputFile(inputFilePath);
	nMap = spiltedInputFilesPath.size() - 1;
	vector<thread*> t;
	//while (nMap >= 0) {
		int workerID = getIdleWorker(WorkerStateEnum::Map);
		if (workerID >= 0) t.push_back(new thread(&WorkerState::signTask, &workersList[workerID], WorkerStateEnum::Map,
			spiltedInputFilesPath[nMap], workerID));
	//}
	for (int i = 0; i < t.size(); ++i) {
		t[i]->join();
		delete t[i];
		t[i] = NULL;
	}

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
	vector<string> spiltedFile = FileSpliter(inputFilePath, 64).split();
	return spiltedFile;
}
void Cooridinator::heartBreak(int workerID) {
	workersListLock.lock();
	workersList[workerID].heartBreak();
	workersListLock.unlock();
}

