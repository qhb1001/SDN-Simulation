/*
 * super_controller.cc
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

class super_controller : public cSimpleModule
{
    public:
        double loss[20][20], transmissionDelay[20][20], queuingDelay[20][20];
        double availableBandwidth[20][20], totalBandwidth[20][20];
        int src, des, nex[20][20], get;

    protected:
      virtual void forwardMessageToDomain();
      virtual void initialize() override;
      virtual void handleMessage(cMessage *msg) override;
      virtual void retrieveCondition(cMessage* msg);
};

Define_Module(super_controller);

void super_controller::initialize()
{
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            loss[i][j] = transmissionDelay[i][j] = -1;
            queuingDelay[i][j] = availableBandwidth[i][j] = -1;
            totalBandwidth[i][j] = nex[i][j] = -1;
        }
    }
    get = 0;
}

// this part will be updated later
void super_controller::handleMessage(cMessage *msg)
{
    // must come from domain controller
    string name = msg->getName();

    if (name == "retrieve") {
        ++get;
        retrieveCondition(msg);
        if (get == 2) {
            get = 0;
            // perform the algorithm

        }
    } else if (name == "request") {
        // get source and destination information
        switch_message* tempmsg = check_and_cast<switch_message *>(msg);
        src = tempmsg->getSource();
        des = tempmsg->getDestination();

        // retrieve network condition information
        forwardMessageToDomain();
    }

}

void super_controller::forwardMessageToDomain()
{
    cMessage* msg = new cMessage();
    send(msg->dup(), "domain$o", 0);
    send(msg->dup(), "domain$o", 1);
}

void super_controller::retrieveCondition(cMessage* msg) {
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

