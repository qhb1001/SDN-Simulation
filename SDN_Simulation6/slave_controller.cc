/*
 * slave_controller.cc
 *
 *  Created on: 2019年2月4日
 *      Author: qinhongbo
 */


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <condition.h>
#include "switch_message_m.h"

using namespace omnetpp;
using namespace std;

class slave_controller : public cSimpleModule
{
    private:
        double loss[20][20], transmissionDelay[20][20], queuingDelay[20][20];
        double availableBandwidth[20][20], totalBandwidth[20][20];
        int nex[20][20][20];
        bool G[20][20];

    protected:
      virtual void initialize() override;
      virtual void handleMessage(cMessage *msg) override;
      virtual void copyCondition(condition* cond);
};

Define_Module(slave_controller);

void slave_controller::initialize()
{
    memset(G, 0, sizeof(G));
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            loss[i][j] = transmissionDelay[i][j] = -1;
            queuingDelay[i][j] = availableBandwidth[i][j] = -1;
            totalBandwidth[i][j] = -1;
        }
    }
}

// this part will be updated later
void slave_controller::handleMessage(cMessage *msg)
{
    string from = msg->getSenderModule()->getName();

    if (from == "switches") {
        switch_message* tempmsg = check_and_cast<switch_message *>(msg);
        int idx = tempmsg->getSource();
        condition* cond = (condition*) tempmsg->getObject("");

        if (idx == 0 && getIndex() == 0) printf("%d %d cond->transmissionDelay[0][1]: %f\n", idx, getIndex(), cond->transmissionDelay[0][1]);

        for (int i = 0; i < 20; ++i) {
            if (loss[idx][i] <= 0) loss[idx][i] = cond->loss[idx][i];
            if (transmissionDelay[idx][i] <= 0) transmissionDelay[idx][i] = cond->transmissionDelay[idx][i];
            if (queuingDelay[idx][i] <= 0) queuingDelay[idx][i] = cond->queuingDelay[idx][i];
            if (availableBandwidth[idx][i] <= 0) availableBandwidth[idx][i] = cond->availableBandwidth[idx][i];
            if (totalBandwidth[idx][i] <= 0) totalBandwidth[idx][i] = cond->totalBandwidth[idx][i];
            if (G[idx][i] == 0) G[idx][i] = cond->G[idx][i];
        }

        if (idx == 0 && getIndex() == 0) printf("%d %d transmissionDelay[0][1]: %f\n", idx, getIndex(), transmissionDelay[0][1]);


    } else if (from == "domain") {
        // send back the network condition information
        cout << "slave send message to domain\n";

//        if (getIndex() == 0) printf("This is transmissionDelay[0][1]: %f\n", transmissionDelay[0][1]);


        condition* cond = new condition();
        copyCondition(cond);
        cMessage* msg_ = new cMessage();
        msg_->addObject(cond);
        send(msg_->dup(), "domain$o");
    }
}

void slave_controller::copyCondition(condition* cond) {
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
}



