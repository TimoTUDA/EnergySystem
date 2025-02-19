//
// Created by Philipp on 7.12.22.
//

#include "../../header/UDPSender/KomponentenUdpSender.h"


/**
 *
 */
KomponentenUdpSender::KomponentenUdpSender() = default;



/**
 *
 */
KomponentenUdpSender::KomponentenUdpSender(int port) : port(port) {}


/**
 *
 */
void KomponentenUdpSender::send(Komponente *komponente, string msg) {

    // Forward the message in the correct format
    UDPSender::send(komponente->getIp(), this->port, msg);
}

std::thread KomponentenUdpSender::komponentenThreadSend(Komponente *k, string msg) {
    return std::thread([this, k, msg] {send(k, msg);});
}