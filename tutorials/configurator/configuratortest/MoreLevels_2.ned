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
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.visualizer.networklayer.RoutingTableCanvasVisualizer;


network MoreLevels_2
{
    @display("bgb=907,526");
    submodules:
        configurator: IPv4NetworkConfigurator {
            @display("p=107,87");
        }
        visualizer: RoutingTableCanvasVisualizer {
            @display("p=112,167");
        }
        router: Router {
            @display("p=390,205");
        }
        router1: Router {
            @display("p=376,375");
        }
        router2: Router {
            @display("p=512,268");
        }
        router3: Router {
            @display("p=634,268");
        }
        router4: Router {
            @display("p=755,194");
        }
        router5: Router {
            @display("p=748,375");
        }
        standardHost0: StandardHost {
            @display("p=280,124");
        }
        standardHost1: StandardHost {
            @display("p=271,263");
        }
        standardHost2: StandardHost {
            @display("p=263,377");
        }
        standardHost3: StandardHost {
            @display("p=260,479");
        }
        standardHost4: StandardHost {
            @display("p=841,121");
        }
        standardHost5: StandardHost {
            @display("p=849,226");
        }
        standardHost6: StandardHost {
            @display("p=833,343");
        }
        standardHost7: StandardHost {
            @display("p=831,470");
        }
    connections:

        standardHost3.ethg++ <--> Eth100M <--> router1.ethg++;
        standardHost2.ethg++ <--> Eth100M <--> router1.ethg++;
        standardHost1.ethg++ <--> Eth100M <--> router.ethg++;
        standardHost0.ethg++ <--> Eth100M <--> router.ethg++;
        router.ethg++ <--> Eth100M <--> router2.ethg++;
        router1.ethg++ <--> Eth100M <--> router2.ethg++;
        router2.ethg++ <--> Eth100M <--> router3.ethg++;
        router3.ethg++ <--> Eth100M <--> router4.ethg++;
        router4.ethg++ <--> Eth100M <--> standardHost4.ethg++;
        router4.ethg++ <--> Eth100M <--> standardHost5.ethg++;
        router5.ethg++ <--> Eth100M <--> standardHost6.ethg++;
        router5.ethg++ <--> Eth100M <--> standardHost7.ethg++;
        router3.ethg++ <--> Eth100M <--> router5.ethg++;
}
