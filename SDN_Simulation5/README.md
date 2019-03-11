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