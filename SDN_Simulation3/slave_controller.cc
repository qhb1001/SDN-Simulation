/*
 * slave_controller.cc
 *
 *  Created on: 2019年2月4日
 *      Author: qinhongbo
 */


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "sdn_message_m.h"

using namespace omnetpp;

class slave_controller : public cSimpleModule
{
    protected:
      virtual sdn_message *generateMessage();
      virtual void forwardMessage(sdn_message *msg);
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

}

sdn_message *slave_controller::generateMessage()
{

}

void slave_controller::forwardMessage(sdn_message *msg)
{

}


