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
 
#ifndef RADIXTREENODE_H
#define RADIXTREENODE_H

#include <stdio.h>
template <class T>

/**
 * A node in a radix tree.
 *
 * @see RadixTree
 */
class RadixTreeNode {
public:
  RadixTreeNode();
	~RadixTreeNode();
  bool is_empty();
public: 
  T  * data;
  RadixTreeNode * left;
  RadixTreeNode * right;          
};


  // ----- constructor RadixTreeNode --------------------------------------- //
  /**
   * Constructs a radix tree node using default values.
   */
template <class T>
RadixTreeNode<T>::RadixTreeNode(){
  data = NULL;
  left = NULL;
  right = NULL;
}

template <class T>
RadixTreeNode<T>::~RadixTreeNode(){
}

  // ----- RadixTreeNode.is_empty ------------------------------------------ //
  /**
   * Determines whether the subtree rooted at this node contains any data.
   *
   * @return true only if neither this node nor any of its descendants contain
   *         (non-null) data
   */
template <class T>
bool RadixTreeNode<T>::is_empty() {
  if (data != NULL) {
      return false;
  }
  if (left == NULL) {
    if (right == NULL) {
      return true;
    } else {
      return right->is_empty();
    }
  } else if (right == NULL) {
      return left->is_empty();
  } else {
      return (left->is_empty() || right->is_empty());
  }
}
 
#endif
