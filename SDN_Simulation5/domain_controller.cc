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
#include "switch_message_m.h"

using namespace omnetpp;
using namespace std;

class domain_controller : public cSimpleModule
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

Define_Module(domain_controller);

void domain_controller::initialize()
{
}

// this part will be updated later
void domain_controller::handleMessage(cMessage *msg)
{
    string from = msg->getSenderModule()->getName();
    if (from == "switches") {
        switch_message* tempmsg = check_and_cast<switch_message *>(msg);
        // first, receive network condition from slave controller
        // perform RL algorithm or push the message to super controller

    }
}

cMessage *domain_controller::generateMessage()
{

}

void domain_controller::forwardMessage(cMessage *msg)
{

}
