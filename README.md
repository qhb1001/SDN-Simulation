# Experiment 1 - Topology Structure of Network

## Goal

This is the network topology structure designed by "parameter". 

![](SDN.png)

It should be noticed that I design this network from nowhere... Because I can't find a specific SDN topology structure with three layers in paper. At the same time, because my coding ability is not so good yet, I set few nodes for future debugging. 

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

In order to implement a relatively real network and get the reward function in RL algorithm done, we need to set some random values. 

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

# Goal

In order to handle the message generation and forward, there should exist several types of message, such as 1)from switch to switch 2)from switch to domain 3) from domain to switch 4)from switch to slave 5) from slave to domain 6)from domain to super 7)from super to domain

Not only the contents in messages differ, but also the operation after receiving messages differ. What's more, every simple also need some data structures to communicate with each other, such as forward table, storing the information of network or subnetwork, etc. 

## Detail

### 1. Send message to specific switch

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

### 2. Design contents of message for every link

* from switch to switch
* from switch to domain
* from domain to switch 
* from switch to slave
* from slave to domain
* from domain to super
* from super to domain