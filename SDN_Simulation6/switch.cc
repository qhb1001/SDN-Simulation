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
#include <condition.h>
#include "switch_message_m.h"

using namespace omnetpp;
using namespace std;

class sdn_switch : public cSimpleModule
{
    private:
      simsignal_t arrivalSignal;
      simtime_t timeout;  // timeout
      cMessage *timeoutEvent;  // holds pointer to the timeout self-message
      switch_message *msg_;
      double loss[20], transmissionDelay[20], queuingDelay[20];
      double availableBandwidth[20], totalBandwidth[20];
      int idx, nex[20];
      bool G[20][20];

    protected:
      virtual switch_message *generateMessage(char *a);
//      virtual void forwardMessage(sdn_message *msg, int to);
      virtual void forwardMessage(switch_message *msg);
      virtual void forwardMessageToDomain(switch_message *msg);
      virtual void forwardMessageToSlave(switch_message *msg);
      virtual void initialize() override;
      virtual void handleMessage(cMessage *msg) override;
      virtual void sendACK(switch_message *msg);
      virtual void recordInformation(switch_message *msg);
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
    idx = getIndex();

    for (int i = 0; i < 20; ++i) {
        loss[i] = transmissionDelay[i] = -1;
        queuingDelay[i] = availableBandwidth[i] = -1;
        totalBandwidth[i] = nex[i] = -1;
    }

    memset(G, 0, sizeof(G));

    for (cModule::GateIterator i(this); !i.end(); i++)
    {
         cGate *gate = i();
         std::string gateStr = gate->getName();
         if (gateStr == "gate$o")
         {
             int to = gate->getPathEndGate()->getOwnerModule()->getIndex();
             G[idx][to] = 1;
             loss[to] = par("loss");
             transmissionDelay[to] = par("transmissionDelay");
             queuingDelay[to] = par("queuingDelay");
             availableBandwidth[to] = par("availableBandwidth");
             totalBandwidth[to] = par("totalBandwidth");
             while (totalBandwidth[to] < availableBandwidth[to])   totalBandwidth[to] = par("totalBandwidth");
         }
    }


    // Module 0 sends the first message
    if (getIndex() == 0 && cmp(getName(), "switches")) {
        // Boot the process scheduling the initial message as a self-message.
        cout << "About to send the mssage\n";
        msg_ = generateMessage("msg");
        scheduleAt(0.0, msg_);
    }
}

void sdn_switch::sendACK(switch_message *msg) {
    switch_message* ack = generateMessage("ack");

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
    string from =  msg->getSenderModule()->getName();

//    condition* cond = new condition();
//    cond->loss[2][1] = 404;
//    msg->addObject(cond);
//    condition* cond_ = (condition* )(msg->getObject(""));
//    EV << "This is the original value: " << cond_->loss[2][1] << endl;

//    EV << "This is one chance: " << uniform(0, 1) << endl;


    if (from == "switches") {
        if (cmp(msg->getName(), "msg")) {
            switch_message* tempmsg = check_and_cast<switch_message *>(msg);
            recordInformation(tempmsg);
            forwardMessageToSlave(tempmsg);
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
    } else {


    }
}

switch_message *sdn_switch::generateMessage(char *a)
{
    // Produce source and destination addresses.
    int src = getIndex();
    int n = getVectorSize();
    int dest = intuniform(0, n-2);
    if (dest >= src)
        dest++;

    // Create message object and set source and destination field.
    switch_message *msg = new switch_message(a);
    msg->setSource(src);
    msg->setDestination(dest);

//    cout << "About to return the message\n";
    return msg;
}


void sdn_switch::forwardMessage(switch_message *msg)
{
    // Increment hop count.
    msg->setHopCount(msg->getHopCount()+1);
    msg->setSource(getIndex());

    // Same routing as before: random gate.
    int n = gateSize("gate");
    int k = intuniform(0, n-1);

    EV << "Forwarding message " << msg << " on gate[" << k << "]\n";
    send(msg->dup(), "gate$o", k);
}

//void sdn_switch::forwardMessage(sdn_message *msg, int to)
//{
//    // Increment hop count.
//    msg->setHopCount(msg->getHopCount()+1);
//    msg->setSource(getIndex());
//
//    for (cModule::GateIterator i(this); !i.end(); i++)
//    {
//         cGate *gate = i();
//         std::string gateStr = gate->getName();
//         if (gateStr == "gate$o" && gate->getPathEndGate()->getOwnerModule()->getIndex() == to)
//         {
//             int senderId = gate->getIndex();
//             send(msg, "gate$o", senderId);
//         }
//    }
//}

void sdn_switch::forwardMessageToDomain(switch_message *msg) {
    switch_message* copy =  msg->dup();
    send(copy, "domain");
}

void sdn_switch::forwardMessageToSlave(switch_message *msg) {
    switch_message* copy =  msg->dup();
    send(copy, "slave");
}

void sdn_switch::recordInformation(switch_message *msg) {

    condition* cond = new condition();

    for (int i = 0; i < 20; ++i) {
        msg->setLoss(i, loss[i]);
        msg->setTransmissionDelay(i, transmissionDelay[i]);
        msg->setQueuingDelay(i, queuingDelay[i]);
        msg->setAvailableBandwidth(i, availableBandwidth[i]);
        msg->setTotalBandwidth(i, totalBandwidth[i]);
        cond->G[idx][i] = G[idx][i];
    }

    msg->addObject(cond);
}









