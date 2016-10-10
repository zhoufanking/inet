/**
@mainpage Configurator Tutorial for the INET Framework

This tutorial will show how to use the IPv4NetworkConfigurator module to configure IP addresses and routing in wired and wireless IPv4 networks in the INET framework.
The tutorial is organized into multiple steps, each corresponding to a simulation model. The steps demonstrate how to accomplish certain
tasks with the configurator.

This is an advanced tutorial, and assumes that the reader is familiar with creating and running simulations in @opp and INET. If that wasn't the case,
the TicToc Tutorial is a good starting point to get started with @opp. The INET Walkthrough is an introduction to INET and how to work with protocols.
The Wireless Tutorial is another advanced tutorial, and it deals with wireless features of the INET framework.

For additional information, the following documentation should be useful:

- <a href="https://omnetpp.org/doc/omnetpp/manual/usman.html" target="_blank">@opp User Manual</a>
- <a href="https://omnetpp.org/doc/omnetpp/api/index.html" target="_blank">@opp API Reference</a>
- <a href="https://omnetpp.org/doc/inet/api-current/inet-manual-draft.pdf" target="_blank">INET Manual draft</a>
- <a href="https://omnetpp.org/doc/inet/api-current/neddoc/index.html" target="_blank">INET Reference</a>

All simulation models are defined in the omnetpp.ini file as separate configurations. The configurations use a number of different networks,
defined in separate .NED files.

@section contents Contents

 - @ref step1

@nav{index,step1}

<!-------------------------------------------------------------------------------------------------------->

@page step1 Step 1 - Fully automatic IP address assignment

@section s1goals Goals

The goal of this step is to demonstrate that in many scenarios, the configurator can adequatelly configure the network with its default settings, without
any user input. This is useful when it is irrelevant what the nodes' IP addresses are in a simulation, because the goal is to study wireless
transmission ranges, for example. The default settings 'just work'.

@section s1model The model

<strong>About the configurator</strong>

*/