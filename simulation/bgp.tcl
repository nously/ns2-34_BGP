set nf [open out.nam w]
set ns [new Simulator]
$ns namtrace-all $nf

proc finish {} {
	global ns nf
	$ns flush-trace
	close $nf
	exec nam out.nam &
	exit 0
}

# ===================================================== Topology setting ===========================================================

set attacker [$ns node]
set victim [$ns node]
set innocentguy [$ns node]

$ns node-config -BGP ON
set attacker_router [$ns node 0:10.0.0.1]
set victim_router [$ns node 1:10.0.1.1]
set innocent_router [$ns node 1:10.0.2.1]
$ns node-config -BGP OFF

$ns duplex-link $innocentguy $innocent_router 15Mb 10ms DropTail
$ns duplex-link $attacker $attacker_router 15Mb 10ms DropTail
$ns duplex-link $victim_router $attacker_router 15Mb 10ms DropTail
$ns duplex-link $victim_router $innocent_router 15Mb 10ms DropTail
$ns duplex-link $attacker_router $innocent_router 15Mb 10ms DropTail
$ns duplex-link $victim $victim_router 15Mb 10ms DropTail

set bgp_agent0 [$attacker_router get-bgp-agent] ;# gets the BGP routing agent
$bgp_agent0 bgp-id 10.0.0.1 ;# sets the BGP ID
$bgp_agent0 set-auto-config

set bgp_agent1 [$victim_router get-bgp-agent] ;# gets the BGP routing agent
$bgp_agent1 bgp-id 10.0.1.1 ;# sets the BGP ID
$bgp_agent1 set-auto-config

set bgp_agent2 [$innocent_router get-bgp-agent] ;# gets the BGP routing agent
$bgp_agent2 bgp-id 10.0.2.1 ;# sets the BGP ID
$bgp_agent2 set-auto-config

$ns duplex-link-op $victim $victim_router color orange
$ns duplex-link-op $victim_router $attacker_router color orange
$ns duplex-link-op $attacker $attacker_router color orange

$attacker color red
$victim color green
$attacker_router color purple
$victim_router color purple
$innocent_router color purple
# ============================================= end of topology setting =====================================================



# ==================================================== dos event  ===========================================================
set udp0 [new Agent/UDP]
$ns attach-agent $attacker $udp0

set cbr0 [new Application/Traffic/CBR]
$cbr0 set packetSize_ 500
$cbr0 set interval_ 0.0005
$cbr0 attach-agent $udp0

set null0 [new Agent/Null]
$ns attach-agent $victim $null0

$ns connect $udp0 $null0

$ns at 0 "$cbr0 start"
$ns at 4.5 "$cbr0 stop"
# ================================================= end of dos event  =======================================================


$ns at 10.0 "finish"
$ns run
