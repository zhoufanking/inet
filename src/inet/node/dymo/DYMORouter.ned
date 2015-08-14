//
// This program is property of its copyright holder. All rights reserved.
//

package inet.node.dymo;

import inet.node.inet.WirelessHost;
import inet.routing.dymo.DYMO;

module DYMORouter extends WirelessHost
{
    parameters:
        @display("i=device/cellphone");
        forwarding = true;
        wlan[*].mgmtType = default("Ieee80211MgmtAdhoc");
        dymo.routingTableModule = default(absPath(".ipv4.routingTable"));
        dymo.networkProtocolModule = default(absPath(".ipv4.ip"));

    submodules:
        dymo4: DYMO if hasIpv4 {
            parameters:
                routingTableModule = "^.ipv4.routingTable";
                networkProtocolModule = "^.ipv4.ip";
                @display("p=700,300");
        }
        dymo6: DYMO  if hasIpv6{
            parameters:
                routingTableModule = "^.ipv6.routingTable";
                networkProtocolModule = "^.ipv6.ipv6";
                @display("p=750,300");
        }
        dymoGn: DYMO  if hasGn{
            parameters:
                routingTableModule = "^.gn.routingTable";
                networkProtocolModule = "^.gn.gnp";
                @display("p=800,300");
        }

    connections:
        dymo4.ipOut --> tn.upperLayerIn++ if hasIpv4;
        dymo4.ipIn <-- tn.upperLayerOut++ if hasIpv4;

        dymo6.ipOut --> tn.upperLayerIn++ if hasIpv6;
        dymo6.ipIn <-- tn.upperLayerOut++ if hasIpv6;

        dymoGn.ipOut --> tn.upperLayerIn++ if hasGn;
        dymoGn.ipIn <-- tn.upperLayerOut++ if hasGn;
}
