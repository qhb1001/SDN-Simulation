# Experiment 3 Implement stop-and-wait protocol

## Goal

In order to make interaction between the nodes, there should be some kind of protocol to communicate. So I implement stop-and-wait protocol in this network. 

And for that, there should be some timer and ack signals. Acorrding to Official Guide, I use 

- `simtime_t timeout;  // timeout`
- `cMessage *timeoutEvent;  // holds pointer to the timeout self-message`

These two signals are designed to record the time when to send the message, and send the time-out message to the sender. The core function in this experiment is `handleMessage(cMessage *msg);`

- `initialize()`

  initialize the signals `simtime_t timeout;` and `cMessage *timeoutEvent;`, and generate the message, make `msg_` to record that. 

- `sendACK(sdn_message *msg)`

  use the [method](https://stackoverflow.com/questions/48481224/how-to-reply-to-the-message-in-omnet) to get the sender and send ack to it. 

- `handleMessage(cMessage *msg)`

  - if it is a `msg` message, then it should have a choice
    - if this is the destination, then finish this message and start a new one
    - else forward this message
  - if it is a `ack` message, then delete the timer
  - if it is a `timeOut` message, then retransfer the message

- `generateMessage(char *a)`

  generate the kind of `a` message and return it

  this method also set the source and destination for the message

- `forwardMessage(sdn_message *msg)`

  send the message to next random port

## Problem

1. [How to reply to the message in OMNet++?](https://stackoverflow.com/questions/48481224/how-to-reply-to-the-message-in-omnet)
2. use default method like `setName()` and `getName()` regularly for `cMessage*`
3. `getVectorSize()` will return the total gates in network, while `gateSize("gate")` will only return the number of nodes attached to this node.