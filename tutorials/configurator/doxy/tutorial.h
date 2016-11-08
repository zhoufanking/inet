/**
@mainpage IPv4Configurator Tutorial for the INET Framework

This tutorial will show how to use the <tt>IPv4NetworkConfigurator</tt> module to configure IP addresses and routing tables in wired and wireless IPv4 networks in the INET framework.
The tutorial is organized into multiple steps, each corresponding to a simulation model. The steps demonstrate how to accomplish certain
tasks with the configurator.

This is an advanced tutorial, and assumes that the reader is familiar with creating and running simulations in @opp and INET. If that wasn't the case,
the <a href="https://omnetpp.org/doc/omnetpp/tictoc-tutorial/"
target="_blank">TicToc Tutorial</a> is a good starting point to get familiar with @opp. The <a
href="../../../doc/walkthrough/tutorial.html" target="_blank">INET Walkthrough</a> is an introduction to INET and how to work with protocols.
The <a href="https://omnetpp.org/doc/inet/api-current/tutorials/wireless/" target="_blank">Wireless Tutorial</a> is another advanced tutorial, and deals with wireless features of the INET framework. There is a comprehensive description of the configurator's features in the <a href="https://omnetpp.org/doc/inet/api-current/neddoc/index.html?p=inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator.html" target="_blank"><tt>IPv4NetworkConfigurator</tt> NED documentation</a>
in the INET reference.

For additional information, the following documentation should be useful:

- <a href="https://omnetpp.org/doc/omnetpp/manual/usman.html" target="_blank">@opp User Manual</a>
- <a href="https://omnetpp.org/doc/omnetpp/api/index.html" target="_blank">@opp API Reference</a>
- <a href="https://omnetpp.org/doc/inet/api-current/inet-manual-draft.pdf" target="_blank">INET Manual draft</a>
- <a href="https://omnetpp.org/doc/inet/api-current/neddoc/index.html" target="_blank">INET Reference</a>

All simulation models are defined in the omnetpp.ini file as separate configurations. The configurations use a number of different networks,
defined in separate .NED files.

@section contents Contents

 - @ref step1
 - @ref step2
 - @ref step3
 - @ref step4
 - @ref step5

@nav{index,step1}

<!-------------------------------------------------------------------------------------------------------->

@page step1 Step 1 - Fully automatic IP address assignment

@section s1goals Goals

The goal of this step is to demonstrate that in many scenarios, the configurator can properly configure IP addresses in a network with its default settings, without
any user input. This is useful when it is irrelevant what the nodes' actual IP addresses are in a simulation, because the goal is to study wireless
transmission ranges, for example
(this step deals with IP addresses only, routing will be discussed in later steps).

@section s1model The model

@subsection s1about  About the configurator

In INET simulations, configurator modules are responsible for assigning IP addresses to network nodes, and for setting up their
routing tables. Essentially, the configurator modules simulate a real life network administrator. There are various configurator
models in INET (<tt>IPv4NetworkConfigurator</tt>, <tt>FlatNetworkConfigurator</tt>, etc.), but this tutorial is about the features of the <tt>IPv4NetworkConfigurator</tt>, 
which we will refer to as <strong>configurator</strong>. The following is a broad review of the configurator's features and operation,
these and others will be discussed in detail in the following steps.

The configurator assigns IP addresses to interfaces, and sets up static routing in IPv4 networks.
It doesn't configure IP addresses and routes directly, but stores the configuration in its internal data structures.
Network nodes contain an instance of <tt>IPv4NodeConfigurator</tt>, which configures the corresponding node's interface table and routing table
based on information contained in the global <tt>IPv4NetworkConfigurator</tt> module.

The configurator supports automatic and manual network configuration, and their combination. By default,
the configuration is fully automatic, but the user can specify parts (or all) of the configuration manually, and the rest
will be configured automatically by the configurator. The configurator's various features can be turned on and off with NED parameters. The details of the configuration, such as IP addresses and routes, can be specified in an xml configuration file.

@subsection s1configuration The configuration

The configuration for this step uses the <i>ConfiguratorA</i> network, defined in <i>ConfiguratorA.ned</i>.
The network looks like this:

<img src="step1network.png">

The network contains 3 routers, each connected to the other two. There are 3 subnetworks with <tt>StandardHosts</tt>, connected to the routers by ethernet switches.
It also contains an instance of <tt>IPv4NetworkConfigurator</tt>.

The configuration for this step in omnetpp.ini is the following: 

@dontinclude omnetpp.ini
@skip Step1
@until ####

The configuration for Step 1 is basically empty. The configurator configures addresses according to its default parameters, and using the default xml configuration.

The default parameters pertaining to IP address assignment are the following:

<pre>
assignAddresses = default(true);
assignDisjunctSubnetAddresses = default(true);
</pre>

- <strong>assignAddresses = true</strong> tells the configurator to assign IP addresses to interfaces. It assigns addresses based on the supplied xml configuration,
or the default xml configuration if none is specified. Since no xml configuration is specified in this step, it uses the default configuration.

- <strong>assignDisjunctSubnetAddresses = true</strong> sets that the configurator should assign different address prefixes and netmasks
to nodes on different links.

Additionally, the <strong>dumpAddresses</strong> parameter sets whether the configurator prints assigned IP addresses to the module output.
This is false by default, but it's set to true in the <i>General</i> configuration at the begining of omnetpp.ini (along with other settings, which
will be discussed later).

@dontinclude omnetpp.ini
@skipline General
@until ####

An XML configuration can be supplied with the <i>config</i> parameter. When the user doesn't specify an xml configuration,
the configurator will use the following default configuration:

<code>
config = default(xml("<config><interface hosts='**' address='10.x.x.x' netmask='255.x.x.x'/></config>"));
</code>

The default xml configuration tells the configurator to assign IP addresses to all interfaces of all hosts, 
from the IP range 10.0.0.0 - 10.255.255.255 and netmask range 255.0.0.0 - 255.255.255.255.

@section s1results Results

The IP addresses assigned to interfaces by the configurator are shown on the image below.
The switches and hosts connected to the individual routers are considered to be one the same link.
Note that the configurator assigned addresses sequentially starting from 10.0.0.1, while making sure that different subnets got different address prefixes and netmasks,
as instructed by the <strong>assignDisjunctSubnetAddresses</strong> parameter.

<img src="step1addresses.png" width=850px>

@nav{index,step2}
@fixupini

<!-------------------------------------------------------------------------------------------------------->

@page step2 Step 2 - Manually overriding individual IP addresses

@nav{step1,step3}

@section s2goals Goals

Sometimes the user may want to specify the IP addresses of some nodes with special purpose in the network and leave the rest for the automatic configuration. This helps remembering IP addresses of said nodes. This step demonstrates manually specifying individual IP addresses.

@section s2model The model

This step uses the <i>ConfiguratorA</i> network from the previous step. We will assign the 10.0.0.50 address to <i>host1</i>
and 10.0.0.100 to <i>host3</i>. The configurator will automatically assign addresses to the rest of the nodes.

@subsection s2config Configuration

The configuration in omnetpp.ini for this step is the following:

@dontinclude omnetpp.ini
@skip Step2
@until ####

The xml configuration can be supplied to the <i>config</i> parameter in one of two ways:

- Inline xml using the <i>xml()</i> function. The argument of the function is the xml code.
- External xml file using the <i>xmldoc()</i> function. The argument of the function is the name of the xml configuration file.

In this step, the xml configuration is supplied to the configurator as inline xml. Xml configurations contain one <i><config></i> element. Under this root element there can be
multiple configuration elements, such as the <i><interface></i> elements here.
The <interface> element can contain selector attributes, which limit the scope of what interfaces are affected by the <interface> element.
Multiple interfaces can be selected with one <interface> element using the * wildcard.
They can also contain parameter attributes, which deal with what parameters those selected interfaces will have, like IP addresses and
netmasks. Address templates can be specified with one more 'x' in the address. The 'x' in the IP address and netmask signify that the value is not fixed, but the configurator should choose it automatically.
With these address templates it is possible to leave everything to the configurator or specify everything, and anything in between.
<! how about optional selectors and parameters>

In the XML configuration for this step, the first two rules state that host3's (hosts="*.host3") interface named 'eth0' (names="eth0") should get the IP address 10.0.0.100 (address="10.0.0.100"), and host1's interface 'eth0' should get IP 10.0.0.50.
The third rule is the default configuration, which tells the configurator to assign the rest of the addresses automatically.

Note that the configuration is processed sequentially, thus the order of the configuration elements is important.
Also, when there is a supplied configuration, and it doesnt specify all the addresses, the entry for the default configuration must be included in order for the configurator to assign addresses to all interfaces. For the manual address assignment rules to take effect,
the default configuration should be after the manual entries. This way the configurator first assigns the manually specified addresses,
and then automatically assigns the rest. 

Note that the order of configuration elements is important, but the configurator doesn't assign addresses in the order of xml statements. It iterates
interfaces, and for each interface the first matching rule in the xml configuration will take effect. Thus, the statements that are positioned earlier in the configuration take precedence over those that come later. When an xml configuration is supplied, it must contain address assignment statements in order for
the addresses to be assigned. 

When there is an xml configuration supplied, the configurator will assign addresses according to the address assignment statements in the xml configuration.
To have the configurator automatically assign addresses, the rule from the default configuration has to be included.
Even if the xml configuration contains user specified address assignments, the default rule should be included <strong>after</strong> the user defined ones
(so the user defined ones take effect first, and the rest of the addresses are assigned automatically). This is unless the intention is to leave some
interfaces unassigned.

@section s2results Results

The assigned addresses are shown in the following image.

<img src="step2address.png" width=850px>

As in the previous step, the configurator assigned disjunct subnet addresses. Note that the configurator still assigned addresses sequentially,
that is after setting the 10.0.0.100 address to <i>host3</i>, it didnt go back to the beginning of the address pool when assigning the
remaining addresses.

@nav{step1,step3}
@fixupini

<!-------------------------------------------------------------------------------------------------------->

@page step3 Step 3 - Automatically assigning IP addresses to a subnet from a given range

@nav{step2,step4}

@section s3goals Goals

Complex networks often contain several subnetworks, and the user may want to assign specific IP address ranges for them.
This can make it easier to tell them apart when looking at the IP addresses.
This step demonstrates how to assign a range of IP addresses to subnets with xml configuration templates.

@section s3model The model

This step uses the <i>ConfiguratorA</i> network, as in the previous two steps.
The three hosts connected to the router through the switch will be on the same subnet, and there are 3 such groups in the network.


The configuration is the following:

@dontinclude omnetpp.ini
@skipline Step3
@until ####

The xml configuration is supplied in an external file (step3.xml), using the xmldoc function:

@include step3.xml

- The first 3 lines assign IP addresses with different network prefixes to hosts in the 3 different subnets.

- The <i>towards</i> selector can be used to easily select the interfaces that are connected towards a certain host (or set of hosts using wildcards).
The next 3 entries specify that each router's interface that connects to the subnet should belong in that subnet.

- The last entry sets the network network prefix of interfaces of all routers to be 10.1. Since the addresses for the interfaces
connected towards the hosts are already specified by a previous entry, this effects only the rest of interfaces, those facing the other routers.

The same effect can be achieved in more than one way. Here is an alternative xml configuration (step3alt.xml) that results in the same address assignment:

@include step3alt.xml

The <i>among</i> selector selects the interfaces of the specified hosts towards the specified hosts (the statement <i>among="X Y Z"</i> is the same as
<i>host="X Y Z" towards="X Y Z"</i>). The rules containing the towards statements can be merged into the rules that assigns addresses to the hosts in the subnets,
as they have the same address templates.

@section s3results Results

The assigned addresses are shown on the following image.

<img src="step3address.png" width=850px>

The addresses are assigned as intended.

@nav{step2,step4}
@fixupini

<!-------------------------------------------------------------------------------------------------------->

@page step4 Step 4 - Fully automatic static routing table configuration

@nav{step3,step5}

@section s4goals Goals

Just as with IP addresses, in many cases the configurator configures routes in a network properly without any user input.
This step demonstrates the default configuration for routing.

@section s4model The model

This step uses the <i>ConfiguratorB</i> network, defined in ConfiguratorB.ned. This extends the previous network, <i>ConfiguratorA</i> with
the addition of a <tt>RoutingTableCanvasVisualizer</tt> module.

@dontinclude ConfiguratorB.ned
@skip ConfiguratorB
@until ####

<img src="step4network.png">

@subsection s4config Configuration

The configuration for this step in omnetpp.ini is the following:

@dontinclude omnetpp.ini
@skip Step4
@until ####

A ping app in <i>host0</i> is configured to send ping packets to <i>host7</i>, which is on the other side of the network.
This should aid in visualizing routing.

The <i>RoutingTableCanvasVisualizer</i> module can be used to visualize routes in the network.
Routes are visualized with arrows. In general, an arrow signifies an entry in the source host's routing table. It points
to the host that is the next hop or gateway for that routing table entry.
Routes to be visualized are selected with the visualizer's <i>destinationFilter</i> parameter.
All routes leading towards that destination are indicated by arrows.
The default setting is <tt>""</tt>, which means no routes are visualized. The <tt>"*.*"</tt> setting visualizes all routes
going from every node to every other node, which can make the screen cluttered.
In this step the <i>destinationFilter</i> is set to visualize all routes heading towards <i>host7</i>.

The IP address assignment is fully automatic, and the resulting addresses should be the same as in Step 1.

@subsection s4defaults Configurator routing parameters

The configurator's default parameters concerning routing are the following:

<pre>
addStaticRoutes = default(true)
addDefaultRoutes = default(true)
addSubnetRoutes = default(true)
optimizeRoutes = default(true)
</pre>

The configuration for this step didn't set any of these parameters, thus the default values will take effect.

- <i>addStaticRoutes</i>: the configurator adds static routes to the routing table of all nodes in the network, 
with routes leading to all destination interfaces. This should be turned off when the xml configuration contains
manual routes.
- <i>addDefaultRoutes</i>: Add a default route if all routes from a node go through the same gateway.
This is often the case with hosts, which usually connect to a network via a single interface. This parameter
is not used if <i>addStaticRoutes = false</i>.
- <i>addSubnetRoutes</i>: Reach nodes in own subnet directly (ie. the gateway is the same as the destination), rather than with separate destination interface routes. This is only used where applicable, and not used if <i>addStaticRoutes = false</i>.
- <i>optimizeRoutes</i>: Optimize routing tables by merging entries where possible. Not used if <i>addStaticRoutes = false</i>.

Additionally, the <i>dumpTopology</i>, <i>dumpLinks</i> and <i>dumpRoutes</i> parameters are set to true in the <i>General</i> configuration.
These instruct the configurator to print to the module output the topology of the network, the recognized network links, and the routing tables of all nodes, respectively. Topology describes which nodes are connected to which nodes. Hosts that can directly reach each other (i.e. the next hop is the destination), 
are considered to be on the same link.

@dontinclude omnetpp.ini
@skip General
@until ####

The <i>General</i> configuration also sets GlobalARP to keep the packet exchanges simpler. GlobalARP fills the ARP tables of all nodes in advance,
so when the simulation begins no ARP exchanges are necessary. The <i>**.routingTable.netmaskRoutes = ""</i> keeps the routing table modules from
adding netmask routes to the routing tables. Netmask routes mean that nodes with the same netmask but different IP should reach each other directly.
These routes are also added by the configurator, so netmaskRoutes are turned off for the sake of simplicity.

@section s4result Results

The visualized routes are displayed on the following image:

<img src="step4routes.png" width=850px>

Note that routes from all nodes to host7 are visualized.

The routing tables are the following (routes visualized on the image above are highlighted with red):

@htmlonly
<div class="fragment">
<pre class="monospace">
Node ConfiguratorB.host0 (hosts 1-2 similar)
-- Routing table --
Destination      Netmask          Gateway          Iface           Metric
10.0.0.0         255.255.255.248  *                eth0 (10.0.0.1) 0
<span class="marker">*                *                10.0.0.4         eth0 (10.0.0.1) 0</span>

Node ConfiguratorB.host3 (hosts 4-5 similar)
-- Routing table --
Destination      Netmask          Gateway          Iface           Metric
10.0.0.8         255.255.255.248  *                eth0 (10.0.0.9) 0
<span class="marker">*                *                10.0.0.10        eth0 (10.0.0.9) 0</span>

Node ConfiguratorB.host6 (hosts 7-8 similar)
-- Routing table --
Destination      Netmask          Gateway          Iface            Metric
<span class="marker">10.0.0.32        255.255.255.248  *                eth0 (10.0.0.34) 0</span>
<i></i>*                *                10.0.0.33        eth0 (10.0.0.34) 0

Node ConfiguratorB.router0
-- Routing table --
Destination      Netmask          Gateway          Iface            Metric
10.0.0.18        255.255.255.255  *                eth1 (10.0.0.17) 0
10.0.0.22        255.255.255.255  *                eth2 (10.0.0.21) 0
<span class="marker">10.0.0.25        255.255.255.255  10.0.0.22        eth2 (10.0.0.21) 0</span>
10.0.0.0         255.255.255.248  *                eth0 (10.0.0.4)  0
<span class="marker">10.0.0.32        255.255.255.248  10.0.0.22        eth2 (10.0.0.21) 0</span>
10.0.0.0         255.255.255.224  10.0.0.18        eth1 (10.0.0.17) 0

Node ConfiguratorB.router1
-- Routing table --
Destination      Netmask          Gateway          Iface            Metric
10.0.0.17        255.255.255.255  *                eth0 (10.0.0.18) 0
<span class="marker">10.0.0.22        255.255.255.255  10.0.0.25        eth2 (10.0.0.26) 0</span>
10.0.0.25        255.255.255.255  *                eth2 (10.0.0.26) 0
10.0.0.8         255.255.255.248  *                eth1 (10.0.0.10) 0
<span class="marker">10.0.0.32        255.255.255.248  10.0.0.25        eth2 (10.0.0.26) 0</span>
10.0.0.0         255.255.255.224  10.0.0.17        eth0 (10.0.0.18) 0

Node ConfiguratorB.router2
-- Routing table --
Destination      Netmask          Gateway          Iface            Metric
10.0.0.18        255.255.255.255  10.0.0.26        eth0 (10.0.0.25) 0
10.0.0.21        255.255.255.255  *                eth2 (10.0.0.22) 0
10.0.0.26        255.255.255.255  *                eth0 (10.0.0.25) 0
10.0.0.8         255.255.255.248  10.0.0.26        eth0 (10.0.0.25) 0
<span class="marker">10.0.0.32        255.255.255.248  *                eth1 (10.0.0.33) 0</span>
10.0.0.0         255.255.255.224  10.0.0.21        eth2 (10.0.0.22) 0
</pre>
</div>
@endhtmlonly

The * for the gateway means that the gateway is the same as the destination. Hosts have a routing table entry to reach other nodes which are on the same subnet directly. They also have a default route with the router as
the gateway for packets sent to outside-of-subnet addresses. Routers have 3 rules in their routing tables for reaching the other routers,
specifically, those interfaces of the other routers that are not facing the hosts.

@nav{step3,step5}
@fixupini

<!-------------------------------------------------------------------------------------------------------->

@page step5 Step 5 - Manually overriding individual routes

@nav{step4,step6}

@section s5goals Goals

The automatic configuration can configure routes properly, but sometimes the user might want to manually override some of the routes.
This step has two parts:

- In the <strong>A</strong> part we will override the routes to just one specific host
- In the <strong>B</strong> part we will override routes to a set of hosts

@section s5a Part A - Overriding routes to a specific host

In this part we will override the routes going from the subnet of <i>router0</i> to <i>host7</i>. With the automatic configuration, packets
from router0's subnet would go through router2 to reach host7 (as in the previous step). We want them to go through router1 instead.

<img src="step1network.png">

@subsection s5aconfig Configuration

This configuration uses the same network as the previous step, ConfiguratorB. The configuration in omnetpp.ini is the following:

@dontinclude omnetpp.ini
@skipline Step5
@until ####

For the routes to go through <i>router1</i>, the routing table of <i>router0</i> has to be altered.
The new rules should dictate that packets with the destination of host7 (10.0.0.35) should be routed
towards <i>router2</i>. The XML configuration in step5a.xml:

@dontinclude step5a.xml
@skipline <config>
@until </config>

v1
The <route> element describes routing table entries for one or more nodes in the network.
As with <interface>, selector attributes specify which nodes are affected by the <route> element,
and parameter attributes specify the details of the routing table entry.

v2
The <route> element describes a routing table entry for one or more nodes in the network.
The hosts selector attribute specifies which hosts' routing tables should contain the entry.
There are 5 parameter attributes, that are optional. These are the same as in real life routing tables:
address, netmask, gateway, interface, metric.

The <route> element in this XML configuration adds the following rule to <i>router0's</i> routing table:
Packets with the destination of 10.0.0.35/32 should use the interface 'eth1' and the gateway 10.0.0.18 (router2).
<!should be more detailed>

@subsection s5aresults Results

The routing table of <i>router0</i>:

<div class="fragment">
<pre class="monospace">
Node ConfiguratorB.router0
-- Routing table --
Destination      Netmask          Gateway          Iface            Metric
10.0.0.18        255.255.255.255  *                eth1 (10.0.0.17) 0
10.0.0.22        255.255.255.255  *                eth2 (10.0.0.21) 0
10.0.0.25        255.255.255.255  10.0.0.22        eth2 (10.0.0.21) 0
<span class="marker">10.0.0.35        255.255.255.255  10.0.0.18        eth1 (10.0.0.17) 0</span>
10.0.0.0         255.255.255.248  *                eth0 (10.0.0.4) 	0
10.0.0.32        255.255.255.248  10.0.0.22        eth2 (10.0.0.21) 0
10.0.0.0         255.255.255.224  10.0.0.18        eth1 (10.0.0.17) 0
</pre>
</div>

The routing table of router0 in the last step had 6 entries. Now it has 7,
as the rule specified in the XML configuration has been added (highlighted with red).
<!should be different color than the highlight in the last step because it doesnt signify the visualized routes>

@section s5b Part B - Overriding routes to a set of hosts

In this part, we will override routes going from the subnet of hosts 0-2 to the subnet of hosts 6-8.
These routes will go through <i>router1</i>, just as in Part A.

@subsection s5bconfig Configuration

The configuration in omnetpp.ini:

@dontinclude omnetpp.ini
@skipline Step5B
@until ####

As in Part A, the routing table of <i>router0</i> has to be altered, so that packets to hosts 6-8 go towards <i>router1</i>. 
The XML configuration in step5b.xml:

@dontinclude step5b.xml
@skipline config
@until config

The <route> element specifies a routing table entry for <i>router0</i>. The destination is 10.0.0.32 with netmask 255.255.255.248,
which is the address of the subnet for hosts 6-8.

@subsection s5bresults Results

The routing table of <i>router0</i>:

<div class="fragment">
<pre class="monospace">
Node ConfiguratorB.router0
-- Routing table --
Destination      Netmask          Gateway          Iface            Metric
10.0.0.10        255.255.255.255  10.0.0.18        eth1 (10.0.0.17) 0
10.0.0.18        255.255.255.255  *                eth1 (10.0.0.17) 0
10.0.0.22        255.255.255.255  *                eth2 (10.0.0.21) 0
10.0.0.25        255.255.255.255  10.0.0.22        eth2 (10.0.0.21) 0
10.0.0.26        255.255.255.255  10.0.0.18        eth1 (10.0.0.17) 0
10.0.0.33        255.255.255.255  10.0.0.22        eth2 (10.0.0.21) 0
10.0.0.0         255.255.255.248  *                eth0 (10.0.0.4)  0
10.0.0.8         255.255.255.248  10.0.0.18        eth1 (10.0.0.17) 0
<span class="marker">10.0.0.32        255.255.255.248  10.0.0.18        eth1 (10.0.0.17) -1</span>
10.0.0.32        255.255.255.248  10.0.0.22        eth2 (10.0.0.21) 0
</pre>
</div>

@nav{step4,step6}
@fixupini

<!-------------------------------------------------------------------------------------------------------->

@page step6 Step 6 - Configuring metric for automatic routing table generation

@section s6goals Goals

By default, the network configurator uses the shorthest path algorithm with constant 1 cost function. It produces paths optimized for hop count.
Other cost functions are also available: bitrate, error rate, etc.???

When setting up routes, the configurator uses the shortest path algorithm. By default, paths are optimized for hop count.
However, there are other cost functions available, like data rate, error rate, etc. This step demonstrates using the data rate metric
for automatically setting up routes.

@section s6model The model

When setting up routes, the configurator first builds a graph representing the network topology. It will have vertices for every network device,
like hosts, routers, and L2 devices like switches, access points, and ethernet hubs. The graphs edges represent network connections. The configurator assigns weights to vertices and
edges, this is used by the shortest path algorithm to set up routes. Nodes that have IP forwarding disabled get infinite weight, and all others get
zero. This way routes will not transit nodes that have IP forwarding disabled.
Edge weights will be chosen according to the configured metric. Routes will be created optimised for this metric. The default metric is hop count, thus each edge gets a weight of 1.
The available metrics are "hopCount", "delay", "dataRate", and "errorRate".
When the graph is built and the weights are assigned, the configurator uses Dijkstra's shortest path algorithm to compute the routes.
<!rewrite>

@subsection s6config Configuration

The configuration for this step extends Step 4, thus it uses the ConfiguratorB network. The configuration in omnetpp.ini is the following:

@dontinclude omnetpp.ini
@skipline Step6
@until ####

The XML configuration contains the default rule for IP address assignment, and an <autoroute> element that configures the metric to be used.
The <autoroute> element specifies parameters for automatic static routing table generation. If no <autoroute> element is specified, the configurator
assumes a default that affects all routing tables in the network, and computes shortest paths to all interfaces according to the hop count metric.
Here the <autoroute> element specifies that routes should be added to the routing tables of all hosts (hosts="**") and the metric should be <i>dataRate</i> 
(metric="dataRate"). The configurator assigns weights to the graph's edges that are inversely proportional to the data rate of the network links.
This way generation will favor routes with higher data rates.

Note that <i>router0</i> and <i>router2</i> are connected with a 10 Mbit/s ethernet cable, while <i>router1</i> connects to the other routers with
100 Mbit/s ethernet cables. Since routes are optimized for data rate, packets from router0 to router2 will go via router1 as this path has more bandwidth.

The resulting routes are essentially same as in Step 5B, just realized with a different XML config (the difference is that in this step, no traffic is routed
between router0 and router2 at all. In Step 5B, packets to router2's eth2 interface were routed that way).

v1
<img src="step4routes_3.png">

v2
<img src="step6eth.png">

@section s6results Results

The following image shows the visualized routes towards <i>host7</i>.
Routes towards router2 go through router1, as opposed to the routes in Step 4.

<img src="s6routes.png" width=850px>

The routing tables are as follows:

@htmlonly
<div class="fragment">
<pre class="monospace">
Node ConfiguratorB.host0
-- Routing table --
Destination      Netmask          Gateway          Iface           Metric
10.0.0.0         255.255.255.248  *                eth0 (10.0.0.1) 0
<i></i>*                *                10.0.0.4         eth0 (10.0.0.1) 0

Node ConfiguratorB.host1
-- Routing table --
Destination      Netmask          Gateway          Iface           Metric
10.0.0.0         255.255.255.248  *                eth0 (10.0.0.3) 0
<i></i>*                *                10.0.0.4         eth0 (10.0.0.3) 0

Node ConfiguratorB.host2
-- Routing table --
Destination      Netmask          Gateway          Iface           Metric
10.0.0.0         255.255.255.248  *                eth0 (10.0.0.2) 0
<i></i>*                *                10.0.0.4         eth0 (10.0.0.2) 0

Node ConfiguratorB.host3
-- Routing table --
Destination      Netmask          Gateway          Iface           Metric
10.0.0.8         255.255.255.248  *                eth0 (10.0.0.9) 0
<i></i>*                *                10.0.0.10        eth0 (10.0.0.9) 0

Node ConfiguratorB.router0
-- Routing table --
Destination      Netmask          Gateway          Iface            Metric
10.0.0.18        255.255.255.255  *                eth1 (10.0.0.17) 0
10.0.0.0         255.255.255.248  *                eth0 (10.0.0.4) 	0
10.0.0.0         255.255.255.192  10.0.0.18        eth1 (10.0.0.17) 0

Node ConfiguratorB.router2
-- Routing table --
Destination      Netmask          Gateway          Iface            Metric
10.0.0.26        255.255.255.255  *                eth0 (10.0.0.25) 0
10.0.0.32        255.255.255.248  *                eth1 (10.0.0.33) 0
10.0.0.0         255.255.255.224  10.0.0.26        eth0 (10.0.0.25) 0

Node ConfiguratorB.router1
-- Routing table --
Destination      Netmask          Gateway          Iface            Metric
10.0.0.17        255.255.255.255  *                eth0 (10.0.0.18) 0
10.0.0.22        255.255.255.255  10.0.0.25        eth2 (10.0.0.26) 0
10.0.0.25        255.255.255.255  *                eth2 (10.0.0.26) 0
10.0.0.8         255.255.255.248  *                eth1 (10.0.0.10) 0
10.0.0.32        255.255.255.248  10.0.0.25        eth2 (10.0.0.26) 0
10.0.0.0         255.255.255.224  10.0.0.17        eth0 (10.0.0.18) 0

Node ConfiguratorB.host4
-- Routing table --
Destination      Netmask          Gateway          Iface            Metric
10.0.0.8         255.255.255.248  *                eth0 (10.0.0.11) 0
<i></i>*                *                10.0.0.10        eth0 (10.0.0.11) 0

Node ConfiguratorB.host5
-- Routing table --
Destination      Netmask          Gateway          Iface            Metric
10.0.0.8         255.255.255.248  *                eth0 (10.0.0.12) 0
<i></i>*                *                10.0.0.10        eth0 (10.0.0.12) 0

Node ConfiguratorB.host6
-- Routing table --
Destination      Netmask          Gateway          Iface            Metric
10.0.0.32        255.255.255.248  *                eth0 (10.0.0.34) 0
<i></i>*                *                10.0.0.33        eth0 (10.0.0.34) 0

Node ConfiguratorB.host7
-- Routing table --
Destination      Netmask          Gateway          Iface            Metric
10.0.0.32        255.255.255.248  *                eth0 (10.0.0.35) 0
<i></i>*                *                10.0.0.33        eth0 (10.0.0.35) 0

Node ConfiguratorB.host8
-- Routing table --
Destination      Netmask          Gateway          Iface            Metric
10.0.0.32        255.255.255.248  *                eth0 (10.0.0.36) 0
<i></i>*                *                10.0.0.33        eth0 (10.0.0.36) 0
</pre>
</div>
@endhtmlonly

<!v2 - only router0's routing table>

The routing table of <i>router0</i> is as follows:

<div class="fragment">
<pre class="monospace">
Node ConfiguratorB.router0
-- Routing table --
Destination      Netmask          Gateway          Iface            Metric
10.0.0.18        255.255.255.255  *                eth1 (10.0.0.17) 0
10.0.0.0         255.255.255.248  *                eth0 (10.0.0.4) 	0
10.0.0.0         255.255.255.192  10.0.0.18        eth1 (10.0.0.17) 0
</pre>
</div>

The last rule describes that traffic that is not destined for <i>router0's</i> subnet or <i>router2</i> should be routed towards <i>router2</i>,
via the 100Mbps link.

One can easily check that no routes are going through the link between router0 and router2 by setting the destination filter to "*.*" in the visualizer.
This indicates all routes in the network:

<img src="step6allroutes.png" width=850px>

@endhtmlonly

@fixupini

*/