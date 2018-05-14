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

// Modified by Will Hrudey
#include <math.h>
#include "ipaddress.h"
#include<stdio.h>
// Added by Reza Sahraei
#include <stdlib.h>
#include <string.h>


// ----- constructor IPaddress() ----------------------------------------- //
  /**
   * Default constructor: constructs the IP address 1.1.1.1/32.  (Primarily for
   * debugging use.)
   */     
IPaddress::IPaddress(){
  value = (unsigned int)(1 + 256 + 65536 + (unsigned int)(16777216));
  prefix_length = 32;
}

  // ----- constructor IPaddress(IPaddress) -------------------------------- //
  /**
   * Copy constructor.  All data used to initialize the IP address is copied
   * from the given IP address.
   *
   * @param ipa  The IP address to copy.
   */
IPaddress::IPaddress(const IPaddress &ipa)  {
  value = ipa.value;
  prefix_length = ipa.prefix_length;  
}

  // ----- constructor IPaddress(IPaddress*) -------------------------------- //
  /**
   * Copy constructor.  All data used to initialize the IP address is copied
   * from the given IP address.
   *
   * @param ipa  The pointer IP address to copy.
   */
IPaddress::IPaddress( IPaddress * ipa)  {
  value = ipa->value;
  prefix_length = ipa->prefix_length;  
}

  // ----- constructor IPaddress(int) -------------------------------------- //
  /**
   * Constructs an IP address using an integer as the address's value.  The
   * default of 32 is used for the prefix length.
   *
   * @param v  The integer value of the IP address.
   */
IPaddress::IPaddress(int v) {
  IPaddress((unsigned int)v);  
}

  // ----- constructor IPaddress(unsigned int) ------------------------------------- //
  /**
   * Constructs an IP address using a (unsigned int) integer as the address's value.
   * The default of 32 is used for the prefix length.
   *
   * @param v  The (unsigned int) integer value of the IP address.
   */
IPaddress::IPaddress(unsigned int v) {
  value = v;
  prefix_length = 32;
}

  // ----- constructor IPaddress(int,int) ---------------------------------- //
  /**
   * Constructs an IP address with the given integer value and prefix length.
   *
   * @param v    The integer value of the IP address.
   * @param pre  The prefix length of the IP address.
   */
IPaddress::IPaddress(int v, int pre){
  IPaddress((unsigned int)v, pre);
}


  // ----- constructor IPaddress(unsigned int,int) --------------------------------- //
  /**
   * Constructs an IP address with the given (unsigned int) integer value and
   * prefix length.
   *
   * @param v    The (unsigned int) integer value of the IP address.
   * @param pre  The prefix length of the IP address.
   */
IPaddress::IPaddress(unsigned int v, int pre){
   value = v;
    if (pre < 0 || pre > 32) {
      cerr << "illegal prefix length in IP address: " << pre << endl;
    }
    prefix_length = pre;    
}

  // ----- constructor IPaddress(boolVector) -------------------------------- //
  /**
   * Constructs an IP address using a boolean vector.  Each boolean in the vector
   * represents a bit in the address.  A value of 'true' indicates a 1 and
   * 'false' indicates a 0.  The last element in the boolean vector represents
   * the highest order (first, leftmost) bit in the IP address.  The length of
   * the boolean vector indicates the prefix length of the IP address.  Element
   * 0 in the boolean vector represents the lowest-order bit of the IP address
   * that is not beyond the prefix length.  All bits beyond the prefix length
   * are set to 0.
   *
   * @param bv  An vector of booleans which represents an IP address.
   */
IPaddress::IPaddress(boolVector bv) {
    prefix_length = bv.size();    
    if (prefix_length > 32) {
      cerr << "illegal prefix length in IP address: " << prefix_length << endl;
    }
    value = 0;
    for (int i=0; i<prefix_length; i++) {
      if (bv[i]) {
        value += (long)(1 << i);
      }
    }  
}

  // ----- constructor IPaddress(string) --------------------------------- //
  /**
   * Constructs an IP address using a string.  The string must represent an IP
   * address either in dotted-quad notation (a.b.c.d), dotted-quad plus prefix
   * notation (a.b.c.d/p), or binary notation (for example, 01100010010). There
   * may optionally be double-quotes (") surrounding the notation.  If no
   * prefix is provided in dotted-quad notation, a value of 32 is used.  In
   * binary notation, the prefix length is the length of the string, and the
   * leftmost character represents the most significant bit.
   *
   * @param str  The string representing an IP address.
   */              
IPaddress::IPaddress(string str) {
    int i=0, stop;
    int addr[4];  
    prefix_length = 32;  

    if (str.length() > 0) {
        if (str.find(".",0) < 0) {
        // it's in binary string notation
        if (str.substr(0,1) == "\"") {
          str = str.substr(1, str.length()-1);
        }
        value = 0;
        for (int j=0; j<str.length(); j++) {
          if (str.substr(j,1) == "1") {
            value += (long)(1 << (31-j));
          }
        }
        prefix_length = str.length();
        return;
      }
    } else {
      prefix_length = 0;
      value = (long)0;
      return;
    }

    // dotted-quad notation
    if (str.substr(0,1) == "\"") {
      // there are quotes around the IP address
      i = 1;
      stop = str.length()-1;
    } else { // no quotes
      stop = str.length();
    }

    // parse and convert the a.b.c.d part
    for (int p=0; p<4; p++) {
      addr[p] = 0;
      while (i<stop && str.substr(i,1) != "." && str.substr(i,1) != "/") {
        addr[p] = addr[p]*10 + atoi(str.substr(i,1).c_str());
        i++;
      }
      if (p < 3) {
        i++;
      }
    }
    // parse and convert the prefix (if it exists)
    if (i<stop && str.substr(i,1) == "/") {
      i++;
      prefix_length = 0;
      while (i < stop) {
        prefix_length= prefix_length*10 + atoi(str.substr(i,1).c_str());
        i++;
      }
    }   
    // convert a.b.c.d address to a single integer
    value = addr[0]*(long)16777216 + addr[1]*65536 + addr[2]*256 + addr[3];
}

  // ----- constructor IPaddress(string) --------------------------------- //
  /**
   * Constructs an IP address using a char stream.
   *
   * @param steam  The char stream representing an IP address.
   */ 
IPaddress::IPaddress(char * stream) {
     value = ((unsigned int)stream[0])*(long)16777216 + ((unsigned int)stream[1])*65536 +
             ((unsigned int)stream[2])*256 + (unsigned int)stream[3];
     prefix_length=32;
}

  // ----- constructor IPaddress(string) --------------------------------- //
  /**
   * Constructs an IP address using a char stream and a given prefix length.
   *
   * @param steam  The char stream representing an IP address.
   * @param prefix_len  The prefix length of the IP address.
   */ 

IPaddress::IPaddress(unsigned char * stream, int prefix_len) {
     int masked_bytes = (int)ceil (prefix_len/8.0);
     switch (masked_bytes) {
         case 1:
         value = ((unsigned int)stream[0])*(long)16777216 ;
         break;
         case 2:
         value = ((unsigned int)stream[0])*(long)16777216 + ((unsigned int)stream[1])*65536 ;
         break;
         case 3:
         value = ((unsigned int)stream[0])*(long)16777216 + ((unsigned int)stream[1])*65536 +
                 ((unsigned int)stream[2])*256;
         break;
         case 4:
         value = ((unsigned int)stream[0])*(long)16777216 + ((unsigned int)stream[1])*65536 +
                 ((unsigned int)stream[2])*256 + (unsigned int)stream[3];
         break;
         default:
           cerr <<   "invalid prefix_len" << endl;
     }
     prefix_length= prefix_len;
}

IPaddress::~IPaddress(){
}

  // ----- IPaddress.bin2str ----------------------------------------------- //
  /**
   * Converts an IP address from boolean vector format to string format.  See
   * constructor IPaddress(boolVector) for an explanation of the
   * boolean vector format.
   * @see #IPaddress(boolVector)
   *
   * @param bv  A boolean vector representing an IP address.
   * @return an IP address in String format
   */

string IPaddress::bin2str(boolVector bv) {
    int prefix_length = bv.size();
    int ipval[4];   
    ipval[0] = ipval[1] = ipval[2] = ipval[3] = 0;
    
    // Boolean vector can be clumsy to work with.  Here we convert to
    // an integer array, reverse the bits (so that higher order bits
    // will now come first), and pad with zeroes (so it's a full 32
    // bits long).

    int bits[32];  
    // reverse the order while converting from booleans to integers
    for (int i=0; i<bv.size(); i++) {
      bits[i] = bv[bv.size()-1-i] ? 1 : 0;
    }  
    // pad the lower order bits with 0s out to a total of 32
    for (int i=bv.size(); i<32; i++) {
      bits[i] = 0;
    }   
    // convert from an integer array to dotted-quad notation
    for (int i=0; i<4; i++) {
      for (int j=i*8; j<(i+1)*8; j++) {
        ipval[i] = (ipval[i] << 1) + bits[j];
      }
    }  
    char str[18] = "" ;
    // build a string representation from dotted-quad (plus prefix length)
    sprintf(str, "%d.%d.%d.%d/%d", ipval[0], ipval[1],
                 ipval[2], ipval[3], prefix_length);
    return str;
}

  // ----- IPaddress.str2bin ----------------------------------------------- //
  /**
   * Converts an IP address from string format to boolean array format.
   *
   * @param str  A string representing the IP address to be converted.
   * @return an IP address in boolean vector format
   */
boolVector IPaddress::str2bin(string str) {
    boolVector bv; 
    unsigned int tmpval = (new IPaddress(str))->value;
    
    for (int i=0; i<bv.size(); i++) {
      if (tmpval%2 == 1) {
        bv[i] = true;
      } else {
        bv[i] = false;
      }
      tmpval >>= 1;
    }
    return bv;
}

  // ----- IPaddress.bit --------------------------------------------------- //
  /**
   * Returns the value of a single bit in the IP address.  Bits are indexed
   * from 1 to 32.  Bit 1 is the most significant (leftmost) and bit 32 is the
   * least significant (rightmost).
   *
   * @param bitnum  The number of the bit to return.
   * @return the value of the bit in question
   */
int IPaddress::bit(int bitnum) {
    if (bitnum < 1 || bitnum > 32) {
      cout << "IP address bit number out of range: " << bitnum << endl;
    }
   return (int) ((1 << (32-bitnum)) & value);
}

 // ----- IPaddress.bits -------------------------------------------------- //
  /**
   * Returns the bits in the IP address as a BitString of length 32 (prefix
   * length is ignored).
   *
   * @return an IP address in the form of a BitString object
   */
BitString * IPaddress::bits(){
  return new BitString(value,32);
}

// ----- IPaddress.masked_bits ------------------------------------------- //
  /**
   * Returns the bits in the IP address, after being masked, as a BitString of
   * length 32.
   *
   * @return an IP address in the form of a BitString object
   */
BitString * IPaddress::masked_bits(){
  return new BitString(masked_val(),32);
}

  // ----- IPaddress.prefix_bits ------------------------------------------- //
  /**
   * Returns only the prefix bits in the IP address as a BitString of length
   * prefix_len.
   *
   * @return a BitString object representing the prefix bits of an
   *         IP address
   */
BitString * IPaddress::prefix_bits() {
    return new BitString(value >> (32-prefix_length), prefix_length);
}

  // ----- IPaddress.binval(boolean) --------------------------------------- //
  /**
   * Returns the IP address in boolean array format.  The fullsize
   * parameter determines whether or not the array length is 32 or
   * prefix_len.
   *
   * @param fullsize  Whether or not to pad the array with zeroes (falses)
   *                  up to 32 if the prefix length is less than 32.
   * @return an IP address in boolean array format
   */
boolVector IPaddress::binval(bool fullsize) {
    int len = (fullsize ? 32 : prefix_length);
    boolVector bv; 
    bv.assign(len,false);
    unsigned int tmpval = value;

    for (int i=0; i<len; i++) {
      if (tmpval%2 == 1) {
        bv[i] = true;
      } else {
        bv[i] = false;
      }
      tmpval >>= 1;
    }
    return bv;
}


  // ----- IPaddress.val --------------------------------------------------- //
  /**
   * Returns the value of IP address when taken as a 32-bit number (prefix
   * length ignored).
   *
   * @return the value of the IP address as a 32-bit integer
   */
unsigned int IPaddress::val() {
   return value;
}

  // ----- IPaddress.intval ------------------------------------------------ //
  /**
   * Returns the value of IP address just as in val(), but returns
   * it as an integer, if possible, instead of a long integer.  Some IP address
   * values may be too big to fit in a regular integer.  This method exists
   * because in many cases it's easier to deal with an int than a long.
   *
   * @return the value of the IP address as a 32-bit integer
   */
int IPaddress::intval() {
    if (value == (int)value) {
      return (int)value;
    } else {
      cerr << "IP address too large to be represented as an integer";
    }
}

  // ----- IPaddress.str2int ----------------------------------------------- //
  /**
   * Converts an IP address from string format to integer format.
   *
   * @param str  A string representing an IP address.
   * @return an IP address in integer form
   */
int IPaddress::str2int(string str) {
    IPaddress * tmpip = new IPaddress(str);
    return tmpip->intval();
}

  // ----- IPaddress.str2long ---------------------------------------------- //
  /**
   * Converts an IP address from string format to unsigned integer format.
   *
   * @param str  A string representing an IP address.
   * @return an IP address in long integer form
   */
unsigned int IPaddress::str2long(string str) {
    IPaddress * tmpip = new IPaddress(str);
    return tmpip->val();
}

  // ----- IPaddress.set_val ----------------------------------------------- //
  /**
   * Set the value of the IP address using an integer.
   *
   * @param v  An integer to use as the value of the IP address.
   */
void IPaddress::set_val(int v) {
    value = (unsigned int)v;
}

  // ----- IPaddress.set_val ----------------------------------------------- //
  /**
   * Set the value of the IP address using a unsigned integer.
   *
   * @param v  A unsigned integer to use as the value of the IP address.
   */
void IPaddress::set_val(unsigned int v) {
    value = v;
}

  // ----- IPaddress.set --------------------------------------------------- //
  /**
   * Set the value of the IP address based on another IP address.
   *
   * @param ip  An IP address to use as the value for this IP address.
   */
void IPaddress::set(IPaddress ip) {
    value = ip.val();
    prefix_length = ip.prefix_len();
}

  // ----- IPaddress.set_prefix_len ---------------------------------------- //
  /**
   * Set the prefix length to the given value.
   *
   * @param pre  The value to use as the new prefix length.
   */
void IPaddress::set_prefix_len(int pre) {
    prefix_length = pre;
}

  // ----- IPaddress.prefix_len -------------------------------------------- //
  /**
   * Returns the prefix length.
   *
   * @return the prefix length
   */
int IPaddress::prefix_len() {
    return prefix_length;
}

  // ----- IPaddress.valequals --------------------------------------------- //
  /**
   * Determines whether two IP addresses have equal values.  (Whether or not
   * their prefix lengths are the same is ignored.)
   *
   * @param ipaddr  The IP address with which to make the comparison.
   * @return true only if the two values are the same, false otherwise.
   */
bool IPaddress::valequals(IPaddress *ipaddr) {
    return (ipaddr != NULL && value == ipaddr->val());
}

   // ----- IPaddress.equals ------------------------------------------------ //
  /**
   * Determines whether two IP addresses are equal.  They are equal only if
   * both the values and prefix lengths of each are the same.
   *
   * @param ipaddr  The IP address with which to make the comparison.
   * @return true only if the two are identical, false otherwise.
   */
bool IPaddress::equals(IPaddress * ipaddr) {
    return (ipaddr != NULL &&
            //  (typeid(*ipaddr) == typeid(IPaddress)) &&
            value == ipaddr->val() &&
            prefix_length == ipaddr->prefix_len());
}

  // ----- IPaddress.same_prefix ------------------------------------------- //
  /**
   * Returns whether the given IP address prefix, when masked, is the same as
   * this IP address prefix when masked.
   *
   * @param ipaddr  An IP address prefix for comparison.
   * @return the result of the comparison
   */
bool IPaddress::same_prefix(IPaddress *ipaddr) {
    return (ipaddr != NULL && masked_val() == ipaddr->masked_val());
}

  // ----- IPaddress.masked_val(int) --------------------------------------- //
  /**
   * Returns the value of the IP address when the bits beyond a given point
   * are masked out (taken as zeroes).
   *
   * @param len  The length in bits beyond which to mask the address.
   * @return the masked value of the IP address
   */
unsigned int IPaddress::masked_val(int len) {
    if (len == 0) {
      return 0;
    } else if (len == 32) {
      return value;
    }

    int mask = 1;
    int i;
    for (i=1; i<len; i++) {
      mask <<= 1;
      mask++;
    }
    for (i=len; i<32; i++) {
      mask <<= 1;
    }
    return (value & mask);
}

  // ----- IPaddress.masked_val() ------------------------------------------ //
  /**
   * Returns the value of the IP address when the bits beyond the prefix length
   * are masked out (taken as zeroes).
   *
   * @return the masked value of the IP address
   */
unsigned int IPaddress::masked_val() {
    return masked_val(prefix_length);
}

int IPaddress::masked_intval(int len) {
    if (masked_val(len) == (int)masked_val(len)) {
      return (int)masked_val(len);
    } else {
      cout << "IP address too large to be represented as an integer" << endl;
    }
}

  // ----- IPaddress.masked_intval(int) ------------------------------------ //
  /**
   * Returns the masked value of the IP address just as in
   * >masked_val(int), but returns it as a plain integer if
   * possible.  Some IP address values may be too big to fit in an plain
   * integer.  This method exists because in many cases it's easier to deal
   * with a plain integer than a unsigned integer.
   *
   * @param len  The length in bits beyond which to mask the address.
   * @return the masked value of the IP address in integer form
   */
int IPaddress::masked_intval() {
    return masked_intval(prefix_length);
}

  // ----- IPaddress.val2stream ----------------------------------------------- //
  /**
   * Returns a char stream representing this IP address 
   *
   * @return a string in dotted-quad notation representing this IP address as stream
   */
void IPaddress::val2stream(char stream[4]) {
    unsigned int a = 255, b = 255, c = 255, d = 255;
    int A,B,C,D; 
    a <<= 24;
    b <<= 16;
    c <<= 8;

    A = (int)((value & a) >> 24);
    B = (int)((value & b) >> 16);
    C = (int)((value & c) >> 8);
    D = (int)(value & d);

    stream[0] = A;
    stream[1] = B;
    stream[2] = C;
    stream[3] = D;
}               

  // ----- IPaddress.print_binary ------------------------------------------ //
  /**
   * Prints the binary value of this IP address, ignoring prefix length.
   */
void IPaddress::print_binary() {
    int one = 1;
    unsigned int mask;
    for (int i=31; i>=0; i--) {
      mask = one << i;
      if ((value & mask) != 0) {
        cout << "1";
      } else {
        cout << "0";
      }
      if (i!=0 && (i%8)==0) {
        cout << ".";
      }
    }
}

 // ----- IPaddress.val2str ----------------------------------------------- //
  /**
   * Returns a string representing this IP address in dotted-quad (a.b.c.d)
   * notation, without prefix length.
   *
   * @return a string in dotted-quad notation representing this IP address
   */
string IPaddress::val2str() {
    unsigned int a = 255, b = 255, c = 255, d = 255;
    int A,B,C,D;
    
    a <<= 24;
    b <<= 16;
    c <<= 8;

    A = (int)((value & a) >> 24);
    B = (int)((value & b) >> 16);
    C = (int)((value & c) >> 8);
    D = (int)(value & d);

    char str[18] = "" ;
    sprintf(str, "%d.%d.%d.%d", A,B,C,D); 
    return str;
}

 // ----- IPaddress.is_prefix_of ------------------------------------------ //
  /**
   * Determines whether this IP address is a proper prefix of a given IP
   * address.  Note that since it must be a proper prefix, if they are
   * identical the result will be false.
   *
   * @return true only if it is a proper prefix
   */
bool IPaddress::is_prefix_of(IPaddress * ipa) {
    if (prefix_length >= ipa->prefix_len()) {
      return false;
    }
    return (masked_val() == ipa->masked_val(prefix_length));
}

  // ----- IPaddress.toString(boolean) ------------------------------------- //
  /**
   * Converts an IP address object to a string.  The string may be in IP
   * address or NHI address form.
   *
   * @param usenhi  Whether to use the NHI or traditional address format.
   * @return the address as a string
   */
string IPaddress::toString() {
   char str[20] = "";   
   char stri[5] = "";
   strcpy(str,val2str().c_str());
   sprintf(stri,"/%d", prefix_length);
   strcat(str,stri);
   return str;
}

  // ----- IPaddress.masked_bytes() ------------------------------------- //
  /**
   * Return number of blocks after masked
   *
   * @return number of blocks after masked
   */
int IPaddress::masked_bytes() {
  return  (int)ceil(prefix_length/8.0);
}
                     
  // ----- IPaddress.masked_toStream(char *)---------------------------------------------- //
  /**
   * Converts an IP address to char stream format.
   *
   * @param stream (return para) char stream after conversion.
   */
void IPaddress::masked_toStream(char * stream){
  int masked_value = IPaddress::masked_intval() >> (32-prefix_length);
  int blocks = masked_bytes();
  int aligned_masked_value = masked_value << (blocks*8 - prefix_length);
  for ( int i=0; i< blocks; i++) {
    int offset = (blocks-i-1)*8;
    stream[i] = (aligned_masked_value & (255<<offset)) >> offset; 
  }    
}


