# Experiment 6 Implement RL Algorithm

## 1. Goal

Implement the Qos-Aware adapative routing algorithm mentioned in [paper](https://ieeexplore.ieee.org/document/7557432). 

## 2. Detail

### 2.1 define some useful variables

```c++
int src, des;
int nex[20][20]; // forward table
int num; // the number of the switches under its control
bool isIn[20]; // if this switch is under domain controller's control
bool G[20][20]; // connectivity condition

// RL algorithm
bool visit[20][20];
double Q[20][20][20][20], epsilon = 0.1;
double beta1 = 1, beta2 = 1, beta3 = 1, theta1 = 1, phi1 = 1;
double theta2 = 0.5, phi2 = 0.5;
double alpha = 0.9, gamma = 0.7;
```

There are two new variables `isIn[]` and `G[][]`. Since domain controller and super controller both need to run the algorithm, they all need these two variables. As for the initialization of these viariables, I put them together with other network condtion variables like `loss[][]`. 

### 2.2 modify modules in previous experiment

#### switch

delete the `initliazation` message and merge the connectivity condition into `recordCondition()` function which will be sent to salve controller. 

#### slave controller

merge them into the `copyCondition()` function .

Fix one bug: after receiving message from domain controller, we should generate a new message rather than just send its copy back. 

#### domain controller

modify `retrieveInformation` function to update the `G[][]` variable and implement one little function called `getIsIn()` to get the variable `isIn[]` by `G[][]`.

### 2.3 implement softmax in C++

The original formula is listed below:
$$
\tau_n = -\frac{(\tau_0 - \tau_T)n}{T} + \tau_0, \ \ \ n\le T
$$
And it's kind of tricky to implement 

```c++
int domain_controller::makeSoftmaxPolicy(int state, double (& q)[20]) {
    /*
     *  make softmax policy
     *
     *  Args:
     *      current statement
     *      map of "action -> reward"
     *
     *  Returns:
     *      the next hop
     */

    vector<Node> vec; // record the information of the surrounding nodes
    for (int i = 0; i < 20; ++i) if (isIn[i] && G[state][i])  vec.push_back(Node(i, q[i]));
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
```

### 2.3 send the route plan to junior nodes

```c++
void  domain_controller::forwardMessage(int to, int nextHop)
{
    for (cModule::GateIterator i(this); !i.end(); i++)
    {
         cGate *gate = i();
         std::string gateStr = gate->getName();
         if (gateStr == "gate$o" && gate->getPathEndGate()->getOwnerModule()->getIndex() == to)
         {
             int senderId = gate->getIndex();
             cMessage* msg = new cMessage("update"); // update the forward table
             msg->addPar("hop")->setLongValue(nextHop);
			 msg->addPar("des")->setLongValue(des);
             
             send(msg, "gate$o", senderId);
         }
    }
}

void domain_controller::forwardMessageToSwtich() {
    // update the forward table of the nodes on this route
    int now = src;
    while (now != des) {
        forwardMessage(now, nex[now][des]);
        now = nex[now][des];
    }
}
```

### 2.4 send packets according to the route plan

for domain controller:

```c++
void domain_controller::forwardMessageToSwtich() {
    int now = src;
    while (now != des) {
        forwardMessage(now, nex[now][des]);
        now = nex[now][des];
    }

    // inform the source node to send the message according to the route plan
    cMessage* msg = new cMessage("send");
    forwaedMessage(src, 0);
}
```

for switch: 

```c++
void sdn_switch::handleMessage(cMessage* msg){
	...
        
    int des = msg_->getDestination();
    
    // forward the message based on the route plan
    if (nex[des] != -1) forwardMessageToSwitch(msg_, nex[des]);
    
    //otherwise, request the domain controller to send route plan
    else forwardMessageToDomain(msg_);
    ...
}

```

### 2.5 SARSA

Based on the [framework](https://github.com/dennybritz/reinforcement-learning/blob/master/TD/SARSA%20Solution.ipynb), I implment the SARSA algorithm with softmax policy. But it is a little different with the problem solved in that framework. That's because there's totally N, the number of all switches in the network, SARSA processes running for this SDN. Since only the destination of the message matters, but the source of it doesn't, so there comes N. And based on that, there're some tricky variables:

```c++
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

        // SARSA algorithm
        int visit[20][20]; // visit[src][des]: the number of times the `src -> des` message has been visited
        double Q[20][20][20]; // Q[des][state][action]: 'des'-th SARSA among N SARSA, returns the reward when the current state is 'state' and the next action is 'action'
        
        //some parameters mentioned in the paper
        double epsilon = 0.1, tau0 = 100, tauT = 0.05, T = 100;
        double beta1 = 1, beta2 = 1, beta3 = 1, theta1 = 1, phi1 = 1;
        double theta2 = 0.5, phi2 = 0.5;
        double alpha = 0.9, gamma = 0.7;
        double pi = cos(-1.0);

    protected:
      virtual void getIsIn();
      virtual double getTau();
      virtual double getReward();
      virtual void forwardMessageToSlave();
      virtual void forwardMessageToSwitch();
      virtual void initialize() override;
      virtual void handleMessage(cMessage *msg) override;
      virtual void copyCondition(condition* cond);
      virtual void retrieveCondition(cMessage* msg);
      virtual void forwardMessage(int to, int nextHop);
      virtual int makeSoftmaxPolicy(int state, double (& q)[20]);
      virtual void sarsa(double (& q)[20][20]);
};
```

### 2.6 Hot-start 

All the nodes should turn in its surrounding network condition to the senior node, otherwise this algorithm just can't start. According to my framework, the switch node must turn in the network condition to slave controller. 

Actually, it is better to record one time stamp for every variable in network condition, so as to know whether or not to update the variable based on the coming data. 

## 3. Problem 

### 3.1 pointer being freed was not allocated

The original error information is :`[malloc: *** error for object: pointer being freed was not allocated *** set a breakpoint in malloc_error_break to debug]`

After exploring many answers in StackOverflow, I come to the fact that the parameter of the default constructor is not matched with the parameter I give to, which result this error. After fixing that, it works. 

#### 3.2 Wrong type of gates

If we set the wrong type of gates and ask the node to send the message to some channels which don't exist at all, then the program will return "no more simulation"

