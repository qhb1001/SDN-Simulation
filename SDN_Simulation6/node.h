/*
 * node.h
 *
 *  Created on: 2019年3月1日
 *      Author: qinhongbo
 */


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <string>
#include <math.h>
#include <vector>
#include <algorithm>
#include "switch_message_m.h"

struct Node {
    int idx;
    long double val;
    Node(int a = 0, long double b = 0) {
        idx = a;
        val = b;
    }
    bool operator <  (const Node b) const {
        return val > b.val;
    }
};

