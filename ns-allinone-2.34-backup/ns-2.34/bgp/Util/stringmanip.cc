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
 
#include "stringmanip.h" 
#include <string.h> // Added by Reza Sahraei

  // ----- StringManip.repeat(char,int,int) -------------------------------- //
  /**
   * Creates a string of arbitrary length by repeating just one character.
   *
   * @param ch   The character to use for the string.
   * @param len  The length (in characters) of the string to be created.
   *             A negative value will be treated as a 0.
   * @param min  The minimum length (in characters) of the string to be
   *             created.  This may seem odd since length is specified, but
   *             it is useful sometimes when an expression with unknown
   *             value is provided for the length.
   * @return a string composed only of spaces
   */
string StringManip::repeat(char ch, int len, int min) {
    string str = "";
    int truelen = (len<min)?min:len;
    for (int i=0; i<truelen; i++) {
      str += ch;
    }
    return str;
}

  // ----- StringManip.repeat(char,int) ------------------------------------ //
  /**
   * Creates a string of arbitrary length by repeating just one character.
   *
   * @param ch   The character to use for the string.
   * @param len  The length (in characters) of the string to be created.
   *             A non-positive value is be treated as a 1.
   * @return a string composed only of spaces
   */
string StringManip::repeat(char ch, int len) {
    return repeat(ch,len,1);
}

  // ----- StringManip.ws(int,int) ----------------------------------------- //
  /**
   * Creates a string of whitespace composed of space characters only.
   *
   * @param len  The length (in characters) of the whitespace to be created.
   *             A negative value will be treated as a 0.
   * @param min  The minimum length (in characters) of the whitespace to
   *             be created.
   * @return a string composed only of spaces
   */
string StringManip::ws(int len, int min) {
    return repeat(' ',len,min);
}

  // ----- StringManip.ws(int) --------------------------------------------- //
  /**
   * Creates a string of whitespace composed of space characters only.
   *
   * @param len  The length (in characters) of the whitespace to be created.
   *             A non-positive value is treated as a 1.
   * @return a string composed only of spaces
   */
string StringManip::ws(int len) {
    return repeat(' ',len,1);
}

  // ----- StringManip.pad(string,int,char,bool) ------------------------ //
  /**
   * Pads a string by adding characters to it.
   *
   * @param str     The string to pad.
   * @param places  The maximum length of the string to pad up to.
   * @param ch      The character to pad with.
   * @param atend   Whether to pad at the end or the beginning of the string.
   * @return the padded string
   */
string StringManip::pad(string str, int places, char ch, bool atend) {
    while (str.length() < places) {
      if (atend) {
        str = str + ch;
      } else {
        str = ch + str;
      }
    }
    return str;
}

  // ----- StringManip.pad(string,int,char) -------------------------------- //
  /**
   * Pads a string by adding characters to it.
   *
   * @param str     The string to pad.
   * @param places  The maximum length of the string to pad up to.
   * @param ch      The character to pad with.
   * @return the padded string
   */
string StringManip::pad(string str, int places, char ch) {
    return pad(str, places, ch, false);
}

  // ----- StringManip.pad(string,int) ------------------------------------- //
  /**
   * Pads a string by adding characters to it.
   *
   * @param str     The string to pad.
   * @param places  The maximum length of the string to pad up to.
   * @return the padded string
   */
string StringManip::pad(string str, int places) {
    return pad(str, places, '0', false);
}

  // ----- StringManip.pad(string) ----------------------------------------- //
  /**
   * Pads a string by adding characters to it.
   *
   * @param str  The string to pad.
   * @return the padded string
   */
string StringManip::pad(string str) {
    return pad(str, 2, '0', false);
}

  // ----- StringManip.unpad(string,char,bool,int) ---------------------- //
  /**
   * Removes "padding" characters from a string.  The opposite of pad.
   *
   * @param str        The string to unpad.
   * @param ch         The character used as padding.
   * @param atend      Whether to unpad at the end or beginning of the string.
   * @param minplaces  The minimum number of characters to leave in the string
   *                   after unpadding.
   * @return the unpadded string
   */
string StringManip::unpad(string str, char ch, bool atend, int minplaces) {
    const char * temp= &ch;
    if (atend) {
      while (str.length() > minplaces &&
            strcmp(str.substr(str.length()-1,1).c_str(), temp)) {
        str = str.substr(0,str.length()-2);
      }
    } else { // unpad at beginning
      while (str.length() > minplaces &&
            strcmp(str.substr(0,1).c_str() , temp) ) {
        str = str.substr(1, str.length()-1);
      }
    }
    return str;
}

  // ----- StringManip.unpad(string,char,bool) -------------------------- //
  /**
   * Removes "padding" characters from a string.  The opposite of pad.
   *
   * @param str    The string to unpad.
   * @param ch     The character used as padding.
   * @param atend  Whether to unpad at the end or beginning of the string.
   * @return the unpadded string
   */
string StringManip::unpad(string str, char ch, bool atend) {
    return unpad(str, ch, atend, 0);
}

  // ----- StringManip.unpad(string,char) ---------------------------------- //
  /**
   * Removes "padding" characters from a string.  The opposite of pad.
   *
   * @param str  The string to unpad.
   * @param ch   The character used as padding.
   * @return the unpadded string
   */
string StringManip::unpad(string str, char ch) {
    return unpad(str, ch, false, 0);
}

  // ----- StringManip.unpad(string) --------------------------------------- //
  /**
   * Removes "padding" characters from a string.  The opposite of pad.
   *
   * @param str  The string to unpad.
   * @return the unpadded string
   */
string StringManip::unpad(string str) {
    return unpad(str, '0', false, 0);
}


