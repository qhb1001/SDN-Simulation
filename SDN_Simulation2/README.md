# Experiment 2 - Design simples for switches

It is neccessary to make use of the following statement carefully to get a good design

- input gate;
- output gate;
- inout gate;

And the methods below is important 

- gateSize("gate");

  count the number of two-way channel

- send(msg, "gate$o", k);

  send the message to k-th channel

- getIndex()

- getName()

## Simples

### Switch

There exists two directions for message to send: one for peer swithes, another for sending message to slave controller, which can be implemented in method `forwardMessage(cMessage*)`. 

It should be noticed that the link between peer switches are `inout gate[];`, whereas the link to slave controller is `output slave[];`, the link to domain controller is `output domain[];`. 

### Slave controller

Receive information about network state from switches and transfer it ro domain controller. 

- Information in salve controller should be updated whenever there's change in network. 

  So there should exists `input in[];` to receive information from switches. 

- Domain controller get the network state from slave swtiches whenever neccessary, so the link between them should be two-way link. Domain controller inform the slave controller to send the information, and then slave controller send it. 

  So there should exists `inout gate[];` in slave controller. 

### Domain controller

- From what is discussed above, it should have `inout gate[]` to link with slave controllers.

- If the source node and destination node are not in a same subnet, then it should deliver the message to transport to super controller. What's more, when neccessary, it should send the network state information to super controller when the situation above happen. This should happen when the super inform it. 

  After super controller get the work done, it will send the forward table to domain controller.

  Everything above makes `inout gate[];` neccessary. 

### Super controller

The interaction with domain controller is clear above, which makes `inout gate[];` neccessary. 

## Problem

I don't know the function to compare two strings, while compilor tells me to use `strncmp` but it didn't work. So I code it myself. 

# 