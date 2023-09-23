#pragma once
enum WorkerStateEnum {
	//前两个说明在做任务，Idle：空闲 Dead：过长时间
	Reduce, Map, Idle, Dead
};