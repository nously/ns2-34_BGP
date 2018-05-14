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


#include "ribelement.h"
#include "rtProtoBGP.h" 
     
  // ----- constructor RIBElement ------------------------------------------ //
  /**
   * Constructs a RIB element with a pointer to the local BGP protocol
   * agent.
   */
RIBElement::RIBElement(rtProtoBGP *b):RadixTree<RouteInfo>(){
  bgp = b;
  version = 0;
}             

RIBElement::~RIBElement() {}

  // ----- RIBElement.find ------------------------------------------------- //
  /**
   * Attempts to find information for a given destination.
   *
   * @param ipa  The destination IP address prefix to search for info about.
   * @return the route information if the prefix is found, null if not
   */
RouteInfo *  RIBElement::find(IPaddress * ipa) {
    RouteInfo *  oldinfo =  rtes[ipa->val()];
    return oldinfo;
}

  // ----- RIBElement::add -------------------------------------------------- //
  /**
   * Adds route information.  If route information with the same NLRI already
   * exists, then the add fails and the pre-existing information is returned.
   *
   * @param info  The route information to add.
   * @return null if the add was successful, the pre-existing route information
   *         otherwise
   */
RouteInfo * RIBElement::add(RouteInfo * info) {
    RouteInfo * oldinfo  = RadixTree<RouteInfo>::add(
                                *(info->route->nlri->prefix_bits()), info);
    rtes[info->route->nlri->val()]= info;
    version++;
    return oldinfo;
}

  // ----- RIBElement::replace ---------------------------------------------- //
  /**
   * Adds route information, replacing any pre-existing information with the
   * same NLRI.
   *
   * @param info  The route information to add.
   * @return the pre-existing route information, or null if there was none
   */
RouteInfo *  RIBElement::replace(RouteInfo *info) {
    RouteInfo * oldinfo  = RadixTree<RouteInfo>::replace(
                                *(info->route->nlri->prefix_bits()), info);
    rtes[info->route->nlri->val()]= info;
    version++;
    return oldinfo;
}

  // ----- RIBElement.remove ----------------------------------------------- //
  /**
   * Removes the route information corresponding to the given route
   * destination.
   *
   * @param ipa  The destination of the route information to remove.
   * @return the removed route information
   */
RouteInfo * RIBElement::remove(IPaddress * ipa) {
   RouteInfo *  oldinfo  =  RadixTree<RouteInfo>::remove(*(ipa->prefix_bits()));
    rtes[ipa->val()]= NULL;
    version++;
    return oldinfo;
  }                        

  // ----- RIBElement.remove_all ------------------------------------------- //
  /**
   * Removes all route information in the RIB element and returns it as a list.
   *
   * @return a list of removed route information
   */
vector<RouteInfo *> RIBElement::remove_all() {
    vector<RouteInfo *> allrtes;
    map<unsigned int,RouteInfo*>::iterator iter;
    for (iter=rtes.begin(); iter!=rtes.end(); iter++) {
      RouteInfo * ri = iter->second;
      if (ri !=NULL) {
        (*iter).second = NULL;
        RadixTree<RouteInfo>::remove(*(ri->route->nlri->prefix_bits()));
        allrtes.push_back(ri);
      }  
    }
    return allrtes;
}  

  // ----- RIBElement.get_all_routes --------------------------------------- //
  /**
   * Constructs and returns a vector of all route information in the RIB element.
   *
   * @return a vector of all route information in the RIB element
   */
vector<RouteInfo *> RIBElement::get_all_routes() {
    vector<RouteInfo *> allrtes;
    map<unsigned int,RouteInfo*>::iterator iter;
    for (iter=rtes.begin(); iter!=rtes.end(); iter++) {
      RouteInfo * ri = iter->second;
      if (ri !=NULL) {
          allrtes.push_back(ri);
      }
    }
    return allrtes;
}


  // ----- RIBElement.get_less_specifics ----------------------------------- //
  /**
   * Finds any routes with overlapping but less specific NLRI than the given
   * IP address prefix.
   *
   * @param ipa  An IP address prefix to find less specific NLRI than.
   * @return a list of routes with overlapping but less specific NLRI
   */
vector<RouteInfo*> RIBElement::get_less_specifics(IPaddress *ipa) {
    return RadixTree<RouteInfo>::get_ancestors(*(ipa->prefix_bits()));
}

  // ----- RIBElement.get_less_specifics ----------------------------------- //
  /**
   * Finds any routes with overlapping but more specific NLRI than the given
   * IP address prefix.
   *
   * @param ipa  An IP address prefix to find more specific NLRI than.
   * @return a list of routes with overlapping but more specific NLRI
   */
vector<RouteInfo*> RIBElement::get_more_specifics(IPaddress * ipa) {
    return RadixTree<RouteInfo>::get_descendants(*(ipa->prefix_bits()));
}

  // ----- RIBElement.is_less_specific ------------------------------------- //
  /**
   * Determines if there are any routes with more specific NLRI than the given
   * IP address prefix.
   *
   * @param ipa  An IP address prefix to find more specific NLRI than.
   * @return true only if there is at least one route with more specific NLRI
   */
bool RIBElement::is_less_specific(IPaddress * ipa) {
    return RadixTree<RouteInfo>::has_descendants(*(ipa->prefix_bits()));
}

  // ----- RIBElement::toString() ------------------------------------------- //
  /**
   * Returns this RIB element as a string.
   *
   * @return this RIB element as a string
   */
string RIBElement::toString() {
  // header 
  string str = "BGP table version is";
  char * temp = new char[10];
  sprintf(temp," %d, ",version);
  str += string(temp);
  str += "local router ID is " + bgp->bgp_id->val2str() + "\n";
  str += "Status codes: * valid, > best, i - internal.\n";
  str += "     Network            Next Hop       Metric  LocPrf  Weight Path \n";
  // entries
  str += toString_helper(root());
  delete[] temp;
  return str;
}

  // ----- RIBElement=::toString_helper ----------------------------- //
  /**
   * Returns this RIB element as a string
   *
   * @param node  the root node of the RadixTree to start with.
   * @return this RIB element as a string
   */
string RIBElement::toString_helper(RadixTreeNode<RouteInfo> * node) {
  string str = "";
    if (node->data != NULL) {
      str = node->data->toString();
    }      
    if (node->left != NULL) {
      str = str + toString_helper(node->left);
    }
    if (node->right != NULL) {
      str = str + toString_helper(node->right);
    } 
    return str; 
}


