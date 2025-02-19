//
// Created by Timo on 22.12.2022
//

#ifndef ENERGIEVERSORGER_CLIENT_H
#define ENERGIEVERSORGER_CLIENT_H

#include "Komponenten/KomponentenController.h"
#include "iostream"
#include "ctime"
#include "RpcController.h"


using std::cout;
using std::endl;

class Client {
private:
    KomponentenController* komponentenController;
    RpcController* rpcController {nullptr};


public:
    Client(string ip);
    virtual ~Client();

    void printKomponenten();
};


#endif //ENERGIEVERSORGER_CLIENT_H
