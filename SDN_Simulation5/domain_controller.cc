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
    public:
        double loss[20][20], transmissionDelay[20][20], queuingDelay[20][20];
        double availableBandwidth[20][20], totalBandwidth[20][20];
        int get, src, des, nex[20][20];
        bool G[20];

    protected:
      virtual void forwardMessageToSlave();
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
            totalBandwidth[i][j] = nex[i][j] = -1;
        }
        G[i] = 0;
    }
}

// this part will be updated later
void domain_controller::handleMessage(cMessage *msg)
{
    string from = msg->getSenderModule()->getName();
    string name = msg->getName();
    EV << "Receiving from " << name << endl;
    if (name == "initialization") {
        int idx1 = getIndex();
        int idx2 = msg->getSenderModule()->getIndex();

        EV << "This is " << idx1 << " domain and receive info from " << idx2 << endl;
        G[idx2] = 1;
        return ;
    }

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
            if (G[src] && G[des]) {
                // perform RL algorithm or push the message to super controller

            } else {
                // otherwise, send this request to super controller
                switch_message* msg_ = new switch_message("request");
                msg_->setSource(src);
                msg_->setDestination(des);
                send(msg_->dup(), "super");
            }

        }

    } else if (from == "super") {
        cMessage* msg_ = new cMessage("retrieve");
        condition* cond = new condition();
        copyCondition(cond);
        msg_->addObject(cond);
        send(msg_->dup(), "super");
    }
}

void domain_controller::forwardMessageToSlave()
{
    cMessage* msg = new cMessage();
    send(msg->dup(), "slave$o", 0);
    send(msg->dup(), "slave$o", 1);
}


void domain_controller::copyCondition(condition* cond)
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

void domain_controller::retrieveCondition(cMessage* msg) {
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
