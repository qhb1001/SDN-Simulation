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
      simtime_t timeout;  // timeout
      cMessage *timeoutEvent;  // holds pointer to the timeout self-message
      sdn_message *msg_;
      double loss[200], transmissionDelay[200], queuingDelay[200];
      double availableBandwidth[200], totalBandwidth[200];

    protected:
      virtual sdn_message *generateMessage(char *a);
      virtual void forwardMessage(sdn_message *msg, int to);
      virtual void forwardMessageToDomain(sdn_message *msg);
      virtual void initialize() override;
      virtual void handleMessage(cMessage *msg) override;
      virtual void sendACK(sdn_message *msg);
};

Define_Module(sdn_switch);

bool cmp(string a, string b) {
    EV << a << " " << b << endl;
    return a == b;
}


void sdn_switch::initialize()
{
    timeout = 1.0;
    timeoutEvent = new cMessage("timeoutEvent");

    arrivalSignal = registerSignal("arrival");
    int id = getIndex();
    int numberOfGate = gateSize("gate");
    int n = getVectorSize();
//    EV << numberOfGate << endl;

    for (int i = 0; i <= numberOfGate; ++i) {
            loss[i] = par("loss");
            transmissionDelay[i] = par("transmissionDelay");
            queuingDelay[i] = par("queuingDelay");
            availableBandwidth[i] = par("availableBandwidth");
            totalBandwidth[i] = par("totalBandwidth");
            while (totalBandwidth[i] < availableBandwidth[i])   totalBandwidth[i] = par("totalBandwidth");
        }


//    EV << "This is node of " << id << " and it has " << numberOfGate << " gates with the name of " << getName() << "\n";
//    EV << "Vector size for node " << id << " is " << n << endl;
    EV << "This is node " << id << " and its loss[0] is " << loss[0] << ", transmissionDelay[0]:" << transmissionDelay[0] << ", queuingDelay[0]:" << queuingDelay[0] << endl;
    EV << "And its availableBandwidth[0]:" << availableBandwidth[0] << ", totalBandwidth[0]:" << totalBandwidth[0] << endl;

    // Module 0 sends the first message
    if (getIndex() == 0 && cmp(getName(), "switches")) {
        // Boot the process scheduling the initial message as a self-message.
        cout << "About to send the mssage\n";
        msg_ = generateMessage("msg");
        scheduleAt(0.0, msg_);
    }
}

void sdn_switch::sendACK(sdn_message *msg) {
    sdn_message* ack = generateMessage("ack");
    int to = msg->getSource();
    //EV << "This is node " << getIndex() << " with " << gateSize("gate") << "nodes and des " << to << endl;
    cGate * sender = msg->getSenderGate();
    for (cModule::GateIterator i(this); !i.end(); i++)
    {
         cGate *gate = i();
         std::string gateStr = gate->getName();
         if (gateStr == "gate$o" && gate->getPathEndGate()->getOwnerModule() == sender->getOwnerModule() )
         {
             int senderId = gate->getIndex();
             send(ack, "gate$o", senderId);
         }
    }
}

void sdn_switch::handleMessage(cMessage *msg)
{
    EV << "This is " << msg->getName() << " message. ";

    if (cmp(msg->getName(), "msg")) {
        sdn_message* tempmsg = check_and_cast<sdn_message *>(msg);
        if (tempmsg->getSource() != getIndex()) sendACK(tempmsg);
//        EV << "just send ack to " << tempmsg->getSource() << " " << tempmsg->getDestination() << " " << getIndex() << endl;

        if (tempmsg->getDestination() == getIndex()) {
            int hopcount = tempmsg->getHopCount();
            emit(arrivalSignal, hopcount);

            bubble("ARRIVED, starting new one!");

            //EV << "Generating another message: ";
            msg_ = generateMessage("msg");
            //EV << newmsg << endl;
            forwardMessage(msg_);
            scheduleAt(simTime()+timeout, timeoutEvent);
        } else {
            tempmsg->setSource(getIndex());
            msg_ = tempmsg;
            forwardMessage(tempmsg);
            scheduleAt(simTime()+timeout, timeoutEvent);
        }

    } else if (cmp(msg->getName(), "ack")){
        cancelEvent(timeoutEvent);

    } else if (cmp(msg->getName(), "timeoutEvent")) {
        forwardMessage(msg_);
        scheduleAt(simTime()+timeout, timeoutEvent);

    }
}

sdn_message *sdn_switch::generateMessage(char *a)
{
    // Produce source and destination addresses.
    int src = getIndex();
    int n = getVectorSize();
    int dest = intuniform(0, n-2);
    if (dest >= src)
        dest++;

    // Create message object and set source and destination field.
    sdn_message *msg = new sdn_message(a);
    msg->setSource(src);
    msg->setDestination(dest);

    cout << "About to return the message\n";
    return msg;
}

void sdn_switch::forwardMessage(sdn_message *msg, int to)
{
    // Increment hop count.
    msg->setHopCount(msg->getHopCount()+1);
    msg->setSource(getIndex());

    for (cModule::GateIterator i(this); !i.end(); i++)
    {
         cGate *gate = i();
         std::string gateStr = gate->getName();
         if (gateStr == "gate$o" && gate->getPathEndGate()->getOwnerModule()->getIndex() == to)
         {
             int senderId = gate->getIndex();
             send(msg, "gate$o", senderId);
         }
    }
}

void sdn_switch::forwardMessageToDomain(sdn_message *msg) {


}
