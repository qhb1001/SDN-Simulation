/*
 * super_controller.cc
 *
 *  Created on: 2019年2月4日
 *      Author: qinhongbo
 */


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "sdn_message_m.h"

using namespace omnetpp;

class super_controller : public cSimpleModule
{
    protected:
      virtual sdn_message *generateMessage();
      virtual void forwardMessage(sdn_message *msg);
      virtual void initialize() override;
      virtual void handleMessage(cMessage *msg) override;
};

Define_Module(super_controller);

void super_controller::initialize()
{
}

// this part will be updated later
void super_controller::handleMessage(cMessage *msg)
{

}

sdn_message *super_controller::generateMessage()
{

}

void super_controller::forwardMessage(sdn_message *msg)
{

}

