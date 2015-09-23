//
// Copyright (C) 2015 OpenSim Ltd.
//



package inet.examples.inet.icmptest;

import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;


network IcmpTest
{
    @display("bgb=651,322");
    types:
        channel ethernetline extends ThruputMeteringChannel
        {
            delay = 0.1us;
            datarate = 100Mbps;
            thruputDisplayFormat = "u";
        }
    submodules:
        configurator: IPv4NetworkConfigurator {
            parameters:
                config = xmldoc("network.xml");
                addDefaultRoutes = false;
                addStaticRoutes = false;
                addSubnetRoutes = false;
                optimizeRoutes = false;
                @display("p=48,50");
        }
        cli: StandardHost {
            parameters:
                @display("i=device/laptop;p=131,194");
        }
        r1: Router {
            parameters:
                @display("p=260,250");
        }
        r2: Router {
            parameters:
                @display("p=405,250");
        }
        srv: StandardHost {
            parameters:
                @display("p=544,194;i=device/server_l");
        }
    connections:
        cli.ethg++ <--> ethernetline <--> r1.ethg++;
        r1.ethg++ <--> ethernetline <--> r2.ethg++;
        r2.ethg++ <--> ethernetline <--> srv.ethg++;
}

