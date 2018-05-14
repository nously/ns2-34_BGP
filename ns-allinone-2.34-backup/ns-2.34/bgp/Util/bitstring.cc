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

#include "bitstring.h"

  // ----- constructor BitString(int,int) ---------------------------------- //
  /**
   * Converts an integer into an array of bits.  The bits are represented by
   * booleans (false=0, true=1), and the conversion is done in the following
   * way.  The lower order bits of the integer will become the lower numbered
   * elements in the bit array.  If the size is specified larger than the
   * actual number of bits in the internal representation of the integer, it
   * will be assumed that these non-existent higher order bits are zeroes.
   *
   * <p>EXAMPLE: BitString(23,8) is called.  Mentally we might picture the
   * integer 23 as the bit string 10111 (with any number of 0's optionally
   * prepended).  The size parameter tells us that eight of the bits are
   * significant to us, so for convenience we'll represent 23 as 00010111.  A
   * bit array 'b' constructed from this input would be b={1,1,1,0,1,0,0,0}, or
   * b[0]=b[1]=b[2]=b[4]=1 and b[3]=b[5]=b[6]=b[7]=0, where 0=false and 1=true.
   *
   * The standard printed representation of the array has the most
   * significant bits on the left and the least significant bits on the right.
   * The array representation has each index corresponding to the significance
   * of the bit it represents. In other words, the element 0 corresponds to the
   * least significant bit, element 1 corresponds to the second least
   * significant bit, and so on through element size-1, which represents the
   * most significant bit.
   *
   * @param val   The integer value of the bit string.
   * @param size  The number of bits in the bit string.
   * @see #BitString(unsigned int,int)
   */     
BitString::BitString(int val, int size) {
    BitString((unsigned int)val, size);
}

  // ----- constructor BitString(long,int) --------------------------------- //
  /**
   * Converts a long integer into an array of bits.
   *
   * @param val   The integer value of the bit string.
   * @param size  The number of bits in the bit string.
   * @see #BitString(int,int)
   */    
BitString::BitString(unsigned int val, int size) {   
   bits.assign(size,false);
   if ((val >> size) > 0) {       
      // not all of the non-zero bits are used for the bit string
      cerr << "[BitString] warning: some non-zero higher order bits ignored" << endl;
    }
    for (int i=0; i<size; i++) {
      // set bits[i] to 1 (true) if the i-th significant bit of val is set      
      bits[i] = ((val >> i)%2 == 1);
    }  
}

  // ----- BitString.set --------------------------------------------------- //
  /**
   * Sets the value of the given bit to one.
   *
   * @param bitnum  The index of a bit in the string.
   */
BitString::BitString(vector<bool> b) {
    bits = b;
}

BitString::~BitString(){
}

  // ----- BitString.set --------------------------------------------------- //
  /**
   * Sets the value of the given bit to one.
   *
   * @param bitnum  The index of a bit in the string.
   */
void BitString::set(int bitnum) {
    bits[bitnum] = Bit::one;
}

  // ----- BitString.clear ------------------------------------------------- //
  /**
   * Sets the value of the given bit to zero.
   *
   * @param bitnum  The index of a bit in the string.
   */
void BitString::clear(int bitnum) {
    bits[bitnum] = Bit::zero;
}

  // ----- BitString.getlr ------------------------------------------------- //
  /**
   * Returns the value of the given bit as an integer.  The index is
   * interpreted by counting from left to right, starting at 0.
   *
   * @param bitnum  The index of a bit in the string.
   * @return the value of the given bit as an integer
   */
int BitString::getlr(int bitnum) {
    return ((bits[size()-1-bitnum]) ? 1 : 0);
}

  // ----- BitString.bgetrl ------------------------------------------------ //
  /**
   * Return the value of the given bit as a boolean.  The index is
   * interpreted by counting from right to left, starting at 0.
   *
   * @param bitnum  The index of a bit in the string.
   * @return the value of the given bit as a boolean
   */
bool BitString::bgetrl(int bitnum) {
    return bits[bitnum];
}

  // ----- BitString.bgetlr ------------------------------------------------ //
  /**
   * Returns the value of the given bit as a boolean.  The index is
   * interpreted by counting from left to right, starting at 0.
   *
   * @param bitnum  The index of a bit in the string.
   * @return the value of the given bit as a boolean
   */
bool BitString::bgetlr(int bitnum) {
    return bits[size()-1-bitnum];
}

  // ----- BitString.size -------------------------------------------------- //
  /**
   * Returns the length of the string (the number of bits).
   *
   * @return the number of bits in the bit string
   */
int BitString::size() {
    return bits.size();
}

  // ----- BitString.And --------------------------------------------------- //
  /**
   * Performs a logical AND between this BitString and the given
   * BitString and assigns the results to this BitString.
   *
   * @param bs  The bit string to AND with.
   */
void BitString::And(BitString *bs) {
    if (size() != bs->size()) {
      cerr << "cannot AND bit strings of different size" << endl;
    }
    for (int i=0; i<size(); i++) {
      bits[i] = bits[i] && bs->bgetrl(i);
    }
}

  // ----- BitString.Or ---------------------------------------------------- //
  /**
   * Performs a logical OR between this BitString and the given
   * BitString and assigns the results to this BitString.
   *
   * @param bs  The bit string to OR with.
   */
void BitString::Or(BitString *bs) {
    if (size() != bs->size()) {
       cerr << "cannot OR bit strings of different size" << endl;
    }
    for (int i=0; i<size(); i++) {
      bits[i] = bits[i] || bs->bgetrl(i);
    }
}

  // ----- BitString.Xor --------------------------------------------------- //
  /**
   * Performs a logical XOR between this BitString and the given
   * BitString and assigns the results to this BitString.
   *
   * @param bs  The bit string to XOR with.
   */
void BitString::Xor(BitString *bs) {
    if ( size() != bs->size()) {
      cerr << "cannot XOR bit strings of different size" << endl;
    }
    for (int i=0; i<size(); i++) {
      bits[i] = (bits[i] && !bs->bgetrl(i)) || (!bits[i] && bs->bgetrl(i));
    }
}   

  // ----- BitString.Not --------------------------------------------------- //
  /**
   * Performs a logical NOT operation on this BitString and assigns
   * the results to this BitString.
   */
void BitString::Not() {
    for (int i=0; i<size(); i++) {
      bits[i] = (!bits[i]);
    }
}

  // ----- static BitString.And -------------------------------------------- //
  /**
   * Performs a logical AND between this two given BitStrings and
   * returns the result in a new BitString.
   *
   * @param bs1  The first of two bit strings perform an AND upon.
   * @param bs2  The second of two bit strings perform an AND upon.
   * @return the result of an AND between two bit strings
   */
BitString * BitString::And(BitString *bs1, BitString *bs2) {
    if (bs1->size() != bs2->size()) {
      cerr << "cannot AND bit strings of different size" << endl;
    }
    vector<bool> b;
    b.assign(bs2->size(),false);
    for (int i=0; i<bs1->size(); i++) {
      b[i] = bs1->bgetrl(i) && bs2->bgetrl(i);
    }
    return new BitString(b);
}

  // ----- static BitString.Or --------------------------------------------- //
  /**
   * Performs a logical OR between this two given BitStrings and
   * returns the result in a new BitString.
   *
   * @param bs1  The first of two bit strings perform an OR upon.
   * @param bs2  The second of two bit strings perform an OR upon.
   * @return the result of an OR between two bit strings
   */
BitString * BitString::Or(BitString *bs1, BitString *bs2) {
   if (bs1->size() != bs2->size()) {
      cerr << "cannot OR bit strings of different size" << endl;
    }
    vector<bool> b;
    b.assign(bs2->size(),false);
    for (int i=0; i<bs1->size(); i++) {
      b[i] = bs1->bgetrl(i) || bs2->bgetrl(i);
    }
    return new BitString(b);
}

  // ----- static BitString.Not -------------------------------------------- //
  /**
   * Performs a logical NOT on this BitStrings and returns the result
   * in a new BitString.
   *
   * @param bs  A bit string to perform a NOT upon.
   * @return the result of a NOT applied to a bit string
   */
BitString * BitString::Not(BitString *bs) {
    vector<bool> b;
    b.assign(bs->size(),false); 
    for (int i=0; i<bs->size(); i++) {
      b[i] = !bs->bgetrl(i);
    }
    return new BitString(b);
  }

 // ----- static BitString.Xor -------------------------------------------- //
  /**
   * Performs a logical XOR between this two given BitStrings and
   * returns the result in a new BitString.
   *
   * @param bs1  The first of two bit strings perform an XOR upon.
   * @param bs2  The second of two bit strings perform an XOR upon.
   * @return the result of an XOR between two bit strings
   */
BitString * BitString::Xor(BitString *bs1, BitString *bs2) {
    if (bs1->size() != bs2->size()) {
      cerr << "cannot XOR bit strings of different size" << endl;
    }
    vector<bool> b;
    b.assign(bs2->size(),false);
    for (int i=0; i<bs1->size(); i++) {
      b[i] = (bs1->bgetrl(i) && !bs2->bgetrl(i)) ||
             (!bs1->bgetrl(i) && bs2->bgetrl(i));
    }
    return new BitString(b);
}


  // ----- BitString.substring(int,int) ------------------------------------ //
  /**
   * Returns a substring of the bit string given two indices.
   *
   * @param s  The starting index for the substring.
   * @param e  The ending index for the substring.
   * @return a substring of the bit string
   */
BitString * BitString::substring(int s, int e) {
    BitString * bs = new BitString((unsigned int)0,e-s+1);
    int n = 0;
    for (int i=s; i<=e; i++) {
      bs->bits[n++] = bits[i];
    }
    return bs;
}

  // ----- BitString.substring(int) ---------------------------------------- //
  /**
   * Returns a substring of the bit string given one index and assuming the end
   * of the string as the other index.
   *
   * @param s  The starting index for the substring.
   * @return a substring of the bit string from the given index to the end of
   *         the string
   */
BitString * BitString::substring(int s) {
    int e = size()-1;
    BitString * bs = new BitString((unsigned int)0,e-s+1);
    int n = 0;
    for (int i=s; i<=e; i++) {
      cout << i;
      bs->bits[n++] = bits[i];
    }
    return bs;
}

  // ----- BitString.revSubstring(int,int) --------------------------------- //
  /**
   * Returns a substring of the reverse of the bit string given two indices.
   * In other words, the bit string is reversed, and then the normal substring
   * procedure is applied to the new, reversed string.
   *
   * @param s  The starting index for the substring.
   * @param e  The ending index for the substring.
   * @return a substring of the bit string's reverse
   */
BitString * BitString::revSubstring(int s, int e) {
    BitString *bs = new BitString((unsigned int)0,e-s+1);
    int n = 0;
    for (int i=e; i>=s; i--) {
      bs->bits[n++] = bits[i];
    }
    return bs;
}

  // ----- BitString.revSubstring(int) ------------------------------------- //
  /**
   * Returns a substring of the reverse of the bit string given one index and
   * assuming the end of the string as the other index.  In other words, the
   * bit string is reversed, and then the normal substring procedure is applied
   * to the new, reversed string.
   *
   * @param s  The starting index for the substring.
   * @return a substring of the bit string's reverse
   */
BitString * BitString::revSubstring(int s) {
    int e = size()-1;
    
    BitString *bs = new BitString((unsigned int)0,e-s+1);
    int n = 0;
    for (int i=e; i>=s; i--) {
      bs->bits[n++] = bits[i];
    }
    return bs;
}

// ----- BitString.toString ---------------------------------------------- //
  /**
   * Prints out the bit string.  It does so from left to right, from
   * most significant bit to least significant bit.
   *
   * @return a (text) string representation of the bit string
   */
string BitString::toString() {
    string str = "";
    for (int i=size()-1; i>=0; i--) {
      str += ((bits[i] ? "1" : "0"));  
    }   
    // put it in quotes to make the null string more recognizable
    return "\"" + str + "\"";
}



