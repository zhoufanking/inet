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

import inet.node.ethernet.Eth100M;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.node.inet.WirelessHost;
import inet.node.wireless.AccessPoint;


network modify
{
    @display("bgb=1356.6421,793.072");
    submodules:
        router: Router {
            @display("p=316,308");
        }
        router1: Router {
            @display("p=487,423");
        }
        router2: Router {
            @display("p=481,185");
        }
        router3: Router {
            @display("p=651,305");
        }
        etherSwitch: EtherSwitch {
            @display("p=608,526");
        }
        etherSwitch1: EtherSwitch {
            @display("p=374,541");
        }
        etherSwitch2: EtherSwitch {
            @display("p=781,258");
        }
        etherSwitch3: EtherSwitch {
            @display("p=780,395");
        }
        accessPoint: AccessPoint {
            @display("p=622,93");
        }
        standardHost: StandardHost {
            @display("p=258.02002,661.346");
        }
        standardHost1: StandardHost {
            @display("p=366.66,655.914");
        }
        standardHost2: StandardHost {
            @display("p=494.312,674.926");
        }
        standardHost3: StandardHost {
            @display("p=681.716,600.236");
        }
        standardHost4: StandardHost {
            @display("p=775.418,578.508");
        }
        standardHost5: StandardHost {
            @display("p=930.23004,460.36203");
        }
        standardHost6: StandardHost {
            @display("p=954.674,355.79602");
        }
        standardHost7: StandardHost {
            @display("p=907.14404,234.934");
        }
        standardHost8: StandardHost {
            @display("p=787.64,162.96");
        }
        standardHost9: StandardHost {
            @display("p=726.53,42.098");
        }
        wirelessHost: WirelessHost {
            @display("p=578.508,33.95");
        }
        wirelessHost1: WirelessHost {
            @display("p=467.152,29.876001");
        }
        standardHost10: StandardHost {
            @display("p=200.98401,382.95602");
        }
        standardHost11: StandardHost {
            @display("p=186.046,241.72401");
        }
    connections:
        standardHost10.ethg++ <--> Eth100M <--> router.ethg++;
        standardHost11.ethg++ <--> Eth100M <--> router.ethg++;
        etherSwitch1.ethg++ <--> Eth100M <--> router1.ethg++;
        standardHost.ethg++ <--> Eth100M <--> etherSwitch1.ethg++;
        standardHost1.ethg++ <--> Eth100M <--> etherSwitch1.ethg++;
        etherSwitch1.ethg++ <--> Eth100M <--> standardHost2.ethg++;
        etherSwitch.ethg++ <--> Eth100M <--> standardHost3.ethg++;
        etherSwitch.ethg++ <--> Eth100M <--> standardHost4.ethg++;
        etherSwitch.ethg++ <--> Eth100M <--> router1.ethg++;
        router1.ethg++ <--> Eth100M <--> router3.ethg++;
        router3.ethg++ <--> Eth100M <--> etherSwitch3.ethg++;
        etherSwitch3.ethg++ <--> Eth100M <--> standardHost5.ethg++;
        etherSwitch3.ethg++ <--> Eth100M <--> standardHost6.ethg++;
        etherSwitch2.ethg++ <--> Eth100M <--> standardHost7.ethg++;
        etherSwitch2.ethg++ <--> Eth100M <--> standardHost8.ethg++;
        router3.ethg++ <--> Eth100M <--> etherSwitch2.ethg++;
        router2.ethg++ <--> Eth100M <--> router3.ethg++;
        router.ethg++ <--> Eth100M <--> router2.ethg++;
        router.ethg++ <--> Eth100M <--> router1.ethg++;
        router2.ethg++ <--> Eth100M <--> accessPoint.ethg++;
        accessPoint.ethg++ <--> Eth100M <--> standardHost9.ethg++;
}
