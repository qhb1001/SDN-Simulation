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
#include <math.h>
#include <vector>
#include <algorithm>
#include <route.h>
#include "switch_message_m.h"
#include <node.h>

using namespace omnetpp;
using namespace std;

class domain_controller : public cSimpleModule
{
    public:
        double loss[20][20], transmissionDelay[20][20], queuingDelay[20][20];
        double availableBandwidth[20][20], totalBandwidth[20][20];
        int get; // the number of slave controller that has send network condition
        int src, des;
        int nex[20][20]; // forward table
        int num; // the number of the switches under its control
        bool isIn[20]; // if this switch is under domain controller's control
        bool G[20][20]; // connectivity condition
        bool turnIn;

        // RL algorithm
        int visit[20][20];
        double Q[20][20][20], epsilon = 0.1, tau0 = 100, tauT = 0.05, T = 100;
        double beta1 = 1, beta2 = 1, beta3 = 1, theta1 = 1, phi1 = 1;
        double theta2 = 0.5, phi2 = 0.5;
        double alpha = 0.9, gamma = 0.7;
        double pi = acos(-1.0);

    protected:
      virtual void getIsIn();
      virtual long double getTau();
      virtual double getReward(int state, int nextState);
      virtual void forwardMessageToSlave(cMessage* msg);
      virtual void forwardMessageToSwitch();
      virtual void initialize() override;
      virtual void handleMessage(cMessage *msg) override;
      virtual void copyCondition(condition* cond);
      virtual void retrieveCondition(cMessage* msg);
      virtual void forwardMessage(int to, int nextHop, string name);
      virtual int makeSoftmaxPolicy(int state, double (& q)[20]);
      virtual void sarsa(double (& q)[20][20]);
};

Define_Module(domain_controller);


long double domain_controller::getTau() {
    return -((tau0 - tauT) * visit[src][des]) / T + tau0;
}

namespace temp{
    bool cmp(Node* a, Node* b) {
        return a->val > b->val;
    }
}

//int turn = 0;
int domain_controller::makeSoftmaxPolicy(int state, double (& q)[20]) {
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
//    printf("************Round: %d **************\n", turn++);
    cout << "in the softmax policy\n";
    vector<Node*> vec; // record the id of the surrounding nodes

    for (int i = 0; i < 20; ++i) if (G[state][i] && isIn[i])   vec.push_back(new Node(i, q[i]));


    int len = vec.size();

    double tauN = getTau();
    double sum = 0;
    for (int i = 0; i < len; ++i) {
//        printf("This is val: %f, tauN: %f\n", vec[i]->val, tauN);
        vec[i]->val = exp(vec[i]->val / tauN);
        sum += exp(vec[i]->val / tauN);
    }

    for (int i = 0; i < len; ++i)
        vec[i]->val /= sum;


    sort(vec.begin(), vec.end(), temp::cmp);


    double chance = uniform(0, 1);
    for (int i = 0; i < len; ++i) {

        if (chance <= vec[i]->val) {

            return vec[i]->idx;
        }
        else chance -= vec[i]->val;
    }

    return vec[len - 1]->idx;
}


double domain_controller::getReward(int state, int nextState) {
    double totalQueuingDelay = 0;
    double totalTransmissionDelay = 0;
    double totalAvailableBandwidth = 0;
    int n = 0; // number of the surrounding nodes
    for (int i = 0; i < 20; ++i) if (isIn[i] && G[state][i]) {
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
    double los = 1 - 2 * loss[state][nextState];

    // cost: B1
    double B1 = 2 * availableBandwidth[state][nextState] / totalBandwidth[state][nextState];

    // cost: B2
    double B2 = 2 * atan(0.01 * (availableBandwidth[state][nextState] - totalAvailableBandwidth / n)) / pi;


    return -3 + beta1 * (theta1 * delay + theta2 * queue) + beta2 * los + beta3 * (phi1 * B1 + phi2 * B2);

}

void domain_controller::sarsa(double (& q)[20][20]) {
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
    action = makeSoftmaxPolicy(state, q[state]);
    double reward;

    int x = 0;
    while (true) {

        nex[state][des] = action;
        nextState = action;
        nextAction = makeSoftmaxPolicy(nextState, q[nextState]);

        reward = getReward(state, action);
        q[state][action] += alpha * (reward + gamma * q[nextState][nextAction] - q[state][action]);

        if (nextState == des)   break;

        action = nextAction;
        state = nextState;
    }
}

void domain_controller::getIsIn() {
    memset(isIn, 0, sizeof(isIn));
    num = 0;

    bool flag;
    for (int i = 0; i < 20; ++i) {
        flag = false;
        for (int j = 0; j < 20; ++j) if (G[i][j]) {
            flag = true;
            break;
        }
        isIn[i] = flag;
        num += flag;
    }
}

void domain_controller::initialize()
{
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            loss[i][j] = transmissionDelay[i][j] = -1;
            queuingDelay[i][j] = availableBandwidth[i][j] = -1;
            totalBandwidth[i][j] = nex[i][j] = -1;
            visit[i][j] = 0;
        }
        isIn[i] = 0;
    }
    get = turnIn = 0;
}

void  domain_controller::forwardMessage(int to, int nextHop, string name)
{
    for (cModule::GateIterator i(this); !i.end(); i++)
    {
         cGate *gate = i();
         std::string gateStr = gate->getName();
         if (gateStr == "gate$o" && gate->getPathEndGate()->getOwnerModule()->getIndex() == to)
         {
             int senderId = gate->getIndex();
             cMessage* msg = new cMessage(name.c_str()); // update the forward table
             msg->addPar("hop").setLongValue(nextHop);
             msg->addPar("des").setLongValue(des);
             send(msg->dup(), "gate$o", senderId);
         }
    }
}

void domain_controller::forwardMessageToSwitch() {
    int now = src;
    while (now != des) {
        if (isIn[now])  {
//            printf("domain controller %d update nodes %d\n", getIndex(), now);
            forwardMessage(now, nex[now][des], "update");
//            printf("done\n");
        }
        now = nex[now][des];
    }

    // inform the source node to send the message according to the route plan
    if (isIn[src])
        forwardMessage(src, 0, "send");

}

// this part will be updated later
void domain_controller::handleMessage(cMessage *msg)
{
    string from = msg->getSenderModule()->getName();
    string name = msg->getName();

    if (from == "switches") {
//        cout << "domain controller send message to slave\n";
        turnIn = true;
        forwardMessageToSlave(msg);

    } else if (from == "slave") {
        get++;

        printf("This is domain %d, get: %d\n", getIndex(), get);
        retrieveCondition(msg);
        if (get == 2) { // all network information received
            get = 0;
            if (isIn[src] && isIn[des]) {
                cout << "domain controller running SARSA. " << src << ' ' << des << endl;

                // perform RL algorithm or push the message to super controller
                sarsa(Q[des]);

                int l = src;
                EV << "Here is the path: ";
                while (l != des) {EV << l << ' '; l = nex[l][des];}
                EV << des << endl;

                //send route plan to switch
                forwardMessageToSwitch();
            } else {
                // otherwise, send this request to super controller

//               printf("This is transmissionDelay[0][1]: %f\n", transmissionDelay[0][1]);

                if (turnIn) {
                    turnIn = false;
                    switch_message* msg_ = new switch_message("turn in");
                    msg_->setSource(src);
                    msg_->setDestination(des);

                    send(msg_->dup(), "super$o");
                } else {
                    cMessage* mmsg = new cMessage("retrieve");
                    condition* cond = new condition();
                    copyCondition(cond);
                    mmsg->addObject(cond);
                    send(mmsg, "super$o");
                }
            }

        }

    } else if (from == "super") {

        if (name == "request") {

            forwardMessageToSlave(msg);

        } else if (name == "update") {

            Route* route = (Route*) msg->getObject("");

            des = msg->par("des").longValue();
            src = msg->par("src").longValue();
            for (int i = 0; i < 20; ++i)
                nex[i][des] = route->nex[i][des];

            //send route plan to switch
            forwardMessageToSwitch();
        }

    }
}

void domain_controller::forwardMessageToSlave(cMessage* msg)
{
    switch_message* tempmsg = check_and_cast<switch_message *>(msg);
    // retrieve network condition from slave controller
    src = tempmsg->getSource();
    des = tempmsg->getDestination();

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
                cond->G[i][j] = G[i][j];
            }
        }
}

void domain_controller::retrieveCondition(cMessage* msg) {
    condition* cond = (condition*) msg->getObject("");
    for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 20; ++j) {
                if (loss[i][j] == -1) loss[i][j] = cond->loss[i][j];
                if (transmissionDelay[i][j] == -1) transmissionDelay[i][j] = cond->transmissionDelay[i][j];
                if (queuingDelay[i][j] == -1) queuingDelay[i][j] = cond->queuingDelay[i][j];
                if (availableBandwidth[i][j] == -1) availableBandwidth[i][j] = cond->availableBandwidth[i][j];
                if (totalBandwidth[i][j] == -1) totalBandwidth[i][j] = cond->totalBandwidth[i][j];
                if (G[i][j] == 0) G[i][j] = cond->G[i][j];
            }
        }

    getIsIn();
}
