/*
 * slave_controller.cc
 *
 *  Created on: 2019年2月4日
 *      Author: qinhongbo
 */


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "switch_message_m.h"

using namespace omnetpp;

class slave_controller : public cSimpleModule
{
    private:
        double loss[20][20], transmissionDelay[20][20], queuingDelay[20][20];
        double availableBandwidth[20][20], totalBandwidth[20][20];

    protected:
      virtual cMessage *generateMessage();
      virtual void forwardMessage(cMessage *msg);
      virtual void initialize() override;
      virtual void handleMessage(cMessage *msg) override;
};

Define_Module(slave_controller);

void slave_controller::initialize()
{
}

// this part will be updated later
void slave_controller::handleMessage(cMessage *msg)
{
    string from = msg->getSenderModule()->getName();
    if (from == "switches") {
        switch_message* tempmsg = check_and_cast<switch_message *>(msg);
        int idx = tempmsg->getSource();
        for (int i = 0; i < 20; ++i) {
            loss[idx][i] = msg->setLoss(i);
            transmissionDelay[idx][i] = msg->setTransmissionDelay(i);
            queuingDelay[idx][i] = msg->setQueuingDelay(i);
            availableBandwidth[idx][i] = msg->setAvailableBandwidth(i);
            totalBandwidth[idx][i] = msg->setTotalBandwidth(i);
        }
    } else if (from == "domain") {
        // send back the network condition information

    }
}

cMessage *slave_controller::generateMessage()
{

}

void slave_controller::forwardMessage(cMessage *msg)
{

}


