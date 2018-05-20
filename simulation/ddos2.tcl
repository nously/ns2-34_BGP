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

$ns node-config -BGP ON
set attacker_router [$ns node 0:10.0.0.1]
set victim_router [$ns node 1:10.0.1.1]
set innocent_router [$ns node 2:10.0.2.1]
set attacker [$ns node 3:10.0.3.1]
set victim [$ns node 4:10.0.4.1]
set innocentguy [$ns node 5:10.0.5.1]
$ns node-config -BGP OFF

#set tcp_attacker [new Agent/TcpMaster]
#$tcp_attacker set node_ $attacker
#$ns attach-agent $attacker $tcp_attacker

#set tcp_victim [new Agent/TcpMaster]
#$tcp_victim set node_ $victim
#$ns attach-agent $victim $tcp_victim
#set ftp [new Application/FTP]
#$ftp attach-agent $tcp_attacker

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

set bgp_agent3 [$attacker get-bgp-agent] ;# gets the BGP routing agent
$bgp_agent3 bgp-id 10.0.3.1 ;# sets the BGP ID
$bgp_agent3 set-auto-config

set bgp_agent4 [$victim get-bgp-agent] ;# gets the BGP routing agent
$bgp_agent4 bgp-id 10.0.4.1 ;# sets the BGP ID
$bgp_agent4 set-auto-config

set bgp_agent5 [$innocentguy get-bgp-agent] ;# gets the BGP routing agent
$bgp_agent5 bgp-id 10.0.5.1 ;# sets the BGP ID
$bgp_agent5 set-auto-config



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
#set udp0 [new Agent/UDP]
#$ns attach-agent $attacker $udp0

#set cbr0 [new Application/Traffic/CBR]
#$cbr0 set packetSize_ 500
#$cbr0 set interval_ 0.0005
#$cbr0 attach-agent $udp0

#set null0 [new Agent/Null]
#$ns attach-agent $victim $null0

#$ns connect $udp0 $null0

#$ns at 32 "$cbr0 start"
#$ns at 40 "$cbr0 stop"

$ns at 0.25 "puts \"\n time: 0.25 \n n0 (ip_addr 10.0.0.1) \
                     defines a network 10.0.0.0/24.\"" 
$ns at 0.25 "$bgp_agent0 network 10.0.0.0/24"

$ns at 0.35 "puts \"\n time: 0.35 \n n1 (ip_addr 10.0.1.1) \
                     defines a network 10.0.1.0/24.\"" 
$ns at 0.35 "$bgp_agent1 network 10.0.1.0/24"

$ns at 0.45 "puts \"\n time: 0.45 \n n2 (ip_addr 10.0.2.1) \
                     defines a network 10.0.2.0/24.\"" 
$ns at 0.45 "$bgp_agent2 network 10.0.2.0/24"

$ns at 0.55 "puts \"\n time: 0.55 \n n2 (ip_addr 10.0.3.1) \
                     defines a network 10.0.3.0/24.\"" 
$ns at 0.55 "$bgp_agent3 network 10.0.3.0/24"

$ns at 0.65 "puts \"\n time: 0.65 \n n2 (ip_addr 10.0.4.1) \
                     defines a network 10.0.4.0/24.\"" 
$ns at 0.65 "$bgp_agent4 network 10.0.4.0/24"

$ns at 0.75 "puts \"\n time: 0.75 \n n2 (ip_addr 10.0.5.1) \
                     defines a network 10.0.5.0/24.\"" 
$ns at 0.75 "$bgp_agent5 network 10.0.5.0/24"


$ns at 31 "$bgp_agent0 show-routes"
$ns at 31 "$bgp_agent1 show-routes"
$ns at 31 "$bgp_agent2 show-routes"
$ns at 31 "$bgp_agent3 show-routes"
$ns at 31 "$bgp_agent4 show-routes"
$ns at 31 "$bgp_agent5 show-routes"

set tcp [new Agent/TCP]
$ns attach-agent $attacker $tcp
set sink [new Agent/TCPSink]
$ns attach-agent $victim $sink
$ns connect $tcp $sink
$tcp set fid_ 1


# ================================================= end of dos event  =======================================================


$ns at 50.0 "finish"
$ns run
