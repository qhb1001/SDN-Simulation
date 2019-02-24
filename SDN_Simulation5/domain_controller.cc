/*
 * domain_controller.cc
 *
 *  Created on: 2019年2月4日
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

class domain_controller : public cSimpleModule
{
    private:
        double loss[20][20], transmissionDelay[20][20], queuingDelay[20][20];
        double availableBandwidth[20][20], totalBandwidth[20][20];
        int get, src, des;
    protected:
      virtual cMessage *generateMessage4Super();
      virtual void forwardMessageToSlave();
      virtual void forwardMessageToSuper(cMessage *msg);
      virtual void initialize() override;
      virtual void handleMessage(cMessage *msg) override;
      virtual void copyCondition(condition* cond);
      virtual void retrieveCondition(cMessage* msg);
};

Define_Module(domain_controller);

void domain_controller::initialize()
{
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            loss[i][j] = transmissionDelay[i][j] = -1;
            queuingDelay[i][j] = availableBandwidth[i][j] = -1;
            totalBandwidth[i][j] = -1;
        }
    }
}

// this part will be updated later
void domain_controller::handleMessage(cMessage *msg)
{
    string from = msg->getSenderModule()->getName();
    if (from == "switches") {
        switch_message* tempmsg = check_and_cast<switch_message *>(msg);
        // retrieve network condition from slave controller
        src = tempmsg->getSource();
        des = tempmsg->getDestination();
        forwardMessageToSlave();

    } else if (from == "slave") {
        get++;
        retrieveCondition(msg);
        if (get == 2) { // all network information received
            get = 0;
            // perform RL algorithm or push the message to super controller

        }

    }
}

void domain_controller::forwardMessageToSlave()
{
    cMessage* msg = new cMessage();
    send(msg->dup(), "slave$o", 0);
    send(msg->dup(), "slave$o", 1);
}

void domain_controller::forwardMessageToSuper(cMessage *msg)
{

}

void copyCondition(condition* cond)
{
    for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 20; ++j) {
                cond->loss[i][j] = loss[i][j];
                cond->transmissionDelay[i][j] = transmissionDelay[i][j];
                cond->queuingDelay[i][j] = queuingDelay[i][j];
                cond->availableBandwidth[i][j] = availableBandwidth[i][j];
                cond->totalBandwidth[i][j] = totalBandwidth[i][j];
            }
        }
}

void retrieveCondition(cMessage* msg) {
    condition* cond = (condition*) msg->getObject("");
    for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 20; ++j) {
                loss[i][j] = cond->loss[i][j];
                transmissionDelay[i][j] = cond->transmissionDelay[i][j];
                queuingDelay[i][j] = cond->queuingDelay[i][j];
                availableBandwidth[i][j] = cond->availableBandwidth[i][j];
                totalBandwidth[i][j] = cond->totalBandwidth[i][j];
            }
        }
}
