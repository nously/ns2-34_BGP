set nf [open out.nam w]
set ns [new Simulator]

proc finish {} {
	global nf ns
	$ns flush-trace
	close $nf
	exec nam out.nam &
	exit 0
}

$ns node-config -BGP ON
set n0 [$ns node 0:10.0.0.1]
set n1 [$ns node 1:10.0.1.1] 
set n2 [$ns node 1:10.0.2.1]
$ns node-config -BGP OFF

set bgp_agent0 [$n0 get-bgp-agent]
$bgp_agent0 bgp-id 10.0.0.1
$bgp_agent0 neighbor 10.0.1.1 remote-as 1
$bgp_agent0 neighbor 10.0.2.1 remote-as 2


$ns at 10 "finish"
