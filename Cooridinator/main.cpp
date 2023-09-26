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

void heartBreak(rpc_conn conn, int workerID) {
    coordinator->heartBreak(workerID);
}
void runServer(int port) {
    rpc_server server(port, 1);
    server.register_handler("heartBreak", heartBreak);
    server.run();//Æô¶¯·þÎñ¶Ë
}
int main() {
    try {
        thread runHeartBreak(runServer, 8998);
        coordinator = new Cooridinator(2);
        coordinator->registerWorker("127.0.0.1", 9000, 8999);
        coordinator->run("../Files/example.txt");
        runHeartBreak.join();
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
   
    cout << endl;
    return 0;
}