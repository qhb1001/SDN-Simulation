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
      double loss[20][20], transmissionDelay[20][20], queuingDelay[20][20];
      double availableBandwidth[20][20], totalBandwidth[20][20];
      int nex[20]; // nex[des]: the next hop if the destination of the packet is 'des'
      int idx;
      bool G[20][20];

    protected:
      virtual switch_message *generateMessage(char *a);
      virtual void forwardMessageToSwitch(switch_message *msg, int to);
      virtual void forwardMessageToDomain(switch_message *msg);
      virtual void forwardMessageToSlave(switch_message *msg);
      virtual void initialize() override;
      virtual void handleMessage(cMessage *msg) override;
      virtual void recordInformation(switch_message *msg);
      virtual switch_message* initializationMessage();
};

Define_Module(sdn_switch);

switch_message* sdn_switch::initializationMessage() {
    switch_message* msg = new switch_message("ini");

    msg->setSource(getIndex());
    condition* cond = new condition();

    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            cond->loss[i][j] = loss[i][j];
            cond->transmissionDelay[i][j] = transmissionDelay[i][j];
            cond->queuingDelay[i][j] = queuingDelay[i][j];
            cond->availableBandwidth[i][j] = availableBandwidth[i][j];
            cond->totalBandwidth[i][j] = totalBandwidth[i][j];
            cond->G[i][j] = G[i][j];
        }
    }

    msg->addObject(cond);

    return msg;
}

void sdn_switch::initialize()
{
    timeout = 1.0;
    timeoutEvent = new cMessage("timeoutEvent");

    arrivalSignal = registerSignal("arrival");
    idx = getIndex();

    for (int i = 0; i < 20; ++i)
        for (int j = 0; j < 20; ++j) {
        loss[i][j] = transmissionDelay[i][j] = -1;
        queuingDelay[i][j] = availableBandwidth[i][j] = -1;
        totalBandwidth[i][j] = nex[i] = -1;
    }

    memset(G, 0, sizeof(G));
    cout << "node " << idx << ": ";
    for (cModule::GateIterator i(this); !i.end(); i++)
    {
         cGate *gate = i();
         std::string gateStr = gate->getName();
         if (gateStr == "gate$o")
         {
             int to = gate->getPathEndGate()->getOwnerModule()->getIndex();
             G[idx][to] = 1;
             cout << to << ' ';
             loss[idx][to] = par("loss");
             transmissionDelay[idx][to] = par("transmissionDelay");
             queuingDelay[idx][to] = par("queuingDelay");
             availableBandwidth[idx][to] = par("availableBandwidth");
             totalBandwidth[idx][to] = par("totalBandwidth");
             while (totalBandwidth[idx][to] < availableBandwidth[idx][to])   totalBandwidth[idx][to] = par("totalBandwidth");
         }
    }
    cout << endl;

    scheduleAt(0.0, initializationMessage()->dup());

    string name =  getName();
    // Module 0 sends the first message
    if (getIndex() == 0 && name == "switches") {
        // Boot the process scheduling the initial message as a self-message.
        cout << "About to send the mssage\n";
        msg_ = generateMessage("msg");
        scheduleAt(1.0, msg_);
    }
}


void sdn_switch::handleMessage(cMessage *msg)
{
    string from =  msg->getSenderModule()->getName();
    string name = msg->getName();


    if (name == "ini") {
        send(msg->dup(), "slave");
        return ;
    }


    if (from == "switches") {

        if (name == "msg") {
            switch_message* tempmsg = check_and_cast<switch_message *>(msg);
            tempmsg->setSource(getIndex());
            recordInformation(tempmsg);
            forwardMessageToSlave(tempmsg);

            if (tempmsg->getDestination() == getIndex()) {
                int hopcount = tempmsg->getHopCount();
                emit(arrivalSignal, hopcount);

                bubble("ARRIVED, starting new one!");

                msg_ = generateMessage("msg");

                int des = msg_->getDestination();
                if (nex[des] != -1) forwardMessageToSwitch(msg_->dup(), nex[des]);
                else forwardMessageToDomain(msg_);

                scheduleAt(simTime()+timeout, timeoutEvent);

            } else {
//                tempmsg->setSource(getIndex());
                int des = tempmsg->getDestination();
                if (nex[des] != -1) forwardMessageToSwitch(tempmsg->dup(), nex[des]);
                else {
//                    cout << "No record, sent to domain controller.\n";
                    forwardMessageToDomain(msg_);
                }
            }

        }
    } else if (from == "domain") {
        int des = (int) msg->par("des").longValue();
        int hop = (int) msg->par("hop").longValue();

        if (name == "update") {
            nex[des] = hop;
        } else if (name == "send") {
            EV << "Get the message from domain. \n";
            forwardMessageToSwitch(msg_, nex[des]);
        }

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
    msg->setDestination(dest);

    return msg;
}


void sdn_switch::forwardMessageToSwitch(switch_message *msg, int to)
{
    // Increment hop count.
    msg->setHopCount(msg->getHopCount()+1);

    for (cModule::GateIterator i(this); !i.end(); i++)
    {
         cGate *gate = i();
         std::string gateStr = gate->getName();
         if (gateStr == "gate$o" && gate->getPathEndGate()->getOwnerModule()->getIndex() == to)
         {
             int senderId = gate->getIndex();
             send(msg->dup(), "gate$o", senderId);
         }
    }
}

void sdn_switch::forwardMessageToDomain(switch_message *msg) {
    switch_message* copy =  msg->dup();
    send(copy, "domain");
}

void sdn_switch::forwardMessageToSlave(switch_message *msg) {
    if (getIndex() == 0) printf("transmissionDelay[0][1]: %f\n", ((condition*)msg->getObject(""))->transmissionDelay[0][1]);

    switch_message* copy =  msg->dup();
    send(copy, "slave");
}

void sdn_switch::recordInformation(switch_message *msg) {

    condition* cond = new condition();

    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            cond->loss[i][j] = loss[i][j];
            cond->transmissionDelay[i][j] = transmissionDelay[i][j];
            cond->queuingDelay[i][j] = queuingDelay[i][j];
            cond->availableBandwidth[i][j] = availableBandwidth[i][j];
            cond->totalBandwidth[i][j] = totalBandwidth[i][j];
            cond->G[i][j] = G[i][j];
        }
    }

    msg->addObject(cond);
}









