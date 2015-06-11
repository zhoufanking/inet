//
// Copyright (C) 2012 Opensim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

package inet.networklayer.generic;

import inet.networklayer.arp.generic.GenericARP;
import inet.networklayer.common.EchoProtocol;
import inet.networklayer.contract.INetworkLayer;


//
// This module provides the generic network layer.
//
module GenericNetworkLayer like INetworkLayer
{
    parameters:
        bool forwarding = default(false);
        bool multicastForwarding = default(false);
        string interfaceTableModule;
        string routingTableModule;
        *.forwarding = default(forwarding);
        *.multicastForwarding = default(multicastForwarding);
        *.interfaceTableModule = default(absPath(interfaceTableModule));
        *.routingTableModule = default(absPath(".routingTable"));
        *.arpModule = default(absPath(".arp"));
        @display("i=block/fork");
    gates:
        input ifIn @labels(INetworkDatagram);
        output ifOut @labels(INetworkDatagram);
        input transportIn @labels(IPv4ControlInfo/down);
        output transportOut @labels(IPv4ControlInfo/up);

    submodules:
        routingTable: GenericRoutingTable {
            parameters:
                @display("p=100,50;is=s");
        }
        gnp: GenericNetworkProtocol {
            parameters:
                @display("p=85,95;q=queue");
            gates:
        }
        echo: EchoProtocol {
            parameters:
                @display("p=160,63");
        }
        arp: GenericARP {
            parameters:
                @display("p=202,142;q=pendingQueue");
        }
    connections allowunconnected:
        gnp.transportOut++ --> { @display("m=n"); } --> transportOut;
        gnp.transportIn++ <-- { @display("m=n"); } <-- transportIn;

        gnp.transportOut++ --> echo.localIn;
        gnp.transportIn++ <-- echo.sendOut;

        gnp.arpOut --> arp.netwIn;
        gnp.arpIn <-- arp.netwOut;

        ifIn --> { @display("m=s"); } --> gnp.queueIn;
        gnp.queueOut --> { @display("m=s"); } --> ifOut;
}
