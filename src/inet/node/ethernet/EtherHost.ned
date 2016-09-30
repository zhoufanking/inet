//
// Copyright (C) 2003 Andras Varga; CTIE, Monash University, Australia
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


package inet.node.ethernet;

import inet.applications.ethernet.EtherAppCli;
import inet.applications.ethernet.EtherAppSrv;
import inet.common.MessageDispatcher;
import inet.common.lifecycle.NodeStatus;
import inet.linklayer.ethernet.EthernetInterface;
import inet.networklayer.common.InterfaceTable;


//
// An example host with one Ethernet port and a traffic generator that
// generates request-reply traffic directly over Ethernet. This host model
// does not contain higher layer protocols (IP, TCP). By default it is
// configured to use half-duplex MAC (CSMA/CD).
//
module EtherHost
{
    parameters:
        bool hasStatus = default(false);
        bool csmacdSupport = default(true);  // by default use CSMA/CD
        string macType = default(csmacdSupport ? "EtherMAC" : "EtherMACFullDuplex"); // ~EtherMAC or ~EtherMACFullDuplex
        string queueType = default(""); // DropTailQueue, DiffServQueue, or empty to use the internal queue
        @networkNode();
        @labels(node,ethernet-node);
        @display("i=device/pc2");
        *.interfaceTableModule = default("^.interfaceTable");
    gates:
        inout ethg @labels(EtherFrame-conn);
    submodules:
        status: NodeStatus if hasStatus {
            @display("p=50,50;is=s");
        }
        cli: EtherAppCli {
            parameters:
                registerSAP = true;
                @display("p=120,58,col");
        }
        srv: EtherAppSrv {
            parameters:
                registerSAP = true;
                @display("p=310,58,col");
        }
        ll: MessageDispatcher {
            parameters:
                @display("p=215,152");
        }
        eth0: EthernetInterface {
            parameters:
                @display("p=215,255;q=l2queue");
        }
        interfaceTable: InterfaceTable {
            @display("p=50,186");
        }
    connections:

        cli.out --> ll.upperLayerIn++;
        cli.in <-- ll.upperLayerOut++;
        srv.out --> ll.upperLayerIn++;
        srv.in <-- ll.upperLayerOut++;

        ll.lowerLayerOut++ --> eth0.upperLayerIn;
        eth0.upperLayerOut --> ll.lowerLayerIn++;
        eth0.phys <--> ethg;
}
