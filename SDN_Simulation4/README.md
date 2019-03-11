# Experiment 4 Randomize the Network

## Goal

In order to implement a relatively real network and get the reward function in RL algorithm done, we need to set some random values. **For the sake of simpleness, I set these random value to be static.** In other words, once they have been set, those values won't change during the simulation.

## Details

Biscally, five values need to be randomized and they are: 1)transmission delay 2)queuing delay 3)loss rate 4)available bandwidth 5)total bandwidth, which need to be set for every link. 

As for the first three values, it is reasonable for me to make them follow normal distribution because in normal situation.They will alawys be in good work condition which means 

- the transmission delay and queuing delay shouldn't be too low or too high
- I think 10% is a reasonable value for loss rates

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