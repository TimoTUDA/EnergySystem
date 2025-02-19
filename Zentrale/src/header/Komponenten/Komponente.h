//
// Created by Timo on 11.12.22.
//
#include "map"
#include "string"
#include "mutex"
#include "queue"
#include <bits/stdc++.h>

#ifndef ZENTRALE_KOMPONENTE_H
#define ZENTRALE_KOMPONENTE_H


class Komponente {
protected:
    std::string type;
    std::string name;
    int id;
    bool status;
    //timestamp, value
    std::map<time_t ,double> values;
    //max heap for msgID
    std::vector<int> maxHeap;
    std::mutex mtx;
    std::string ip {};


    //helper function
    static bool cmp(int i, int j);

public:

    Komponente();
    virtual ~Komponente();

    virtual std::string getType() = 0;
    virtual const std::map<time_t , double> &getValues() const;
    virtual void addNewValue(time_t timestamp, double value);
    virtual double getLatestValue() const;
    const std::string &getName() const;
    int getId() const;
    std::vector<int> checkMissingMsg(int msgID);
    std::string getIp() const;
    void setIp(std::string ip);
    unsigned long long getMsgCount() const;
};


#endif //ZENTRALE_KOMPONENTE_H
