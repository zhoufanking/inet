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

import inet.node.ethernet.EtherSwitch;
import inet.node.inet.Router;
import inet.node.wireless.AccessPoint;


network modify
{
    @display("bgb=1002,675");
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
}
