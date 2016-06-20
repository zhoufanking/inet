//
// Copyright (C) 2005 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

package inet.samples.handover;

import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.inet.WirelessHost;
import inet.node.wireless.AccessPoint;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.visualizer.integrated.IntegratedCanvasVisualizer;

// TODO: rename this module to HandoverExample, all example networks should be called SomethingExample to make searching for examples easy
network HandoverExample
{
    parameters:
	    @display("bgb=640,370");
    submodules:
        visualizer: IntegratedCanvasVisualizer {
            parameters:
                @display("p=71,100");
        }
        radioMedium: Ieee80211ScalarRadioMedium {
            parameters:
                // TODO: y positions should be a multiply of 100, it makes displaying two status text lines above the submodule possible
                // TODO: x positions should be 100 for consistency
                @display("p=70,30");
        }
        host: WirelessHost {
            parameters:
                // TODO: update vertical positions to avoid overlapping
                @display("p=50,230;r=,,#707070");
        }
        ap1: AccessPoint {
            parameters:
                @display("p=100,300;r=,,#707070");
        }
        ap2: AccessPoint {
            parameters:
                @display("p=500,300;r=,,#707070");
        }
}

