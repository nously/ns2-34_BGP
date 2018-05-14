/*
 * Copyright (c) 2003 Communication Networks Lab, Simon Fraser University.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Tony Dongliang Feng <tdfeng@cs.sfu.ca>, Brian J. Premore.
 *
 */

#ifndef global_h
#define global_h

class Global {

public:
  // ........................ member data ........................ //

  /** The minimum period of time at the beginning of the simulation, in
   *  seconds, during which all BGP speakers remain inactive. */
  const static double base_startup_wait = 0.01;

  /* The random number seed for jitter factors. If set to 0, jitter factors will
   * vary every simulation. It's non-zero by default for the purpose of simulation
   * repetablilty. */
  const static long jitter_factor_seed = 12345;

  /** A global Minimum Route Advertisement Interval Timer value for eBGP neighbors
   *  in seconds.  This value can be specified in the TCL configuration file.
   *  If autoconfig is in use and this value is specified, all MRAI timers for 
   *  external BGP neighbors use it without exception.  If autoconfig is in use
   *  and this value is unspecified, all MRAI timers use the TCL default value 
   *  (mrai_ of Agent/PeerEntry in ns-default.tcl) for eBGP neighbors without
   *  exception.  If autoconfig is not in use and this value is specified, all
   *  eBGP MRAI timers default to it, but they may be overridden on a 
   *  per-BGP-neighbor basis in the configuration file.  */
  const static int user_ebgp_mrai_default = 30;

  /** A global Minimum Route Advertisement Interval Timer value for iBGP neighbors
   *  in seconds.  This value can be specified in the TCL configuration file.
   *  If autoconfig is in use and this value is specified, all MRAI timers for
   *  external BGP neighbors use it without exception.  If autoconfig is in use
   *  and this value is unspecified, all MRAI timers use the TCL default value
   *  (mrai_ of Agent/PeerEntry in ns-default.tcl) for eBGP neighbors without
   *  exception.  If autoconfig is not in use and this value is specified, all
   *  eBGP MRAI timers default to it, but they may be overridden on a
   *  per-BGP-neighbor basis in the configuration file.  */
  const static int user_ibgp_mrai_default = -1;

  /** A global Hold Timer value for BGP neighbors, in seconds.  This value can
   *  be specified in the TCL configuration file.
   *  If autoconfig is in use and this value is specified, all Hold timers use
   *  it without exception.  If autoconfig is in use and this value is unspecified,
   *  all Hold timers use the TCL default value (hold_time_ of Agent/PeerEntry
   *  in ns-default.tcl) without exception.  If autoconfig is not in use and this
   *  value is specified, Hold timers default to it, but they may be overridden
   *  on a per-BGP-neighbor basis in the configuration file.  */
  const static int user_hold_time_default = 90;

  /** A global KeepAlive Timer value for BGP neighbors, in seconds.  This value can
   *  be specified in the TCL configuration file.
   *  If autoconfig is in use and this value is specified, all KeepAlive timers use
   *  it without exception.  If autoconfig is in use and this value is unspecified,
   *  all KeepAlive timers use the TCL default value (keep_alive_interval_ of 
   *  Agent/PeerEntryin ns-default.tcl) without exception.  If autoconfig is not 
   *  in use and thisvalue is specified, KeepAlive timers default to it, but they 
   *  may be overridden on a per-BGP-neighbor basis in the configuration file.  */
  const static int user_keep_alive_time_default = 30;   

  /** Whether or not rate-limiting should be applied to withdrawals (using the
   *  Minimum Route Advertisement Interval in the same way as it is used with
   *  advertisements).  RFC1771 states that it should not be applied, though
   *  many vendor implementations apparently to not comply. After changing 
   *  this option, remember to touch the files of mraiperpeertimer.cc and 
   *  mraitimer.cc and recompile before the changes take effect.   */
  const static bool wrate = false;

  /** Whether or not to perform sender-side loop detection. */
  const static bool ssld = false;

  /** Whether or not rate-limiting should be applied on a per-peer,
   *  per-destination basis.  The default is false, and rate-limiting is
   *  applied only on a per-peer basis, without regard to destination. */
  const static bool rate_limit_by_dest = false;

  /** Whether or not to jitter the Minimum AS Origination Timer.  Jitter is
   *  required for a conformant BGP implementation (see RFC1771:9.2.2.3). */
  const static bool jitter_masoi = true;

  /** Whether or not to jitter the Keep Alive Timers.  Jitter is required for a
   *  conformant BGP implementation (see RFC1771:9.2.2.3). */
  const static bool jitter_keepalive = true;

  /** Whether or not to jitter the Minimum Route Advertisement Interval Timers.
   *  Jitter is required for a conformant BGP implementation (see
   *  RFC1771:9.2.2.3). */
  const static bool jitter_mrai = true;

  /** Whether or not a BGP speaker should automatically attempt to reconnect
   *  when a peering session is lost.  If true, as soon as a peering session is
   *  broken, BGP will re-enter the Connect state and begin an attempt to
   *  establish a new transport session. */
  const static bool auto_reconnect = true;

  /** Whether or not to perform route aggregation.  (Currently disabled.) */
  const static bool use_aggregation = false;

  /** Whether or not a BGP speaker should automatically advertise its AS's
   *  network prefix to all neighbors. */
  const static bool auto_advertise = true;

   /** Whether or not to print out the debug messages.  */
  const static bool DEBUG = false;

};

#endif
