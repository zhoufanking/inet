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
        Area1Subnet1Host0: StandardHost {
            @display("p=25,98");
        }
        Area1Subnet1Host1: StandardHost {
            @display("p=98,101");
        }
        Area1Subnet1Host2: StandardHost {
            @display("p=191,96");
        }
        Area1Switch1: EtherSwitch {
            @display("p=100,182");
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
        Area2Switch1: EtherSwitch {
            @display("p=308,607");
        }
        Area3Subnet1Host1: StandardHost {
            @display("p=830,448");
        }
        Area3Subnet1Host2: StandardHost {
            @display("p=922,470");
        }
        Area3Subnet1Host3: StandardHost {
            @display("p=1079.54,455.65002");
        }
        Area3Switch1: EtherSwitch {
            @display("p=936,378");
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
        Area1Switch2: EtherSwitch {
            @display("p=104,391");
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
        Area2Switch2: EtherSwitch {
            @display("p=680,613");
        }
        Area3Subnet2Host1: StandardHost {
            @display("p=790,70");
        }
        Area3Subnet2Host2: StandardHost {
            @display("p=928,74");
        }
        Area3Subnet2Host3: StandardHost {
            @display("p=1100.5701,73.605");
        }
        Area3Switch2: EtherSwitch {
            @display("p=946,154");
        }
        Area1Router: Router {
            @display("p=290,275");
        }
        Area2Router: Router {
            @display("p=486,511");
        }
        Area3Router: Router {
            @display("p=806,254");
        }
        Backbonerouter: Router {
            @display("p=531.0075,334.7275;i=abstract/router2");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=308.88,46.332");
        }
        visualizer: RoutingTableCanvasVisualizer {
            @display("p=494.208,49.14");
        }
    connections:
        Area2Subnet1Host1.ethg++ <--> Eth100M <--> Area2Switch1.ethg++;
        Area2Subnet1Host2.ethg++ <--> Eth100M <--> Area2Switch1.ethg++;
        Area2Subnet1Host3.ethg++ <--> Eth100M <--> Area2Switch1.ethg++;
        Area1Switch2.ethg++ <--> Eth100M <--> Area1Subnet2Host1.ethg++;
        Area1Subnet2Host2.ethg++ <--> Eth100M <--> Area1Switch2.ethg++;
        Area1Switch2.ethg++ <--> Eth100M <--> Area1Subnet2Host3.ethg++;
        Area1Switch1.ethg++ <--> Eth100M <--> Area1Subnet1Host0.ethg++;
        Area1Switch1.ethg++ <--> Eth100M <--> Area1Subnet1Host1.ethg++;
        Area1Switch1.ethg++ <--> Eth100M <--> Area1Subnet1Host2.ethg++;
        Area1Switch1.ethg++ <--> Eth100M <--> Area1Router.ethg++;
        Area1Switch2.ethg++ <--> Eth100M <--> Area1Router.ethg++;
        Area1Router.ethg++ <--> Eth100M <--> Backbonerouter.ethg++;
        Area2Switch1.ethg++ <--> Eth100M <--> Area2Router.ethg++;
        Area2Router.ethg++ <--> Eth100M <--> Area2Switch2.ethg++;
        Area2Switch2.ethg++ <--> Eth100M <--> Area2Subnet2Host1.ethg++;
        Area2Switch2.ethg++ <--> Eth100M <--> Area2Subnet2Host2.ethg++;
        Area2Switch2.ethg++ <--> Eth100M <--> Area2Subnet2Host3.ethg++;
        Area3Switch1.ethg++ <--> Eth100M <--> Area3Subnet1Host1.ethg++;
        Area3Switch1.ethg++ <--> Eth100M <--> Area3Subnet1Host2.ethg++;
        Area3Switch1.ethg++ <--> Eth100M <--> Area3Subnet1Host3.ethg++;
        Area3Switch1.ethg++ <--> Eth100M <--> Area3Router.ethg++;
        Area3Router.ethg++ <--> Eth100M <--> Area3Switch2.ethg++;
        Area3Switch2.ethg++ <--> Eth100M <--> Area3Subnet2Host1.ethg++;
        Area3Switch2.ethg++ <--> Eth100M <--> Area3Subnet2Host2.ethg++;
        Area3Switch2.ethg++ <--> Eth100M <--> Area3Subnet2Host3.ethg++;
        Area3Router.ethg++ <--> Eth100M <--> Backbonerouter.ethg++;
        Area2Router.ethg++ <--> Eth100M <--> Backbonerouter.ethg++;
}
