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
	    slave[4] : sdn_switch; 
	    // 将来需要为每个类型的交换机设置一个simple
	    // 并且其中管道的类型也需要重新定义，具体到方向
	    // 因为后面当进行随机数据转发时，需要统计出口的个数，如果是双向通道就不行了
	    domain[2] : sdn_switch;
	    super : sdn_switch;
	
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
	    
	    // slave controller
	    for i = 0..2 {
	    	slave[0].gate++ <--> Channel <--> switches[i].gate++;   
	    }
	    for i = 3..6 {
	    	slave[1].gate++ <--> Channel <--> switches[i].gate++;
	    }
	    for i = 7..9 {
	    	slave[2].gate++ <--> Channel <--> switches[i].gate++;
	    }
	    for i = 10..12 {
	    	slave[3].gate++ <--> Channel <--> switches[i].gate++;
	    }
	    
	    // domain controller
	    slave[0].gate++ <--> Channel <--> domain[0].gate++;
	    slave[1].gate++ <--> Channel <--> domain[0].gate++;
	    slave[2].gate++ <--> Channel <--> domain[1].gate++;
	    slave[3].gate++ <--> Channel <--> domain[1].gate++;
	    
	    // super controller
	    for i = 0..1 {
	    	super.gate++ <--> Channel <--> domain[i].gate++;
	    }
}









