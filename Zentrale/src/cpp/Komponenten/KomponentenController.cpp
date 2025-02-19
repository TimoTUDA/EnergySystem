//
// Created by Timo on 12.12.22.
//

#include "../../header/Komponenten/KomponentenController.h"

KomponentenController* KomponentenController::instance = nullptr;

KomponentenController::KomponentenController() {
    this->sender = new KomponentenUdpSender();
}

KomponentenController::~KomponentenController() {
    delete sender;
}

void KomponentenController::processMessageUdp(std::string ip, std::string message) {
    ++msgCount;

    //format {"type": "Unternehmen", "name": "FLEISCHER", "id": 123,"value": 2006.550000}
    std::string type;
    std::string name;
    int id;
    double value;
    time_t time;
    int msgID = -1;
    bool status;
    try{
        //get type
        std::size_t pos = message.find("\"type\"");
        pos += 9;
        type = message.substr(pos, message.substr(pos,message.length() - pos).find_first_of('"'));

        //get name
        pos = message.find("\"name\"");
        pos += 9;
        name = message.substr(pos, message.substr(pos,message.length() - pos).find_first_of('"'));

        //get id
        pos = message.find("\"id\"");
        pos += 6;
        std::string tmp;
        int i = pos;
        while (i < message.size() && message[i] >= '0' && message[i] <= '9'){
            tmp += message[i];
            ++i;
        }
        id = std::stoi(tmp);

        //get value
        pos = message.find("\"value\"");
        pos += 9;
        i = pos;
        tmp = "";

        while (i < message.size() && ((message[i] >= '0' && message[i] <= '9') || message[i] == '.')){
            tmp += message[i];
            ++i;
        }
        value = std::stod(tmp);

        //get time
        pos = message.find("\"time\"");
        pos += 8;
        i = pos;
        tmp = "";

        while (i < message.size() && message[i] >= '0' && message[i] <= '9'){
            tmp += message[i];
            ++i;
        }
        time = std::stoull(tmp);

        //get msgID
        pos = message.find("\"msgID\"");
        if (pos != message.npos) {
            pos += 9;
            i = pos;
            tmp = "";

            while (i < message.size() && message[i] >= '0' && message[i] <= '9') {
                tmp += message[i];
                ++i;
            }
            msgID = std::stoi(tmp);
        }


        //check for status (only for erzeuger)
        pos = message.find("\"status\"");

        if (pos != string::npos) {
            pos += 10;
            if (pos < message.size()) {
                if (message.at(pos) == '0')
                    status = false;
                else
                    status = true;
            }
        }

        //check for ip (needed for mqtt to get a clients ip address)
        pos = message.find("\"ip\"");

        if (pos != string::npos) {
            pos += 6;
            while (pos < message.length() && message[pos] != ' ' && message[pos] != ',' && message[pos] != '}') {
                ip += message[pos];
                ++pos;
            }
        }

        mtx.lock();
        Komponente* k;

        auto it = komponenten.find(id);
        if (it == komponenten.end()){
            if (type == "Unternehmen" || type == "Haushalt") {
                k = new Verbraucher(id, name, type);
            } else {
                k = new Erzeuger(type, name, id, status);
            }
            k->setIp(ip);
            nameMapping.insert({name, id});
            komponenten.insert({id, k});
            k->addNewValue(time, value);

        } else{
            k = it->second;
            k->addNewValue(time, value);

        }

        //check for missing message
        if (this->enableMissingMessages) {
            vector<int> missingMsg = k->checkMissingMsg(msgID);

            if (missingMsg.size() > 0) {
                cout << "Missing " << missingMsg.size() << " messages from " << k->getName() << ".:(" << endl;
            }

            for (int i = 0; i < missingMsg.size(); ++i) {
                string msg = createMissingMessageJSON(missingMsg[i]);
                std::thread t = sender->komponentenThreadSend(k, msg);
                t.detach();
            }
        }


        mtx.unlock();

        if (this->enableDataOutput) {
            std::cout << "Type: " << type << "\tID: " << id << "\tName: " << name << "\tValue: " << value
                      << "\tTime: " << to_string(time) << std::endl;
        }
    }catch (std::exception &e){
        std::cerr <<"Failed to process the message: " << message << std::endl << e.what() << std::endl;
    }
}



Komponente *KomponentenController::getKomponenteById(int id) {
    auto it = komponenten.find(id);
    if (it == komponenten.end())
        return nullptr;
    return it->second;
}



Komponente *KomponentenController::getKomponenteByName(string name) {

    auto it = nameMapping.find(name);
    if (it == nameMapping.end())
        return nullptr;
    else{
        auto it2 = komponenten.find(it->second);
        if (it2 == komponenten.end())
            return nullptr;
        return it2->second;
    }
}

std::vector<std::string> KomponentenController::getKomponentenNamen() {
    std::vector<std::string> names;
    for (auto it = nameMapping.begin(); it != nameMapping.end(); ++it)
        names.push_back(it->first);
    return names;
}

std::vector<int> KomponentenController::getKomponentenIDs() {
    std::vector<int> ids;
    for (auto it = nameMapping.begin(); it != nameMapping.end(); ++it)
        ids.push_back(it->second);
    return ids;
}

std::vector<Komponente *> KomponentenController::getErzeuger() {
    std::vector<Komponente*> list;
    for (auto it = komponenten.begin(); it != komponenten.end(); ++it){
        if (it->second->getType() != "Haushalt" && it->second->getType() != "Unternehmen")
            list.push_back(it->second);
    }
    return list;
}

std::vector<Komponente *> KomponentenController::getVerbraucher() {
    std::vector<Komponente*> list;
    for (auto it = komponenten.begin(); it != komponenten.end(); ++it){
        if (it->second->getType() == "Haushalt" || it->second->getType() == "Unternehmen")
            list.push_back(it->second);
    }
    return list;
}

KomponentenController* KomponentenController::getInstance() {
    if (KomponentenController::instance == nullptr)
        KomponentenController::instance = new KomponentenController();
    return KomponentenController::instance ;
}

string KomponentenController::createMissingMessageJSON(int msgID) {
    string message = "{";
    message += "\"msgID\": ";
    message += to_string(msgID);
    message += "}";
    return message;
}



/**
 *
 */
void KomponentenController::setTestMode(bool enableDataOutput, bool enableMissingMessages) {
    this->enableDataOutput = enableDataOutput;
    this->enableMissingMessages = enableMissingMessages;
}

unsigned long long int KomponentenController::getMsgCount() const {
    return msgCount;
}

const unordered_map<int, Komponente *> &KomponentenController::getKomponenten() const {
    return komponenten;
}



/**
 *
 */
void KomponentenController::connected(const string &message) {
    cout << "[MQTT] Connected to mqtt broker." << endl;
}

void KomponentenController::connection_lost(const string &message) {
    cout << "[MQTT] Connection to mqtt broker lost." << endl;
}

void KomponentenController::message_arrived(mqtt::const_message_ptr ptr) {
    cout << "[MQTT] Message arrived: " << ptr->to_string() << endl;
    cout << std::chrono::system_clock::now().time_since_epoch().count() << endl;
    // Check topic  "<id>/zentrale/<name>" or "<id>/data/<name>"
    string topic = ptr->get_topic();
    long firstSlash = topic.find('/');
    long length = topic.substr(firstSlash + 1).find('/');
    string type = topic.substr(firstSlash + 1, length);
    if (type == "data") {
        // Forward to other zentralen
        cout << "[Message] Forwarding message." << endl;
        thread t(&MqttInterfaceZentrale::sendToOtherZentralen, zentrale, ptr->get_payload());
        t.detach();
    }

    processMessageUdp("", ptr->get_payload_str());
}



/**
 *
 */
void KomponentenController::setZentraleInterface(MqttInterfaceZentrale *zentrale) {
    this->zentrale = zentrale;
}
