//
// Copyright (C) OpenSim Ltd.
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

package inet.linklayer.ideal;

import inet.linklayer.base.MACProtocolBase;
import inet.linklayer.contract.ILinkLayer;
import inet.linklayer.contract.IMACProtocol;

//
// This module implements an idealized MAC protocol that completely bypasses
// the physical layer. Packets received from the upper layer protocols are
// never lost. Theis MAC protocol directly sends packets to the destination
// MAC protocol without any physical layer processing. Physical layer overhead
// is simply simulated by overhead bits, overhead transmission duration and
// a propagation delay.
//
simple IdealMac extends MACProtocolBase like ILinkLayer, IMACProtocol
{
    parameters:
        string address = default("auto");   // MAC address as hex string (12 hex digits), or
                                            // "auto". "auto" values will be replaced by
                                            // a generated MAC address in init stage 0.
        double bitrate @unit(bps);
        volatile int overheadBitLength @unit(b) = default(0b); // extra bits added to the packet before transmission
        volatile double overheadDuration @unit(s) = default(0s); // extra transmission duration overhead
		volatile double propagationDelay @unit(s) = default(0s); // propagation time to all destinations
        @class(IdealMac);
        @display("i=block/rxtx");
    gates:
        input peerIn;
}
