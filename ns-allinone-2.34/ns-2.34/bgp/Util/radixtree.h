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
 
#ifndef RADIXTREE_H
#define RADIXTREE_H

#include "radixtreenode.h"
#include "bitstring.h"
#include <vector>

using namespace std;

/**
 * This class is used as an efficient way to store information which is keyed
 * by a binary string (IP addresses, for example).
 */
template <class T>
class RadixTree {
public:
  typedef RadixTreeNode<T> TreeNodeType;
  typedef vector<T*>  TpVector;
	RadixTree();
	~RadixTree();
  TreeNodeType * root();
  T * add(BitString  bs, T * obj);
  T * replace(BitString bs, T * obj);
  T * find(BitString bs);
  TpVector get_ancestors(BitString bs);
  T * oldest_ancestor(BitString bs);
  T * youngest_ancestor(BitString bs);
  TpVector get_descendants(BitString bs);
  bool has_descendants(BitString bs);
  T * remove(BitString bs);
  void prune(BitString bs);
  void print();
  
private:
  T * add_helper(TreeNodeType * node, BitString bs,
                  int pos, T * obj, bool replace);
  T * find_helper(TreeNodeType * node, BitString bs,
                   int pos, bool remove);
  TpVector ga_helper(TreeNodeType * node, BitString bs,
                   int pos, TpVector ancestors);
  T * oa_helper(TreeNodeType * node, BitString bs, int pos);
  T * ya_helper(TreeNodeType * node, BitString bs, int pos,
                T * best);
  TpVector gd_helper(TreeNodeType * node, BitString bs, int pos,
                     TpVector descendants);
  bool hd_helper(TreeNodeType * node, BitString bs, int pos);
  void prune_helper(TreeNodeType * node, BitString bs, int pos);
  void print_helper(TreeNodeType * node, string str);
  
private:
  /** The root node of the tree. */
  TreeNodeType * root_node;
};


  // ----- constructor RadixTree ------------------------------------------- //
  /**
   * Constructs an empty radix tree.
   */
template <class T>
RadixTree<T>::RadixTree(){
   root_node = new RadixTreeNode<T>();
}

template <class T>
RadixTree<T>::~RadixTree(){
}

  // ----- RadixTree.root -------------------------------------------------- //
  /**
   * Returns the root node of the tree.
   *
   * @return the root node of the tree
   */
template <class T>
RadixTreeNode<T> * RadixTree<T>::root() {
  return root_node;
}

// ----- RadixTree.replace ----------------------------------------------- //
  /**
   * Adds data to the tree, keyed by the given binary string, replacing any
   * pre-existing data with that key which may have already been there.
   *
   * @param bs   The binary string to use as the key for the data.
   * @param obj  The data to add to the tree.
   * @return data which was replaced by the addition; null if none
   */
template <class T>
T * RadixTree<T>::replace(BitString bs, T * obj) {
    return add_helper(root_node, bs, 0, obj, true);
}

// ----- RadixTree.add --------------------------------------------------- //
  /**
   * Attempts to add data to the tree, keyed by the given binary string, but
   * fails if data associated with that string already exists.  Upon failure,
   * the pre-existing data is returned.  Upon success, null is returned.
   *
   * @param bs   The binary string to use as the key for the addition
   * @param obj  The data to add to the tree.
   * @return data which was replaced by the addition; null if none
   */
template <class T>
T * RadixTree<T>::add(BitString  bs, T * obj) {
    return add_helper(root_node, bs, 0, obj, false);
}

 // ----- RadixTree.add_helper -------------------------------------------- //
  /**
   * A recursive helper for both add and replace.
   *
   * @param node  The current node being traversed.
   * @param bs    The bit string being used to key the addition.
   * @param pos   The position in the bit string associated with the
   *              current node.
   * @param obj   The data to be added.
   * @param replace  Whether or not to replace any pre-existing data.
   * @return any pre-existing data, whether replaced or not
   * @see #add(BitString,Object)
   * @see #replace(BitString,Object)
   */

template <class T>
T * RadixTree<T>::add_helper(TreeNodeType * node, BitString  bs,
                             int pos, T * obj, bool replace) {
    if (pos == bs.size()) {
      // this is where it goes
      if (replace) {
        // it's OK to replace any pre-existing data
        T * o = node->data;
        node->data = obj;
        return o;
      } else {
        // if there's data here already, we're not allowed to replace it
        if (node->data != NULL) {
          // something's already here, add fails
          return node->data; // return the pre-existing data
        } else {
          // nothing's here, add succeeds
          node->data = obj;
          return NULL; // null indicates success
        }
      }
    }

    if (bs.bgetlr(pos) == Bit::zero) {
      // the next bit is a zero, so follow down the left child
      if (node->left == NULL) {
        node->left = new RadixTreeNode<T>();
      }
      return add_helper(node->left, bs, pos+1, obj, replace);
    } else {
      // the next bit is a one, so follow down the right child
      if (node->right == NULL) {
        node->right = new RadixTreeNode<T>();
      }
      return add_helper(node->right, bs, pos+1, obj, replace);
    }
}

 // ----- RadixTree.find -------------------------------------------------- //
  /**
   * Returns the data associated with the given binary string, if any.
   *
   * @param bs  The bit string being used to key the find.
   * @return data associated with the given binary string, if any
   */
template <class T>
T * RadixTree<T>::find(BitString bs) {
    return find_helper(root_node, bs, 0, false);
}

  // ----- RadixTree.find_helper ------------------------------------------- //
  /**
   * A recursive helper for both find and remove,
   * since they are very similar.
   *
   * @param node  The current node being traversed.
   * @param bs    The bit string being used to key the action.
   * @param pos   The position in the bit string associated with the
   *              current node.
   * @param remove  Whether or not to remove the data once it is found.
   * @return the data associated with the bit string, if any
   * @see #find(BitString)
   */
template <class T>
T * RadixTree<T>::find_helper(TreeNodeType * node, BitString bs, int pos,
                              bool remove) {

    if (pos == bs.size()) {
      // this is it
      if (remove) {
        T * o = node->data;
        node->data = NULL; // "remove" it by setting it to null
        return o;
      } else {
        return node->data;
      }
    }

    if (bs.bgetlr(pos) == Bit::zero) {
      // the next bit is a zero, so follow down the left child
      if (node->left == NULL) {
        // there was no entry for the given string
        return NULL;
      }
      return find_helper(node->left, bs, pos+1, remove);
    } else {
      // the next bit is a one, so follow down the right child
      if (node->right == NULL) {
        // there was no entry for the given string
        return NULL;
      }
      return find_helper(node->right, bs, pos+1, remove);
    }
}

  // ----- RadixTree.get_ancestors ----------------------------------------- //
  /**
   * Examines each node in the tree which is associated with a proper prefix of
   * the given binary string, and finds all of the ones which have (non-null)
   * data associated with them.  A list containing the data of each such node
   * is returned.  Note that only proper prefixes are considered, so an exact
   * match does not count.
   *
   * @param bs  The bit string being used to key the search.
   * @return the data from each prefix of the given bit string which has
   *         non-null data
   */
template <class T>
vector<T*> RadixTree<T>::get_ancestors(BitString bs) {
    TpVector tpv;
    return ga_helper(root_node, bs, 0, tpv);
}

  // ----- RadixTree.ga_helper --------------------------------------------- //
  /**
   * A recursive helper for get_ancestors.
   *
   * @param node  The current node being traversed.
   * @param bs    The bit string being used to key the search.
   * @param pos   The position in the bit string associated with the
   *              current node.
   * @param ancestors  A list of data from all ancestors found so far.
   * @return the data from each prefix of the given bit string which has
   *         non-null data
   */
template <class T>
vector<T*> RadixTree<T>::ga_helper(TreeNodeType * node, BitString bs, int pos,
                                 vector<T*> ancestors) {
    if (pos == bs.size()) {
      // We're at the end of the given string, so we can stop searching.
      return ancestors; // data will be empty
    }

    if (node->data != NULL) { // we've found a prefix
      ancestors.push_back(node->data);
    }

    if (bs.bgetlr(pos) == Bit::zero) {
      // the next bit is a zero, so follow down the left child
      if (node->left == NULL) {
        // we're as far as we can go, so return what we've found
        return ancestors;
      }
      return ga_helper(node->left, bs, pos+1, ancestors);
    } else {
      // the next bit is a one, so follow down the right child
      if (node->right == NULL) {
        // we're as far as we can go, so return what we've found
        return ancestors;
      }
      return ga_helper(node->right, bs, pos+1, ancestors);
    }
}

  // ----- RadixTree.oldest_ancestor --------------------------------------- //
  /**
   * Examines each node in the tree which is associated with a proper prefix of
   * the given binary string, and finds the shortest one which has (non-null)
   * data associated with it.  If such a prefix is found, its data is returned;
   * otherwise, null is returned.  Note that only proper prefixes are
   * considered, so an exact match does not count.
   *
   * @param bs  The bit string being used to key the search.
   * @return the data from the shortest prefix of the given bit string which
   *         has non-null data
   */
template <class T>
T * RadixTree<T>::oldest_ancestor(BitString bs) {
    return oa_helper(root_node, bs, 0);
}

  // ----- RadixTree.oa_helper --------------------------------------------- //
  /**
   * A recursive helper for oldest_ancestor.
   *
   * @param node  The current node being traversed.
   * @param bs    The bit string being used to key the search.
   * @param pos   The position in the bit string associated with the
   *              current node.
   * @return the data from the shortest prefix of the given bit string which
   *         has non-null data
   */
template <class T>
T * RadixTree<T>::oa_helper(TreeNodeType * node, BitString bs, int pos) {

    if (pos == bs.size()) {
      // We're at the end of the given string, so we must not have found any
      // prefixes.
      return NULL;
    }

    if (node->data != NULL) { // we've found a prefix
      return node->data;
    }

    if (bs.bgetlr(pos) == Bit::zero) {
      // the next bit is a zero, so follow down the left child
      if (node->left == NULL) {
        // we're as far as we can go, and no prefix was found
        return NULL;
      }
      return oa_helper(node->left, bs, pos+1);
    } else {
      // the next bit is a one, so follow down the right child
      if (node->right == NULL) {
        // we're as far as we can go, and no prefix was found
        return NULL;
      }
      return oa_helper(node->right, bs, pos+1);
    }
}

  // ----- RadixTree.youngest_ancestor ------------------------------------- //
  /**
   * Examines each node in the tree which is associated with a proper prefix of
   * the given binary string, and finds the longest one which has (non-null)
   * data associated with it.  If such a prefix is found, its data is returned;
   * otherwise, null is returned.  Note that only proper prefixes are
   * considered, so an exact match does not count.
   *
   * @param bs  The bit string being used to key the search.
   * @return the data from the longest prefix of the given bit string which has
   *         non-null data
   */
template <class T>
T * RadixTree<T>::youngest_ancestor(BitString bs) {
    return ya_helper(root_node, bs, 0, NULL);
}

  // ----- RadixTree.ya_helper --------------------------------------------- //
  /**
   * A recursive helper for youngest_ancestor.
   *
   * @param node  The current node being traversed.
   * @param bs    The bit string being used to key the search.
   * @param pos   The position in the bit string associated with the
   *              current node.
   * @param best  The current longest prefix found.
   * @return the data from the longest prefix of the given bit string which has
   *         non-null data
   */
template <class T>
T * RadixTree<T>::ya_helper(TreeNodeType * node, BitString bs, int pos, T * best) {

    if (pos == bs.size()) {
      // we're at the end of the given string, so return the best match seen
      return best;
    }

    if (node->data != NULL) {
      // this is the best match so far so save a pointer to the
      // associated data in case it turns out to be the best overall
      best = node->data;
    }

    if (bs.bgetlr(pos) == Bit::zero) {
      // the next bit is a zero, so follow down the left child
      if (node->left == NULL) {
        // we're as far as we can go, so return the best match seen
        return best;
      }
      return ya_helper(node->left, bs, pos+1, best);
    } else {
      // the next bit is a one, so follow down the right child
      if (node->right == NULL) {
        // we're as far as we can go, so return the best match seen
        return best;
      }
      return ya_helper(node->right, bs, pos+1, best);
    }
}

  // ----- RadixTree.get_descendants --------------------------------------- //
  /**
   * Examines each node in the tree whose binary string key has the given
   * binary string as a proper prefix, and finds all of the ones which have
   * (non-null) data associated with them.  A list containing the data of
   * each such node is returned.  Note that only proper prefixes are
   * considered, so an exact match does not count.
   *
   * @param bs  The bit string which indicates the node to look for descendants
   *            of.
   * @return a vector containing the data from all descendants that have
   *         (non-null) data
   */
template <class T>
vector<T*> RadixTree<T>::get_descendants(BitString bs) {
    vector<T*> tvp;
    return gd_helper(root_node, bs, 0, tvp);
}

  // ----- RadixTree.gd_helper --------------------------------------------- //
  /**
   * A recursive helper for get_descendants.
   *
   * @param node  The current node being traversed.
   * @param bs    The bit string being used to key the search.
   * @param pos   The position in the bit string associated with the
   *              current node.
   * @param descendants  A vector of data from all descendants found so far.
   * @return a list containing the data from all descendants that have
   *         (non-null) data
   */
template <class T>
vector<T*> RadixTree<T>::gd_helper(TreeNodeType * node, BitString bs, int pos,
                     vector<T*> descendants) {
    if (pos < bs.size()) { // not yet to the node in question
      if (bs.bgetlr(pos) == Bit::zero) { // next bit = 0, follow left child
        if (node->left == NULL) {
          return descendants; // no left child exists
        }
        return gd_helper(node->left, bs, pos+1, descendants);
      } else { // next bit = 1, follow right child
        if (node->right == NULL) {
          return descendants; // no right child exists
        }
        return gd_helper(node->right, bs, pos+1, descendants);
      }
    } else { // we're either at the node in question or one of its descendants
      if (pos > bs.size() && node->data != NULL) {
        descendants.push_back(node->data); // we're at a descendant it has data
      }
      if (node->left == NULL && node->right == NULL) {
        return descendants; // no children
      } else if (node->left == NULL) { // right child only
        return gd_helper(node->right, bs, pos+1, descendants);
      } else if (node->right == NULL) { // left child only
        return gd_helper(node->left, bs, pos+1, descendants);
      } else { // both children
        descendants = gd_helper(node->left, bs, pos+1, descendants);
        return gd_helper(node->right, bs, pos+1, descendants);
      }
    }
}

  // ----- RadixTree.has_descendants --------------------------------------- //
  /**
   * Determines whether or not any descendants of a given node have (non-null)
   * data.
   *
   * @param bs  The bit string which indicates the node to look for descendants
   *            of.
   * @return true only if at least one descendant has (non-null) data
   */
template <class T>
bool RadixTree<T>::has_descendants(BitString bs) {
    return hd_helper(root_node, bs, 0);
}

  // ----- RadixTree.hd_helper --------------------------------------------- //
  /**
   * A recursive helper for has_descendants.
   *
   * @param node  The current node being traversed.
   * @param bs    The bit string being used to key the search.
   * @param pos   The position in the bit string associated with the
   *              current node.
   * @param best  The current longest prefix found.
   * @return the data from the longest prefix of the given bit string which has
   *         non-null data
   */
template <class T>
bool RadixTree<T>::hd_helper(TreeNodeType * node, BitString bs, int pos) {
    if (pos < bs.size()) { // not yet to the node in question
      if (bs.bgetlr(pos) == Bit::zero) { // next bit = 0, follow left child
        if (node->left == NULL) {
          return false; // no left child exists
        }
        return hd_helper(node->left, bs, pos+1);
      } else { // next bit = 1, follow right child
        if (node->right == NULL) {
          return false; // no right child exists
        }
        return hd_helper(node->right, bs, pos+1);
      }
    } else { // we're either at the node in question or one of its descendants
      if (pos > bs.size() && node->data != NULL) {
        return true; // we're at a descendant it has data
      }
      if (node->left == NULL && node->right == NULL) {
        return false; // no children
      } else if (node->left == NULL) { // right child only
        return hd_helper(node->right, bs, pos+1);
      } else if (node->right == NULL) { // left child only
        return hd_helper(node->left, bs, pos+1);
      } else { // both children
        return hd_helper(node->left, bs, pos+1) ||
               hd_helper(node->right, bs, pos+1);
      }
    }
}

 // ----- RadixTree.remove ------------------------------------------------ //
  /**
   * Removes and returns the data (if any) associated with the given
   * binary string.
   *
   * @param bs  The bit string being used to key the removal.
   * @return the data being removed, if any
   */
template <class T>
T * RadixTree<T>::remove(BitString bs) {
    // find() is so similar to remove that we can just reuse that code
    // with only one added parameter to indicate whether or not to remove
    return find_helper(root_node, bs, 0, true);
}

  // ----- RadixTree.prune ------------------------------------------------- //
  /**
   * Prunes the subtree rooted at the node associated with the given
   * binary string.
   *
   * @param bs  The bit string being used to key the pruning.
   */
template <class T>
void RadixTree<T>::prune(BitString bs) {
    if (bs.size() == 0) {
      // An empty bit string is a special case, since we never set the
      // root node to null.
      root_node->data  = NULL;
      root_node->left  = NULL;
      root_node->right = NULL;
    } else {
      if (bs.size() == 1) {
        if (bs.bgetlr(0) == Bit::zero) {
          root_node->left = NULL;
        } else {
          root_node->right = NULL;
        }
      } else {
        prune_helper(root_node, bs, 0);
      }
    }
}

  // ----- RadixTree.prune_helper ------------------------------------------ //
  /**
   * A recursive helper for prune.
   *
   * @param node  The current node being traversed.
   * @param bs    The bit string being used to key the pruning.
   * @param pos   The position in the bit string associated with the
   *              current node.
   */
template <class T>
void RadixTree<T>::prune_helper(TreeNodeType * node, BitString bs, int pos) {
    if ((pos+1) == bs.size()) {
      // one of this node's children is the root of the subtree to be pruned
      if (bs.bgetlr(pos) == Bit::zero) {
        node->left = NULL;
      } else {
        node->right = NULL;
      }
    } else {
      // neither child of this node is the root of the subtree to be pruned
      if (bs.bgetlr(pos) == Bit::zero) {
        // the next bit is a zero, so follow down the left child
        if (node->left != NULL) {
          prune_helper(node->left, bs, pos+1);
        }
      } else {
        // the next bit is a one, so follow down the right child
        if (node->right != NULL) {
          prune_helper(node->right, bs, pos+1);
        }
      }
    }
}

  // ----- RadixTree.print ------------------------------------------------- //
  /**
   * Prints all strings in the tree.  We define a string as being "in"
   * the tree if its associated data is non-null.
   */
template <class T>
void RadixTree<T>::print() {
    print_helper(root_node, "");
}

  // ----- RadixTree.print_helper ------------------------------------------ //
  /**
   * A recursive helper for print.
   *
   * @param node  The current node being traversed.
   * @param str   The binary string for the current node in String format.
   */
template <class T>
void RadixTree<T>::print_helper(TreeNodeType * node, string str) {
    if (node->data != NULL) {
      // we've found a string with an entry, so print it (I put quotes
      // around each string so that it's easier to tell whether or not
      // the null string is in the tree)
     cout << "\n\"" << str << "\"";
    }

    if (node->left != NULL) {
      print_helper(node->left, str+"0");
    }
    if (node->right != NULL) {
      print_helper(node->right, str+"1");
    }
}


#endif
