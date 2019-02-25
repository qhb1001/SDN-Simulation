/*
 * iint.cc
 *
 *  Created on: 2019年2月23日
 *      Author: qinhongbo
 */

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <string>

using namespace omnetpp;
using namespace std;

class condition : public cObject
{

    public:
        double loss[20][20], transmissionDelay[20][20], queuingDelay[20][20];
        double availableBandwidth[20][20], totalBandwidth[20][20];
        bool G[20];
        condition *dup() const override  {return new condition(*this);}
};


