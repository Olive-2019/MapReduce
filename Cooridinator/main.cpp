#include "Cooridinator.h"
#include <iostream>
#include <string>
#include "../rest_rpc/include/rest_rpc.hpp"
#include <thread>
using namespace rest_rpc::rpc_service;

using namespace rest_rpc;
using std::thread;
using namespace std;
Cooridinator* coordinator;


int main() {
    coordinator = new Cooridinator(1, 1);
    coordinator->registerWorker("127.0.0.1", 9000);
    coordinator->run("example");
    cout << endl;
    return 0;
}