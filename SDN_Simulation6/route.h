/*
 * route.h
 *
 *  Created on: 2019年2月27日
 *      Author: qinhongbo
 */


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <string>

using namespace omnetpp;
using namespace std;

class Route : public cObject
{
    public:
        // nex[now][des]: stores the next hop if the current state is 'now'
        // and the destination is 'des'
        int nex[20][20];
        Route *dup() const override  {return new Route(*this);}
};


