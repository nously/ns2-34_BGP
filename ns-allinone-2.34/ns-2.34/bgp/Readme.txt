ns-BGP 2.0
==========

ns-BGP is an implementation of BGP-4 for the Network Simulator ns-2 
(http://www.isi.edu/nsnam/ns/).

ns-BGP was developed by porting the BGP-4 model of SSFNet into ns-2. SSFNet
(http://www.ssfnet.org) is a Java-based simulator for modeling large 
communication networks. The implementation of the BGP-4 model in the SSFNet 
is called SSF.OS.BGP4 (http://www.ssfnet.org/bgp/doc/). ns-BGP tries to
maintain most of the SSF.OS.BGP4 functionality while it incorporates the
model into the ns-2 simulation environment.

ns-BGP 2.0 was developed for ns-2 version 2.27 in 2003. In 2008, it was intregrated and modified by Will Hrudey to compile successfully with ns-2.33 on Fedora Core 8. 



Installing ns-2
===============

Download ns-2 version 2.33 from the ns-2 homepage
http://www.isi.edu/nsnam/dist/ns-allinone-2.33.tar.gz

In the sections that follow, we will assume that you install ns-allinone in
your home directory (~).

Unpack the ns package in your home directory:

  > cd ~
  > tar xzvf ns-allinone-2.33.tar.gz

This will create an "ns-allinone-2.33" subdirectory with all required files
and subdirectories below it.

If you want to use ns-BGP in ns-2, you should install ns-BGP at this point.
Follow the installation instructions in the next section. Then run the ns-2
installation script:

  > cd ~/ns-allinone-2.33
  > ./install

When the installation script finishes, set environment variables as
instructed by the ns-2 install program.



Installing ns-BGP
=================

Download ns-BGP version 2.0 from the ns-BGP homepage
(http://www.ensc.sfu.ca/cnl/ns-bgp)

Unpack it in the " ns-allinone-2.33" directory:

  > cd ~/ns-allinone-2.33
  > tar zxvf ns-2.33-bgp_2.0.tar.gz

This will
  - create a patch file "ns-2.33-bgp_2.0.patch" in the "ns-allinone-2.33" 
    subdirecotry;
  - create a "bgp" subdirectory with all C++ files for BGP under the "ns-2.33"
    subdirectory;
  - add new C++ files for TcpSocket under the "ns-2.33/tcp" subdirectory;
  - create a "bgp" subdirectory with all TCL files for BGP under the 
    "ns-2.33/tcl" subdirectory.

Apply the ns-BGP patch:

  > cd ~/ns-allinone-2.33
  > patch -p0 < ns-2.33-bgp_2.0.patch



Recompiling ns-2
================

If you are installing ns-BGP into an existing, already compiled ns-2
installation, ns-2 needs to be recompiled before the changes take effect:

  > cd ~/ns-allinone-2.33/ns-2.33
  > ./configure
  > make clean
  > make



Using ns-BGP in simulations
===========================

In order to use ns-BGP, turn on the BGP option with the node-config command
in your OTcl scripts before creating any BGP nodes:

  $ns node-config -BGP ON
  set n0 [$ns node 0:10.0.0.1]         ;# creates a BGP node with AS number 0
  		    				   ;# and IP address 10.0.0.1
  set n1 [$ns node 1:10.0.1.1]
  set n2 [$ns node 1:10.0.2.1]
  $ns node-config -BGP OFF



Configuring ns-BGP
==================

Each BGP routing agent instance (one per BGP node) can be configured
individually from OTcl:

  set bgp_agent0 [$n0 get-bgp-agent]	       ;# gets the BGP routing agent
  $bgp_agent0 bgp-id 10.0.0.1				  ;# sets the BGP ID
  $bgp_agent0 neighbor 10.0.1.1 remote-as 1     ;# establishes a BGP session
					         ;# with the router 10.0.1.1
  $bgp_agent0 neighbor 10.0.2.1 remote-as 2

The available configuration commands for a BGP routing agent are:

  $bgp_agent0 bgp-id 10.0.0.1			      	           ;# BGP ID
  $bgp_agent0 cluster-id 1000	      ;# cluster number for route reflection
  $bgp_agent0 connretry_time 120         ;# connection retrial time interval
  $bgp_agent0 min_as_orig_time 15 	 ; # Minimum AS Origination Interval
  $bgp_agent0 set-auto-config
  $bgp_agent0 neighbor $ipaddress $command $cmd-value

The "set-auto-config" command enables a BGP routing agent to automatically
find its potential BGP peers and configure these peers with default
parameter values. The "neighbor" command is used to configure each peer of
a BGP routing agent:

  $bgp_agent0 neighbor 10.0.1.1 remote-as 1
  $bgp_agent0 neighbor 10.0.1.1 route-reflector-client	   ;# sets this peer
					     ;# as a route reflection client
  $bgp_agent0 neighbor 10.0.1.1 hold-time 90	         ;# Hold Timer value
  $bgp_agent0 neighbor 10.0.1.1 keep-alive-time 30  ;# KeepAlive Timer value
  $bgp_agent0 neighbor 10.0.1.1 mrai 30			    ;# Minimum Route
				       ;# Advertisement Interval Timer value

Please note that no error checking is performed on the values of these
variables. Changing them to strange or disallowed values will probably yield
unexpected results. The default values can be found in the
"ns-2.33/tcl/lib/ns-default.tcl" file.

Besides individual configuration, we can also enable some BGP optional
features, such as SSLD (sender-side loop detection) and rate_limit_by_dest
(per-destination rate-limiting), in the "ns-2.33/bgp/global.h" file. These
settings in the global.h file will affect all BGP routing agents in an
ns-BGP simulation.

After the BGP routing agents are configured, three run-time commands can be
executed during ns-BGP simulations:

$bgp_agent0 network 10.0.0.0/24 	      ;# originates a network prefix
$bgp_agent0 no-network 10.0.0.0/24	       ;# withdraws a network prefix
$bgp_agent0 show-routes 	   ;# displays the content of routing tables

A set of validation test scripts can be found in the "ns-2.33/tcl/bgp/test"
directory.



Other notes and limitations
===========================

Route aggregation has not been tested thoroughly. Support for policy routing,
flap damping, and multi-protocol need to be added in the future.



Questions
=========

If you have any questions or bug reports pertaining to running ns-BGP in
ns-2, you're welcome to e-mail me:

  Tony Dongliang Feng <tdfeng@cs.sfu.ca> 

If you have any questions or concerns relating to the integration of ns-BGP in
ns-2.33, you're welcome to email me:

  Will Hrudey <whrudey@cs.sfu.ca>
