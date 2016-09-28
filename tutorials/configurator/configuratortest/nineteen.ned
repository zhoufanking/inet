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

package inet.tutorials.configurator.configuratortest;

import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth10G;
import inet.node.ethernet.Eth1G;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.visualizer.networklayer.RoutingTableCanvasVisualizer;


network nineteen
{
    @display("bgb=1398,793");
    submodules:
        Area1Router: Router {
            @display("p=290,275");
        }
        Area3Router: Router {
            @display("p=806,254");
        }
        Area2Router: Router {
            @display("p=486,511");
        }
        Backbonerouter: Router {
            @display("p=531.0075,334.7275;i=abstract/router2");
        }
        Switch0: EtherSwitch {
            @display("p=100,182");
        }
        Switch1: EtherSwitch {
            @display("p=104,391");
        }
        Switch2: EtherSwitch {
            @display("p=946,154");
        }
        Switch3: EtherSwitch {
            @display("p=936,378");
        }
        Switch4: EtherSwitch {
            @display("p=308,607");
        }
        Switch5: EtherSwitch {
            @display("p=680,613");
        }
        Area1Subnet1Host0: StandardHost {
            @display("p=25,98");
        }
        Area1Subnet1Host1: StandardHost {
            @display("p=98,101");
        }
        Area1Subnet1Host2: StandardHost {
            @display("p=191,96");
        }
        Area1Subnet2Host1: StandardHost {
            @display("p=39,482");
        }
        Area1Subnet2Host2: StandardHost {
            @display("p=123,482");
        }
        Area1Subnet2Host3: StandardHost {
            @display("p=215,485");
        }
        Area2Subnet1Host1: StandardHost {
            @display("p=176,659");
        }
        Area2Subnet1Host2: StandardHost {
            @display("p=268,667");
        }
        Area2Subnet1Host3: StandardHost {
            @display("p=344,670");
        }
        Area2Subnet2Host1: StandardHost {
            @display("p=667,527");
        }
        Area2Subnet2Host2: StandardHost {
            @display("p=775,556");
        }
        Area2Subnet2Host3: StandardHost {
            @display("p=799,644");
        }
        Area3Subnet1Host1: StandardHost {
            @display("p=830,448");
        }
        Area3Subnet1Host2: StandardHost {
            @display("p=922,470");
        }
        Area3Subnet2Host1: StandardHost {
            @display("p=790,70");
        }
        Area3Subnet2Host2: StandardHost {
            @display("p=928,74");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=308.88,46.332");
        }
        visualizer: RoutingTableCanvasVisualizer {
            @display("p=494.208,49.14");
        }
        Area3Subnet2Host3: StandardHost {
            @display("p=1100.5701,73.605");
        }
        Area3Subnet1Host3: StandardHost {
            @display("p=1079.54,455.65002");
        }
    connections:
        Area2Subnet1Host1.ethg++ <--> Eth100M <--> Switch4.ethg++;
        Area2Subnet1Host2.ethg++ <--> Eth100M <--> Switch4.ethg++;
        Area2Subnet1Host3.ethg++ <--> Eth100M <--> Switch4.ethg++;
        Switch1.ethg++ <--> Eth100M <--> Area1Subnet2Host1.ethg++;
        Area1Subnet2Host2.ethg++ <--> Eth100M <--> Switch1.ethg++;
        Switch1.ethg++ <--> Eth100M <--> Area1Subnet2Host3.ethg++;
        Switch0.ethg++ <--> Eth100M <--> Area1Subnet1Host0.ethg++;
        Switch0.ethg++ <--> Eth100M <--> Area1Subnet1Host1.ethg++;
        Switch0.ethg++ <--> Eth100M <--> Area1Subnet1Host2.ethg++;
        Switch0.ethg++ <--> Eth100M <--> Area1Router.ethg++;
        Switch1.ethg++ <--> Eth100M <--> Area1Router.ethg++;
        Area1Router.ethg++ <--> Eth100M <--> Backbonerouter.ethg++;
        Switch4.ethg++ <--> Eth100M <--> Area2Router.ethg++;
        Area2Router.ethg++ <--> Eth100M <--> Switch5.ethg++;
        Switch5.ethg++ <--> Eth100M <--> Area2Subnet2Host1.ethg++;
        Switch5.ethg++ <--> Eth100M <--> Area2Subnet2Host2.ethg++;
        Switch5.ethg++ <--> Eth100M <--> Area2Subnet2Host3.ethg++;
        Switch3.ethg++ <--> Eth100M <--> Area3Subnet1Host1.ethg++;
        Switch3.ethg++ <--> Eth100M <--> Area3Subnet1Host2.ethg++;
        Switch3.ethg++ <--> Eth100M <--> Area3Subnet1Host3.ethg++;
        Switch3.ethg++ <--> Eth100M <--> Area3Router.ethg++;
        Area3Router.ethg++ <--> Eth100M <--> Switch2.ethg++;
        Switch2.ethg++ <--> Eth100M <--> Area3Subnet2Host1.ethg++;
        Switch2.ethg++ <--> Eth100M <--> Area3Subnet2Host2.ethg++;
        Switch2.ethg++ <--> Eth100M <--> Area3Subnet2Host3.ethg++;
        Area3Router.ethg++ <--> Eth100M <--> Backbonerouter.ethg++;
        Area2Router.ethg++ <--> Eth100M <--> Backbonerouter.ethg++;
}
