//
// Created by Philipp on 22.12.22.
//

#ifndef ZENTRALE_HTMLGENERATOR_H
#define ZENTRALE_HTMLGENERATOR_H


#include "../Komponenten/KomponentenController.h"
#include "string"
#include "algorithm"
#include "chrono"
#include "../../header/Rpc/ErzeugerRpcClient.h"

using std::string;

class HTMLGenerator {
private:
    KomponentenController* komponentenController;

    string generateHeader(bool mainPage, string title);
    string openBody(bool mainPage);
    string closeBody();
    string generateMainTab();
    string generateTableHead(string& type);
    string closeTable();
    string generateTableRow(string& type, string name, int id, double value, time_t time);
    string generateSubTabs(string type);
    string generateKomponentenListe(string type);
    static string generateKomptTableHead(string type);
    string generateKompTableRow(string& type, string name, int id, bool status);

public:

    HTMLGenerator();
    virtual ~HTMLGenerator();

    string generateMainPage();
    string generateSubPage(string& name, bool history);
    string handleStatusChange(int id);
};


#endif //ZENTRALE_HTMLGENERATOR_H
