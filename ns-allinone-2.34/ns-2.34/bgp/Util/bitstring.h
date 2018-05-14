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

#ifndef BITSTRING_H
#define BITSTRING_H

#include <iostream>
#include <vector>
#include <string> 
#include "bit.h"

using namespace std;

/**
 * This class implements a binary string. It has the fast array access property
 * of a boolean array, but the nice, intuitive interface of BitSet.  (Thinking
 * about a boolean array as a sequence of bits can get annoying.)
 */
class BitString {
public:
  BitString(int val, int size); 
  BitString(unsigned int val, int size);
  BitString(vector<bool> b);
	~BitString();

  void clear(int bitnum);
  int  getlr(int bitnum);
  bool bgetrl(int bitnum);
  bool bgetlr(int bitnum);
  int  size();
  void set(int bitnum);
  void And(BitString *bs);
  void Or(BitString *bs);
  void Xor(BitString *bs);
  void Not();
  BitString * And(BitString *bs1, BitString *bs2);
  BitString * Or(BitString *bs1, BitString *bs2);
  BitString * Not(BitString *bs);
  BitString * Xor(BitString *bs1, BitString *bs2);
  BitString * substring(int s);
  BitString * substring(int s, int e);
  BitString * revSubstring(int s);
  BitString * revSubstring(int s, int e);
  string toString();
  
private:
 /** An array of booleans representing a string of bits. */
  vector<bool> bits;
};

#endif
