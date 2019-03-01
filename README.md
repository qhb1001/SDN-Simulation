# Experiment 1 - Topology Structure of Network

## Goal

This is the network topology structure designed by "parameter". 

![](SDN.png)

It should be noticed that I design this network from nowhere... Because I can't find a specific SDN topology structure with three layers in paper. At the same time, because my coding ability is not so good yet, I set few nodes for future debugging. 

2.21 Well… I suddenly come to the fact that why there are only two layers in the original paper: because the slave controllers are only responsible for the transferring of information and nothing else. The meaning of it is just reduce the strain of the network, while the domain controller and super controller are enough for this new kind of network. 

## Problem

If there exits blank in project's name, programm won't be compiled successfully. 

# Experiment 2 - Design simples for switches

It is neccessary to make use of the following statement carefully to get a good design

* input gate;
* output gate;
* inout gate;

And the methods below is important 

* gateSize("gate");

  count the number of two-way channel

* send(msg, "gate$o", k);

  send the message to k-th channel

* getIndex()

* getName()

## Simples

### Switch

There exists two directions for message to send: one for peer swithes, another for sending message to slave controller, which can be implemented in method `forwardMessage(cMessage*)`. 

It should be noticed that the link between peer switches are `inout gate[];`, whereas the link to slave controller is `output slave[];`, the link to domain controller is `output domain[];`. 

### Slave controller

Receive information about network state from switches and transfer it ro domain controller. 

* Information in salve controller should be updated whenever there's change in network. 

  So there should exists `input in[];` to receive information from switches. 

* Domain controller get the network state from slave swtiches whenever neccessary, so the link between them should be two-way link. Domain controller inform the slave controller to send the information, and then slave controller send it. 

  So there should exists `inout gate[];` in slave controller. 

### Domain controller

* From what is discussed above, it should have `inout gate[]` to link with slave controllers.

* If the source node and destination node are not in a same subnet, then it should deliver the message to transport to super controller. What's more, when neccessary, it should send the network state information to super controller when the situation above happen. This should happen when the super inform it. 

  After super controller get the work done, it will send the forward table to domain controller.

  Everything above makes `inout gate[];` neccessary. 

### Super controller

The interaction with domain controller is clear above, which makes `inout gate[];` neccessary. 

## Problem

I don't know the function to compare two strings, while compilor tells me to use `strncmp` but it didn't work. So I code it myself. 


# Experiment 3 Implement stop-and-wait protocol

## Goal

In order to make interaction between the nodes, there should be some kind of protocol to communicate. So I implement stop-and-wait protocol in this network. 

And for that, there should be some timer and ack signals. Acorrding to Official Guide, I use 

* `simtime_t timeout;  // timeout`
* `cMessage *timeoutEvent;  // holds pointer to the timeout self-message`

These two signals are designed to record the time when to send the message, and send the time-out message to the sender. The core function in this experiment is `handleMessage(cMessage *msg);`

* `initialize()`

  initialize the signals `simtime_t timeout;` and `cMessage *timeoutEvent;`, and generate the message, make `msg_` to record that. 

* `sendACK(sdn_message *msg)`

  use the [method](https://stackoverflow.com/questions/48481224/how-to-reply-to-the-message-in-omnet) to get the sender and send ack to it. 

* `handleMessage(cMessage *msg)`

  * if it is a `msg` message, then it should have a choice
    * if this is the destination, then finish this message and start a new one
    * else forward this message
  * if it is a `ack` message, then delete the timer
  * if it is a `timeOut` message, then retransfer the message

* `generateMessage(char *a)`

  generate the kind of `a` message and return it

  this method also set the source and destination for the message

* `forwardMessage(sdn_message *msg)`

  send the message to next random port

## Problem

1. [How to reply to the message in OMNet++?](https://stackoverflow.com/questions/48481224/how-to-reply-to-the-message-in-omnet)
2. use default method like `setName()` and `getName()` regularly for `cMessage*`
3. `getVectorSize()` will return the total gates in network, while `gateSize("gate")` will only return the number of nodes attached to this node.

# Experiment 4 Randomize the Network

## Goal

In order to implement a relatively real network and get the reward function in RL algorithm done, we need to set some random values. **For the sake of simpleness, I set these random value to be static.** In other words, once they have been set, those values won't change during the simulation.

## Details

Biscally, five values need to be randomized and they are: 1)transmission delay 2)queuing delay 3)loss rate 4)available bandwidth 5)total bandwidth, which need to be set for every link. 

As for the first three values, it is reasonable for me to make them follow normal distribution because in normal situation.They will alawys be in good work condition which means 

* the transmission delay and queuing delay shouldn't be too low or too high
* I think 10% is a reasonable value for loss rates

As for the last two values, since the event in reality is random, they are reasonable to be random too. 

In this omnet++ framework, we can use built-in feature to get random number which follow some kind of distribution we want. 

First of all, change simple `sdn_switch` in `.NED` file as follows

```c++
simple sdn_switch
{
    parameters:
        @signal[arrival](type="long");
        @statistic[hopCount](title="hop count"; source="arrival"; record=vector,stats; interpolationmode=none);
		volatile double transmissionDelay;
		volatile double queuingDelay;
		volatile double loss;
		volatile double availableBandwidth;
		volatile double totalBandwidth; 
        @display("i=block/routing");
        
    gates:
        inout gate[];
        output slave[];
        output domain[];
}
```

then for `.ini` file. Here I change the default seed to 532569. 

```c++
[General]
network = sdn_network
repeat = 1
seed-0-mt=532569
experiment-label = ${configname}
sdn_network.switches[*].transmissionDelay = truncnormal(50, 30)
sdn_network.switches[*].queuingDelay = truncnormal(50, 30)
sdn_network.switches[*].loss = truncnormal(0.1, 0.11)
sdn_network.switches[*].availableBandwidth = uniform(1, 101)
sdn_network.switches[*].totalBandwidth = uniform(1, 101)
```

Finally, we can get these values by `par("valueName")`, and it can be gurantted that every time we code this thing, we will get a new value following the specific distribution. It's just like one function call. 

## Problem

Remember to save the changes... otherwise you will find that no matter how many times trying to build it or compile it, the executable programs just don't change. 

# Experiment 5 Design Software Defined Network

## 1. Goal

In order to handle the message generation and forward, there should exist several types of message, such as 1)from switch to switch 2)from switch to domain 3) from domain to switch 4)from switch to slave 5) from slave to domain 6)from domain to super 7)from super to domain

Not only the contents in messages differ, but also the operation after receiving messages differ. What's more, every simple also need some data structures to communicate with each other, such as forward table, storing the information of network or subnetwork, etc. 

### 2. Design contents of message for every link

#### 2.1 message from switches

This type of message includes three kinds of link, which are: 

- from switch to switch
- from switch to domain
- from switch to slave

The first two kinds of link use the source address and destination address to plan the route for it. While the last one needs the condition of the network for RL algorithm. Based on the discussion above, I design the message from switch like this: 

```c++
message switch_message
{
    int source;
    int destination;
	double loss[20];
	double transmissionDelay[20];
	double queuingDelay[20];
	double availableBandwidth[20];
	double totalBandwidth[20];
    int hopCount = 0;
}
```

And every switch will have about the same information. As for the reason why these arraies are one-demensional is that omnet++ just doesn't support two-demensional definition in `.msg` file...

**In fact, we can randomize the network condition whenever we use it.**

#### 2.2 message from slave controller

- from slave to domain

After exploring the content about source code of class [cObecjt](https://doc.omnetpp.org/omnetpp/api/classomnetpp_1_1cArray.html), I figure out one way to transfer the  two-dimensional network condition from slave controller to domain controller by cMessge object like this: 

```c++
condition* cond = new condition();
cond->loss[2][1] = 404;
msg->addObject(cond);
condition* cond_ = (condition* )(msg->getObject("")); // the default name for cObject
EV << "This is the original value: " << cond_->loss[2][1] << endl; // retrieve the value in loss[2][1]
```

#### 2.3 massage from domain controller

- from domain to switch
- from domain to super

The purpose of the first kind message is to ask slave controller to send network condition information to itself. The message of the second type is about the same with the message on  `slave -> domain` this link. 

#### 2.4 message from super controller

- from super to domain

There are two kinds of message: a) request retrieve network information from domain controller b) send optimal route to domain controller

### 3. Detail 

#### 3.1 How to add parameters to the cMessage object

First of all, my way to include information such as ID, network condition, etc. into the built-in data structure `cMessage` is to utilize the function of `parList`. For example, if I want to add a new attribute `totalbandwidth` to cMessage pointer `msg`, it would be something like this

```c++
msg->addPar("totalBandwidth").setDoubleValue(123.2); //set the value to 123.2
EV << msg->par("totalBandwidth").doubleValue() << endl; // output the specific value
```

***The source code is the most powerful tool to learn omnet++ framework.*** 

#### 3.2 How to get the module name the message belongs to

```c++
msg->getSenderModule()->getName();
```

#### 3.3 How to send message from switch to domain controller

it should be noticed that after applied the function `send`, the message being sent will be deleted in memory. So it is necessary to make a copy before send it like this: 

```c++
void sdn_switch::forwardMessageToDomain(switch_message *msg) {
    cMessage* copy = (cMessage*) msg->dup();
    send(copy, "domain");
}
```

#### 3.4 How to send message to one specific switch 

```c++
void sdn_switch::forwardMessage(sdn_message *msg, int to)
{
    // Increment hop count.
    msg->setHopCount(msg->getHopCount()+1);
    msg->setSource(getIndex());

    for (cModule::GateIterator i(this); !i.end(); i++)
    {
         cGate *gate = i();
         std::string gateStr = gate->getName();
         if (gateStr == "gate$o" && gate->getPathEndGate()->getOwnerModule()->getIndex() == to)
         {
             int senderId = gate->getIndex();
             send(msg, "gate$o", senderId);
         }
    }
}
```

#### 3.5 How many nodes exist in one subnet(domain controller)? 

In other words, which nodes are included under one domain controller? 

During the initialization process, switch node should turn in its connectivity condition to domain controller by `scheduleAt()` function, which will send "initialization" message to `handleMessage` function, and this function will report to domain controller. 

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