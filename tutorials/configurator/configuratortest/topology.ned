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


network Topology
{
    @display("bgb=988,400");
    submodules:
        standardHost: StandardHost {
            @display("p=254,146");
        }
        standardHost1: StandardHost {
            @display("p=315,299");
        }
        standardHost2: StandardHost {
            @display("p=516,185");
        }
        standardHost3: StandardHost {
            @display("p=863,310");
        }
        standardHost4: StandardHost {
            @display("p=883,82");
        }
        router: Router {
            @display("p=380,201");
        }
        router1: Router {
            @display("p=771,191");
        }
        router2: Router {
            @display("p=574,332");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=100,100");
        }
        visualizer: RoutingTableCanvasVisualizer {
            @display("p=100,200");
        }
    connections:
        standardHost1.ethg++ <--> Eth100M <--> router.ethg++;
        standardHost.ethg++ <--> Eth100M <--> router.ethg++;
        standardHost2.ethg++ <--> Eth100M <--> router.ethg++;
        router.ethg++ <--> Eth100M <--> router2.ethg++;
        router2.ethg++ <--> Eth100M <--> router1.ethg++;
        router1.ethg++ <--> Eth100M <--> standardHost3.ethg++;
        router1.ethg++ <--> Eth100M <--> standardHost4.ethg++;
}
