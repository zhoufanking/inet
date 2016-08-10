//
// Copyright (C) OpenSim Ltd
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

package inet.physicallayer.idealradio;

import inet.physicallayer.base.packetlevel.PhysicalLayerBase;
import inet.physicallayer.contract.packetlevel.IRadio;

//
// This module implements an idealized radio that completely bypasses the
// physical medium. Packets received from the upper layer protocols are never
// lost. This radio module directly sends packets to the other radio module
// without any physical layer processing in the radio medium. Physical layer
// overhead is simply simulated by physical header bits, preamble transmission
// duration and a propagation delay.
//
simple IdealRadio extends PhysicalLayerBase like IRadio
{
    parameters:
        string interfaceTableModule;
        double bitrate @unit(bps);
        double preambleDuration @unit(s) = default(0s); // extra preamble transmission duration 
        int headerBitLength @unit(b) = default(0b); // bit length of physical header added to packets
        volatile double propagationDelay @unit(s) = default(0s); // propagation time to all destinations
        @class(IdealRadio);
}
