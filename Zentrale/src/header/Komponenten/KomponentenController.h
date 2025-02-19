//
// Created by Timo on 12.12.22.
//

#ifndef ZENTRALE_KOMPONENTENCONTROLLER_H
#define ZENTRALE_KOMPONENTENCONTROLLER_H

#include <mqtt/callback.h>
#include "Komponente.h"
#include "unordered_map"
#include "iostream"
#include "Erzeuger.h"
#include "Verbraucher.h"
#include "mutex"
#include "../UDPServer/UDPCallback.h"
#include "vector"
#include "../UDPSender/KomponentenUdpSender.h"
#include "../Rpc/MqttInterfaceZentrale.h"
#include "time.h"
#include "random"


class KomponentenController : public UDPCallback, public mqtt::callback {
private:
    //id,Komponente
    std::unordered_map<int, Komponente*> komponenten;
    std::mutex mtx;

    //name, id
    std::unordered_map<std::string, int> nameMapping;
    static string createMissingMessageJSON(int msgID) ;
    KomponentenUdpSender* sender;
    unsigned long long msgCount = 0;
    KomponentenController();
    static KomponentenController* instance;
    MqttInterfaceZentrale *zentrale;


    bool enableDataOutput = true;
    bool enableMissingMessages = true;


public:
    // Constructor, destructor
    static KomponentenController* getInstance();
    virtual ~KomponentenController();

    // Getter
    Komponente* getKomponenteById(int id);
    Komponente* getKomponenteByName (string name);
    std::vector<std::string> getKomponentenNamen();
    std::vector<int> getKomponentenIDs();
    std::vector<Komponente*> getErzeuger();
    std::vector<Komponente*> getVerbraucher();
    unsigned long long int getMsgCount() const;
    const unordered_map<int, Komponente *> &getKomponenten() const;

    void setZentraleInterface(MqttInterfaceZentrale* zentrale);

    // Callback function for udp server
    void processMessageUdp(std::string ip, std::string message) override;

    // Callback functions for mqtt
    void connected(const string &message) override;
    void connection_lost(const string &message) override;
    void message_arrived(mqtt::const_message_ptr ptr) override;

    // Testmode
    void setTestMode(bool enableDataOutput, bool enableMissingMessagesOutput);
};


#endif //ZENTRALE_KOMPONENTENCONTROLLER_H
