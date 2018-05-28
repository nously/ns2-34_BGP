set nf [open out.nam w]
set tr [open out.tr w]
set ns [new Simulator]
$ns namtrace-all $nf
$ns trace-all $tr

proc finish {} {
	global ns nf tr
	$ns flush-trace
	close $nf
	close $tr
	exec nam out.nam &
	exit 0
}

# ===================================================== Topology setting ===========================================================

$ns node-config -BGP ON
set attacker_router [$ns node 0:10.0.0.1]
set victim_router [$ns node 0:10.0.1.1]
set innocent_router [$ns node 0:10.0.2.1]
set attacker [$ns node 0:10.0.3.1]
set victim [$ns node 0:10.0.4.1]
set innocentguy [$ns node 0:10.0.5.1]
$ns node-config -BGP OFF

$ns duplex-link $innocentguy $innocent_router 250Mb 10ms DropTail
$ns duplex-link $attacker $attacker_router 250Mb 10ms DropTail
$ns duplex-link $victim_router $attacker_router 250Mb 10ms DropTail
$ns duplex-link $victim_router $innocent_router 250Mb 10ms DropTail
$ns duplex-link $attacker_router $innocent_router 250Mb 10ms DropTail
$ns duplex-link $victim $victim_router 250Mb 10ms DropTail

set bgp_agent0 [$attacker_router get-bgp-agent] ;# gets the BGP routing agent
$bgp_agent0 bgp-id 10.0.0.1 ;# sets the BGP ID
$bgp_agent0 neighbor 10.0.3.1 remote-as 0
$bgp_agent0 neighbor 10.0.1.1 remote-as 0
$bgp_agent0 neighbor 10.0.2.1 remote-as 0

set bgp_agent1 [$victim_router get-bgp-agent] ;# gets the BGP routing agent
$bgp_agent1 bgp-id 10.0.1.1 ;# sets the BGP ID
$bgp_agent1 neighbor 10.0.4.1 remote-as 0
$bgp_agent1 neighbor 10.0.0.1 remote-as 0
$bgp_agent1 neighbor 10.0.2.1 remote-as 0

set bgp_agent2 [$innocent_router get-bgp-agent] ;# gets the BGP routing agent
$bgp_agent2 bgp-id 10.0.2.1 ;# sets the BGP ID
$bgp_agent2 neighbor 10.0.5.1 remote-as 0
$bgp_agent2 neighbor 10.0.0.1 remote-as 0
$bgp_agent2 neighbor 10.0.1.1 remote-as 0

set bgp_agent3 [$attacker get-bgp-agent] ;# gets the BGP routing agent
$bgp_agent3 bgp-id 10.0.3.1 ;# sets the BGP ID
$bgp_agent3 neighbor 10.0.0.1 remote-as 0

set bgp_agent4 [$victim get-bgp-agent] ;# gets the BGP routing agent
$bgp_agent4 bgp-id 10.0.4.1 ;# sets the BGP ID
$bgp_agent4 neighbor 10.0.1.1 remote-as 0

set bgp_agent5 [$innocentguy get-bgp-agent] ;# gets the BGP routing agent
$bgp_agent5 bgp-id 10.0.5.1 ;# sets the BGP ID
$bgp_agent5 neighbor 10.0.2.1 remote-as 0


$ns duplex-link-op $victim $victim_router color orange
$ns duplex-link-op $victim_router $attacker_router color orange
$ns duplex-link-op $attacker $attacker_router color orange

$attacker color red
$victim color green
$attacker_router color purple
$victim_router color purple
$innocent_router color purple

$ns color 1 red
$ns color 2 blue
# ============================================= end of topology setting =====================================================


# =================================================== bg traffic  ===========================================================

set tcp0 [new Agent/TCP]
$ns attach-agent $innocentguy $tcp0
set sink0 [new Agent/TCPSink]
$ns attach-agent $victim $sink0
$tcp0 set fid_ 2

set ftp [new Application/FTP]
$ftp attach-agent $tcp0
$ftp set type_ FTP
$ftp set packetSize 500
$ftp set rate_ 5mb

$ns connect $tcp0 $sink0

# ================================================ end of bg traffic  =======================================================



# ==================================================== dos event  ===========================================================

set udp0 [new Agent/UDP]
$ns attach-agent $attacker $udp0
set cbr0 [new Application/Traffic/CBR]
$cbr0 attach-agent $udp0
$cbr0 set packetSize_ 500
$cbr0 set rate_ 100mb
$udp0 set fid_ 1

set null0 [new Agent/Null]
$ns attach-agent $victim $null0

$ns connect $udp0 $null0

# ================================================= end of dos event  =======================================================


# ==================================================== icmp agent  ==========================================================

#Define a 'recv' function for the class 'Agent/Ping'
Agent/Ping instproc recv {from rtt} {
	$self instvar node_
	puts "node [$node_ id] sent icmp packet to victim."
}

set icmp0 [new Agent/Ping]
set icmp1 [new Agent/Ping]
set icmp2 [new Agent/Ping]
set icmp_victim0 [new Agent/Ping]
set icmp_victim1 [new Agent/Ping]
set icmp_victim2 [new Agent/Ping]

$ns attach-agent $attacker_router $icmp0
$ns attach-agent $victim_router $icmp1
$ns attach-agent $innocent_router $icmp2
$ns attach-agent $victim $icmp_victim0
$ns attach-agent $victim $icmp_victim1
$ns attach-agent $victim $icmp_victim2

$ns connect $icmp_victim0 $icmp0
$ns connect $icmp_victim1 $icmp1
$ns connect $icmp_victim2 $icmp2

proc x {} {
	global ns icmp1
	$icmp1 send
	$ns after 0.95 "x"
}

proc y {} {
	global ns icmp0
	$icmp0 send
	$ns after 1 "y"
}

proc z {} {
	global ns icmp2
	$icmp2 send
	$ns after 20 "z"
}

# ================================================= end of icmp agent  ======================================================





#$ns at 0.25 "puts \"\n time: 0.25 \n n0 (ip_addr 10.0.0.1) \
                     defines a network 10.0.0.0/24.\"" 
#$ns at 0.25 "$bgp_agent0 network 10.0.0.0/24"

#$ns at 0.35 "puts \"\n time: 0.35 \n n1 (ip_addr 10.0.1.1) \
                     defines a network 10.0.1.0/24.\"" 
#$ns at 0.35 "$bgp_agent1 network 10.0.1.0/24"

#$ns at 0.45 "puts \"\n time: 0.45 \n n2 (ip_addr 10.0.2.1) \
                     defines a network 10.0.2.0/24.\"" 
#$ns at 0.45 "$bgp_agent2 network 10.0.2.0/24"

#$ns at 0.55 "puts \"\n time: 0.55 \n n2 (ip_addr 10.0.3.1) \
                     defines a network 10.0.3.0/24.\"" 
#$ns at 0.55 "$bgp_agent3 network 10.0.3.0/24"

#$ns at 0.65 "puts \"\n time: 0.65 \n n2 (ip_addr 10.0.4.1) \
                     defines a network 10.0.4.0/24.\"" 
#$ns at 0.65 "$bgp_agent4 network 10.0.4.0/24"

#$ns at 0.75 "puts \"\n time: 0.75 \n n2 (ip_addr 10.0.5.1) \
                     defines a network 10.0.5.0/24.\"" 
#$ns at 0.75 "$bgp_agent5 network 10.0.5.0/24"


#$ns at 31 "$bgp_agent0 show-routes"
#$ns at 31 "$bgp_agent1 show-routes"
#$ns at 31 "$bgp_agent2 show-routes"
#$ns at 31 "$bgp_agent3 show-routes"
#$ns at 31 "$bgp_agent4 show-routes"
#$ns at 31 "$bgp_agent5 show-routes"


$ns at 5 "x; y; z"
$ns at 5 "$ftp start"
$ns at 6 "$cbr0 start"
$ns at 10 "$cbr0 stop"
$ns at 10 "$ftp stop"


$ns at 15 "finish"
$ns run

