//
// Created by Timo on 12.12.2022
//

#ifndef ERZEUGER_SIMULATOR_H
#define ERZEUGER_SIMULATOR_H

#include "Komponente/Erzeuger.h"
#include "iostream"
#include "unistd.h"
#include "thread"
#include "Kommunikation/UDPKommunikation.h"
#include "Kommunikation/ZentralenKommunikation.h"
#include "Kommunikation/MqttKommunikation.h"
#include "unordered_map"
#include "Kommunikation/UDPCallback.h"
#include "mutex"
#include "Kommunikation/RpcController.h"

using namespace std;

/**
 *
 */
class Simulator : public UDPCallback {
private:
    unordered_map<int,string> msgBuffer;
    Erzeuger* erzeuger;
    ZentralenKommunikation* interface;
    mutex mtx;
    string messageToJSON(string type, string name, int id, double value, time_t t, bool status);
    static int msgID;
    RpcController* rpcController;

    //helper methods
    void simulate();
    bool stopped = false;

public:
    Simulator(Erzeuger *erzeuger, const string& communicationType, int port, string address, string brokerIP, string brokerChannel);
    virtual ~Simulator();

    void processMessage(string ip, std::string string1) override;

    void start();
    void startMissingMsgTest(int msgCount);
    void stop();

    void startPerformanceTest(int msgCount);
};


#endif //ERZEUGER_SIMULATOR_H
