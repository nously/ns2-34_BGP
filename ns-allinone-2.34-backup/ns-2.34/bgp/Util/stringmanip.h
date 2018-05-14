/***************************************************************************
                          stringmanip.h  -  description
                             -------------------
    begin                : Sun Jun 1 2003
    copyright            : (C) 2003 by 
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/**
  *@author Tony Dongliang Feng
  *@author BJ Premore
  */
 
#ifndef STRINGMANIP_H
#define STRINGMANIP_H

#include <string>   
using namespace std;

/**
 * This class contains public utility methods useful for manipulating character
 * strings.
 */
class StringManip {
public: 
	StringManip() {};
	~StringManip() {};
  string repeat(char ch, int len, int min);
  string repeat(char ch, int len);
  string ws(int len, int min);
  string ws(int len);
  string pad(string str, int places, char ch, bool atend);
  string pad(string str, int places, char ch);
  string pad(string str, int places);
  string pad(string str);
  string unpad(string str, char ch, bool atend, int minplaces);
  string unpad(string str, char ch, bool atend);
  string unpad(string str, char ch);
  string unpad(string str);
};

#endif
