//
// Created by Timo on 12.12.2022
//

#ifndef ERZEUGER_SOLAR_H
#define ERZEUGER_SOLAR_H


#include "Erzeuger.h"
#include "time.h"

using namespace std;

/**
 *
 */
class Solar : public Erzeuger {
private:
    int hour;
    int hourCount;
    int size;

public:
    Solar(int id, string name, int size);
    virtual ~Solar();

    double getLastHourGeneration() override;
};


#endif //ERZEUGER_SOLAR_H
