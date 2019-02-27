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
#include <vector>
#include <condition.h>
#include "switch_message_m.h"

using namespace omnetpp;

class super_controller : public cSimpleModule
{
    public:
        double loss[20][20], transmissionDelay[20][20], queuingDelay[20][20];
        double availableBandwidth[20][20], totalBandwidth[20][20];
        int src, des, nex[20][20], get;
        bool G[20][20];


        // RL algorithm
        int visit[20][20];
        double Q[20][20][20], epsilon = 0.1;
        double beta1 = 1, beta2 = 1, beta3 = 1, theta1 = 1, phi1 = 1;
        double theta2 = 0.5, phi2 = 0.5;
        double alpha = 0.9, gamma = 0.7;

    protected:
      virtual void forwardMessageToDomain();
      virtual void updateRouteOfDomain();
      virtual void initialize() override;
      virtual void handleMessage(cMessage *msg) override;
      virtual void retrieveCondition(cMessage* msg);
      virtual int makeSoftmaxPolicy(int state, double (& q)[20]);
      virtual double getReward(int state, int nextState);
      virtual void sarsa(double (& q)[20][20]);
};

Define_Module(super_controller);

struct Node {
    int idx;
    long double val;
    Node(int a = 0, long double b = 0) {
        idx = a;
        val = b;
    }
    bool cmp(Node a, Node b) {
        return a.val > b.val;
    }
};

int super_controller::makeSoftmaxPolicy(int state, double (& q)[20]) {
    /*
     *  make epsilon greedy policy
     *
     *  Args:
     *      current statement
     *      map of "action -> reward"
     *
     *  Returns:
     *      the next hop
     */

    vector<Node> vec; // record the id of the surrounding nodes
    for (int i = 0; i < 20; ++i) if (G[state][i])  vec.push_back(Node(i, q[i]));
    int len = vec.size();

    long double tauN = getTau();
    long double sum = 0;
    for (int i = 0; i < len; ++i) {
        vec[i].val = exp(vec[i].val / tauN);
        sum += vec[i].val;
    }

    for (int i = 0; i < len; ++i)
        vec[i].val /= sum;

    sort(vec.begin(), vec.end());
    long double chance = uniform(0, 1);
    for (int i = 0; i < len; ++i) {
        if (chance <= vec[i].val) return vec[i].idx;
        else chance -= vec[i].val;
    }
}


double super_controller::getReward(int state, int nextState) {
    double totalQueuingDelay = 0;
    double totalTransmissionDelay = 0;
    double totalAvailableBandwidth = 0;
    int n = 0; // number of the surrounding nodes
    for (int i = 0; i < 20; ++i) if (G[state][i]) {
        ++n;
        totalQueuingDelay += queuingDelay[state][i];
        totalTransmissionDelay += transmissionDelay[state][i];
        totalAvailableBandwidth += availableBandwidth[state][i];
    }

    // cost: delay
    double delay = 2 * atan(transmissionDelay[state][nextState] - totalTransmissionDelay / n) / pi;

    // cost: queue
    double queue = 2 * atan(queuingDelay[state][nextState] - totalQueuingDelay / n) / pi;

    // cost: loss
    double loss = 1 - 2 * loss[state][nextState];

    // cost: B1
    double B1 = 2 * availableBandwidth[state][nextState] / totalBandwidth[state][nextState];

    // cost: B2
    double B2 = 2 * atan(0.01 * (availableBandwidth[state][nextState] - totalAvailableBandwidth / n)) / pi;


    return -3 + beta1 * (theta1 * delay + theta2 * queue) + beta2 * loss + beta3 * (phi1 * B1 + phi2 * B2);

}

void super_controller::sarsa(double (& q)[20][20]) {
    ++visit[src][des];

    if (visit[src][des] == 1) {
        // initialization
        for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 20; ++j) {
                q[i][j] = 0;
            }
        }
    }

    if (visit[src][des] == 101) visit[src][des] = 1;

    int state = src, action, nextState, nextAction;
    action = makeSoftmaxPolicy(nowState, q[nowState]);
    double reward;
    while (true) {
        nex[src][des][state] = action;
        nextState = action;
        nextAction = makeSoftmaxPolicy(nextState, q[nextState]);

        reward = getReward(state, action);
        q[state][action] += alpha * (reward + gamma * q[nextState][nextAction] - q[state][action]);

        if (nextState = des) break;
        action = nextAction;
        state = NextState;
    }
}

void super_controller::initialize()
{
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            loss[i][j] = transmissionDelay[i][j] = -1;
            queuingDelay[i][j] = availableBandwidth[i][j] = -1;
            totalBandwidth[i][j] = nex[i][j] = -1;
            visit[i][j] = 0;
            G[i][j] = 0;
        }
    }
    get = 0;
}

void super_controller::updateRouteOfDomain() {
    cMessage* msg = new cMessage("update");
    Route* route = new Route();
    for (int i = 0; i < 20; ++i)
        for (int j = 0; j < 20; ++j)
            route->nex[i][j] = nex[i][j];

    msg->addObject(route);
    send(msg->dup(), "domain$o", 0);
    send(msg->dup(), "domain$o", 1);
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
            sarsa(Q[des]);

            // send route plan to domain controller
            updateRouteOfDomain();
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
    cMessage* msg = new cMessage("request");
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
                G[i][j] = cond->G[i][j];
            }
        }
}

