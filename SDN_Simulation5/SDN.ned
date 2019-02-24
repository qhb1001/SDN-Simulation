//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

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
        output slave;
        output domain;
}

simple slave_controller 
{
    parameters:
		@display("i=block/routing");
		
	gates:
	    input in[];
	    inout domain;
}

simple domain_controller 
{
	parameters:
		@display("i=block/routing");
		
	gates:
	    inout slave[];
	    inout super[];
	    input in[];
}

simple super_controller 
{
	parameters:
		@display("i=block/routing");
	
	gates:
	    inout gate[];
}

network sdn_network 
{
    types:
        channel Channel extends ned.DelayChannel {
            delay = 100ms;  //将来需要将这个量随机化
        }
        
	submodules:
	    switches[13] : sdn_switch;
	    slave[4] : slave_controller; 
	    // 将来需要为每个类型的交换机设置一个simple
	    // 并且其中管道的类型也需要重新定义，具体到方向
	    // 因为后面当进行随机数据转发时，需要统计出口的个数，如果是双向通道就不行了
	    domain[2] : domain_controller;
	    super : super_controller;
	
	connections:
	    //SDN switches
	    switches[0].gate++ <--> Channel <--> switches[1].gate++;
	    switches[0].gate++ <--> Channel <--> switches[3].gate++;
	    switches[3].gate++ <--> Channel <--> switches[2].gate++;
	    switches[3].gate++ <--> Channel <--> switches[4].gate++;
	    switches[3].gate++ <--> Channel <--> switches[5].gate++;
	    switches[4].gate++ <--> Channel <--> switches[6].gate++;
		switches[10].gate++ <--> Channel <--> switches[7].gate++;
		switches[10].gate++ <--> Channel <--> switches[9].gate++;
		switches[10].gate++ <--> Channel <--> switches[12].gate++;
		switches[9].gate++ <--> Channel <--> switches[8].gate++;
		switches[9].gate++ <--> Channel <--> switches[11].gate++;
		switches[11].gate++ <--> Channel <--> switches[12].gate++;
		switches[5].gate++ <--> Channel <--> switches[7].gate++;
		switches[4].gate++ <--> Channel <--> switches[9].gate++;
	    
	    // slave controller
	    for i = 0..2 {
	    	slave[0].in++ <-- Channel <-- switches[i].slave;   
	    }
	    for i = 3..6 {
	    	slave[1].in++ <-- Channel <-- switches[i].slave;
	    }
	    for i = 7..9 {
	    	slave[2].in++ <-- Channel <-- switches[i].slave;
	    }
	    for i = 10..12 {
	    	slave[3].in++ <-- Channel <-- switches[i].slave;
	    }
	    
	    // domain controller
	    slave[0].domain <--> Channel <--> domain[0].slave++;
	    slave[1].domain <--> Channel <--> domain[0].slave++;
	    slave[2].domain <--> Channel <--> domain[1].slave++;
	    slave[3].domain <--> Channel <--> domain[1].slave++;
	    
	    for i = 0..6 {
	    	switches[i].domain --> Channel --> domain[0].in++;
	    }
	    
	    for i = 7..12 {
	    	switches[i].domain --> Channel --> domain[1].in++;
	    }
	    
	    // super controller
	    for i = 0..1 {
	    	super.gate++ <--> Channel <--> domain[i].super++;
	    }
}









