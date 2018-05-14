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

#ifndef IPADDRESS_H
#define IPADDRESS_H
#include <iostream> // Changed by Reza Sahraei
#include <vector>
#include "bitstring.h"
#include <typeinfo>

using namespace std;
typedef vector<bool>  boolVector;

/**
 * Each instance of this class represents an IPv4 address.
 */
class IPaddress {
public: 
	IPaddress();
  IPaddress(const IPaddress &ipa);
  IPaddress( IPaddress * ipa);
  IPaddress(int v);
  IPaddress(unsigned int v);
  IPaddress(int v, int pre);
  IPaddress(unsigned int v, int pre);
  IPaddress(char * stream);
  IPaddress(unsigned char * stream, int prefix_len);
  IPaddress(boolVector bv);
  IPaddress(string str);
  ~IPaddress();

  void val2stream(char stream[4]); 
  string bin2str(boolVector bv);
  // Modified by Will Hrudey
  boolVector str2bin(string str);
  int bit(int bitnum);
  BitString * bits();
  BitString * masked_bits();
  BitString * prefix_bits(); 
  boolVector binval(bool fullsize);
  unsigned int val();
  int intval();
  int str2int(string str);
  unsigned int str2long(string str);
  void set_val(int v);
  void set_val(unsigned int v);
  void set(IPaddress ip);
  void set_prefix_len(int pre);
  int  prefix_len();
  bool valequals(IPaddress *ipaddr);
  bool equals(IPaddress * ipaddr);
  bool same_prefix(IPaddress *ipaddr);
  unsigned int masked_val(int len);
  unsigned int masked_val();
  int  masked_intval(int len);
  int  masked_intval();
  int  masked_bytes();
  void masked_toStream(char * stream);
  void print_binary();
  string val2str();     
  bool is_prefix_of(IPaddress * ipa);
  string toString();    


private:
  /** The integer value of the address.  (The 32 bits are treated as one big
   *  integer.) */
  unsigned int value;
  
  /** The number of bits in the prefix. */
  int prefix_length;
  
public:
  /** The integer value of the "maximum" IPv4 address: 255.255.255.255. */
  const static unsigned int MAX_IP = 255 + 255*256 +
                                     255*65536 + 255*(unsigned int)(16777216);
};

#endif
