/*
 * switch.cc
 *
 *  Created on: 2019年2月1日
 *      Author: qinhongbo
 */


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <string>
#include "sdn_message_m.h"

using namespace omnetpp;
using namespace std;

class sdn_switch : public cSimpleModule
{
    private:
      simsignal_t arrivalSignal;

    protected:
      virtual sdn_message *generateMessage();
      virtual void forwardMessage(sdn_message *msg);
      virtual void initialize() override;
      virtual void handleMessage(cMessage *msg) override;
};

Define_Module(sdn_switch);

bool cmp(string a, string b) {
    return a == b;
}

void sdn_switch::initialize()
{
    arrivalSignal = registerSignal("arrival");
    int id = getIndex();
    int numberOfGate = gateSize("gate");
    EV << "This is node of " << id << " and it has " << numberOfGate << " gates with the name of " << getName() << "\n";

    // Module 0 sends the first message
    if (getIndex() == 0 && cmp(getName(), "switches")) {
        // Boot the process scheduling the initial message as a self-message.
        sdn_message *msg = generateMessage();
        scheduleAt(0.0, msg);
    }
}

void sdn_switch::handleMessage(cMessage *msg)
{
    sdn_message *ttmsg = check_and_cast<sdn_message *>(msg);

    if (ttmsg->getDestination() == getIndex()) {
        // Message arrived
        int hopcount = ttmsg->getHopCount();
        // send a signal
        emit(arrivalSignal, hopcount);

        EV << "Message " << ttmsg << " arrived after " << hopcount << " hops.\n";
        bubble("ARRIVED, starting new one!");

        delete ttmsg;

        // Generate another one.
        EV << "Generating another message: ";
        sdn_message *newmsg = generateMessage();
        EV << newmsg << endl;
        forwardMessage(newmsg);
    }
    else {
        // We need to forward the message.
        forwardMessage(ttmsg);
    }
}

sdn_message *sdn_switch::generateMessage()
{
    // Produce source and destination addresses.
    int src = getIndex();
    int n = getVectorSize();
    int dest = intuniform(0, n-2);
    if (dest >= src)
        dest++;

    char msgname[20];
    sprintf(msgname, "tic-%d-to-%d", src, dest);

    // Create message object and set source and destination field.
    sdn_message *msg = new sdn_message(msgname);
    msg->setSource(src);
    msg->setDestination(dest);
    return msg;
}

void sdn_switch::forwardMessage(sdn_message *msg)
{
    // Increment hop count.
    msg->setHopCount(msg->getHopCount()+1);

    // Same routing as before: random gate.
    int n = gateSize("gate");
    int k = intuniform(0, n-1);

    EV << "Forwarding message " << msg << " on gate[" << k << "]\n";
    send(msg, "gate$o", k);
}

