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

package inet.examples.configuratortest;

import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.StandardHost;
import inet.visualizer.networklayer.RoutingTableCanvasVisualizer;
network ConfiguratorTest
{
    @display("bgb=739,400");
    submodules:
        etherSwitch: EtherSwitch {
            @display("p=199,186");
        }
        etherSwitch1: EtherSwitch {
            @display("p=497,175");
        }
        Host0: StandardHost {
            @display("p=77,317");
        }
        Host1: StandardHost {
            @display("p=187,318");
        }
        Host2: StandardHost {
            @display("p=275,314");
        }
        Host3: StandardHost {
            @display("p=453,314");
        }
        Host4: StandardHost {
            @display("p=569,310");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=64,64");
        }

        visualizer: RoutingTableCanvasVisualizer {
            @display("p=69,128");
        }
    connections:
        Host0.ethg++ <--> Eth100M <--> etherSwitch.ethg++;
        Host1.ethg++ <--> Eth100M <--> etherSwitch.ethg++;
        Host2.ethg++ <--> Eth100M <--> etherSwitch.ethg++;
        Host3.ethg++ <--> Eth100M <--> etherSwitch1.ethg++;
        Host4.ethg++ <--> Eth100M <--> etherSwitch1.ethg++;
        etherSwitch.ethg++ <--> Eth100M <--> etherSwitch1.ethg++;
}
