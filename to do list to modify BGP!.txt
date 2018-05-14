in C++
1. make attribute ieB in peer-entry{.h, .cc}
2. go to line 1247 seek to line 1265 in rtProtoBGP.cc (listen function) then call sendiTrace(msg) and
modify code to set ieB of peer-entry to 0
3. make attribute iiB in routeinfo{.h, .cc}
4. loc_rib in rtProtoBGP{.h, .cc} is a routing table
5. make new function in rtProtoBGP to handle iTraceTriger. it will then choose which entry (RoutingInfo object in loc_rib) have iiB set to 1 to set the peerEntry's ieB to 1.
6. make a call to iTraceTrigger in function which handle packetin (potential function: listen() push() or receive())


in TCL
1. make a dos simulation with BGP
2. manually set ieB to one for each client node
