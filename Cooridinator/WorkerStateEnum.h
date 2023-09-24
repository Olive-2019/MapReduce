#pragma once
enum WorkerStateEnum {
	//前两个说明在做任务，Idle：空闲 Dead：死亡 TimeOut：任务超时
	Reduce, Map, Idle, Dead, TimeOut
};