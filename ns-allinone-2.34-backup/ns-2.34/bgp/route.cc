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
      

#include "route.h"

  // ----- constructor Route() --------------------------------------------- //
  /**
   * Default initializer.
   */
Route::Route(){
   // We initialize the array so that we can index it by attribute type code.
   // It's easier than having an unordered array and having to search through
   // it each and every time when looking for a particular attribute.  This
   // way is less space-efficient, but more time-efficient.
   pas = new Attribute *[Attribute::MAX_TYPECODE+1];
   for (int i =0;i< Attribute::MAX_TYPECODE+1;i++) {
        pas[i] = NULL;
   }
     
   // All mandatory attributes should be set immediately after a route is
   // constructed.  Here we initialize the AS path only, because an empty AS
   // path can be valid for routes originated by the local BGP speaker.
   vector<Segment*> sv;    
   pas[ASpath::TYPECODE] = new ASpath(sv);       
}
     
  // ----- constructor Route(IPaddress,Attribute[]) ------------------------ //
  /**
   * Constructs a route given the destination and path attributes.
   *
   * @param ipa  The destination of the route.
   * @param a   The path attributes associated with this route.
   */
Route::Route(IPaddress * ipa, Attribute ** a) {
  nlri = ipa;
  pas = a;
}

  // ----- constructor Route(Route) ---------------------------------------- //
  /**
   * Constructs a new route based on an existing one.
   *
   * @param rte  The existing route on which to base the new one.
   */
Route::Route(Route * rte) {
  nlri = new IPaddress(*(rte->nlri));
  pas = copy_attribs(rte->pas); 
}

  // ----- Route::copy_attribs ---------------------------------------------- //
  /**
   * Copies the array of path attributes into a new array and returns it.
   *
   * @param pas  An array of path attributes.
   * @return a copy of the array of path attributes
   */
Attribute ** Route::copy_attribs(Attribute ** pas) {   
    if (pas == NULL) {
      return NULL;
    }
    Attribute **  attribs = new Attribute *[Attribute::MAX_TYPECODE+1];
    for (int i =0;i< Attribute::MAX_TYPECODE+1;i++) {
      if (pas[i] != NULL) {
        attribs[i] = pas[i]->copy();
      } else {
        attribs[i] = NULL;
      }  
    }
    return attribs;
}

  // ----- constructor Route(Route) ---------------------------------------- //
  /**
   * Constructs a new route based on a char stream.
   *
   * @param stream   The char stream contains the route.
   * @param rt  The char stream which to base the new one.
   */
Route::Route(char * stream, IPaddress * rt) {

  pas = new Attribute *[Attribute::MAX_TYPECODE+1];
  for (int i =0;i< Attribute::MAX_TYPECODE+1;i++) {
        pas[i] = NULL;
  }

  int pas_len = ((unsigned int) stream[0])*256 +(unsigned int)stream[1];
  // k point to head of each pa   
  int k=2;
  while (k < pas_len +2) {  // stop before the nlri
    // the second byte of each pa contain pa_type
    int  pa_type = (unsigned int) stream[k+1];
    // the third byte of each pa contain pa_type 
    int  pa_len =  (unsigned int) stream[k+2];
            
    switch (pa_type) {
      case  Origin::TYPECODE :
        pas[Origin::TYPECODE] = new Origin(stream+k);
      break;
      case  ASpath::TYPECODE :
        pas[ASpath::TYPECODE] = new ASpath(stream+k);
      break;
      case  NextHop::TYPECODE :
        pas[NextHop::TYPECODE] = new NextHop(stream+k);
      break;
      case  MED::TYPECODE :
        pas[MED::TYPECODE] = new MED(stream+k);
      break;
      case  LocalPref::TYPECODE :
        pas[LocalPref::TYPECODE] = new LocalPref(stream+k);
      break;
      case  AtomicAggregate::TYPECODE :
        pas[AtomicAggregate::TYPECODE] = new AtomicAggregate(stream+k);
      break;
      case  Aggregator::TYPECODE :
        pas[Aggregator::TYPECODE] = new Aggregator(stream+k);
      break;
      case  Community::TYPECODE :
      //  The Community path attribute is not currently implemented.
      //  pas[Community::TYPECODE] = new Community(stream+k);
      break;
      case  OriginatorID::TYPECODE :
        pas[OriginatorID::TYPECODE] = new OriginatorID(stream+k);
      break;
      case  ClusterList::TYPECODE :
        pas[ClusterList::TYPECODE] = new ClusterList(stream+k);
      break;
    }
    k += pa_len;
  }
  nlri = rt;  
}     

Route::~Route(){
     for (int i =0;i< Attribute::MAX_TYPECODE+1;i++) {
        if (pas[i] != NULL) {
          delete pas[i];
        }
    }
}

  // ----- Route::aggregator ------------------------------------------------ //
  /**
   * Returns the aggregator path attribute.
   *
   * @return the aggregator path attribute
   */
Aggregator * Route::aggregator() {
    return (Aggregator *)pas[Aggregator::TYPECODE];
}

  // ----- Route::aspath ---------------------------------------------------- //
  /**
   * Returns the AS path attribute.
   *
   * @return the AS path attribute
   */
ASpath * Route::aspath(){
  return (ASpath *)pas[ASpath::TYPECODE];
}

  // ----- Route::cluster_list ---------------------------------------------- //
  /**
   * Returns the cluster list path attribute.
   *
   * @return the cluster list path attribute
   */
ClusterList * Route::cluster_list() {
    return (ClusterList*)pas[ClusterList::TYPECODE];
}

  // ----- Route::localpref ------------------------------------------------- //
  /**
   * Returns the value of the local preference attribute.
   *
   * @return the local preference of the route
   */
int Route::localpref(){
  if ( has_localpref() ) {
    return  ((LocalPref *)pas[LocalPref::TYPECODE])->val;
  } else {
    return 0;
  }
}

  // ----- Route::med ------------------------------------------------------- //
  /**
   * Returns the value of the multiple exit discriminator attribute.
   *
   * @return the multiple exit discriminator value
   */
int Route::med(){
  if ( has_med() ) {
    return ((MED *)pas[MED::TYPECODE])->val;
  } else {
    return 0;
  }
}

 // ----- Route::nexthop --------------------------------------------------- //
  /**
   * Returns the value of the next hop path attribute.
   *
   * @return the value of the next hop path attribute
   */
IPaddress * Route::nexthop(){
  if ( pas[NextHop::TYPECODE]!= NULL ) {
    return ((NextHop *)pas[NextHop::TYPECODE])->ipaddr;
  }  else {
    return NULL;    
  }
}                                     

  // ----- Route::origin ---------------------------------------------------- //
  /**
   * Returns the value of the origin path attribute.
   *
   * @return the value of the origin path attribute
   */
int Route::origin(){
  if ( pas[Origin::TYPECODE]!= NULL ) {
    return ((Origin *)pas[Origin::TYPECODE])->type;
  }  else {
    return 0;
  } 
}

  // ----- Route::orig_id --------------------------------------------------- //
  /**
   * Returns the value of the originator ID attribute.
   *
   * @return the originator ID of the route
   */
IPaddress *  Route::orig_id(){
  if ( has_orig_id() ) {
    return ((OriginatorID*)pas[OriginatorID::TYPECODE])->id;
  } else {
    return NULL;
  }   
}

bool Route::has_aspath() {
    return (pas[ASpath::TYPECODE] != NULL);
}              

  // ----- Route::has_atomicagg --------------------------------------------- //
  /**
   * Returns whether or not the atomic aggregate attribute exists.
   *
   * @return true only if the atomic aggregate attribute exists
   */
bool Route::has_atomicagg() {
    return (pas[AtomicAggregate::TYPECODE] != NULL);
}

  // ----- Route::has_aggregator -------------------------------------------- //
  /**
   * Returns whether or not the aggregator attribute exists.
   *
   * @return true only if the aggregator attribute exists
   */
bool Route::has_aggregator() {
   return (pas[Aggregator::TYPECODE] != NULL);
}

  // ----- Route::has_cluster_list ------------------------------------------ //
  /**
   * Returns whether or not the cluster list attribute exists.
   *
   * @return true only if the cluster list attribute exists
   */
bool Route::has_cluster_list() {
    return (pas[ClusterList::TYPECODE] != NULL);
}

  // ----- Route::has_med --------------------------------------------------- //
  /**
   * Returns whether or not the multiple exit discriminator path attribute
   * exists.
   *
   * @return true only if the MED path attribute exists
   */
bool Route::has_med(){
  return (pas[MED::TYPECODE] != NULL);
}

  // ----- Route::has_localpref --------------------------------------------- //
  /**
   * Returns whether or not the local preference path attribute exists.
   *
   * @return true only if the local preference path attribute exists
   */
bool Route::has_localpref(){
  return (pas[LocalPref::TYPECODE] != NULL);
}

  // ----- Route::has_orig_id ----------------------------------------------- //
  /**
   * Returns whether or not the originator ID attribute exists.
   *
   * @return true only if the originator ID attribute exists
   */
bool Route::has_orig_id(){
    return (pas[OriginatorID::TYPECODE] != NULL);
}

  // ----- Route::prepend_as ------------------------------------------------ //
  /**
   * Prepends an AS NHI prefix address to the AS path attribute.  Creates the
   * attribute if it does not yet exist.
   *
   * @param asnh  The AS NHI prefix address to prepend to the AS path.
   */
void Route::prepend_as(string asnh){
    if (aspath() == NULL) {
      segVector sv;
      sv.clear();
      pas[ASpath::TYPECODE] = new ASpath(sv);
    }
    aspath()->prepend_as(asnh);
}

  // ----- Route::append_cluster -------------------------------------------- //
  /**
   * Appends a cluster number to the cluster list attribute.  The attribute is
   * created if it does not yet exist.
   *
   * @param cnum  The cluster number to add to the cluster list.
   */
void Route::append_cluster(int cnum) {
    if (!has_cluster_list()) {
      intVector intv;
      pas[ClusterList::TYPECODE] = new ClusterList(intv);
    }
    ((ClusterList*)pas[ClusterList::TYPECODE])->append(cnum);
}

  // ----- Route::remove_attrib --------------------------------------------- //
  /**
   * Removes a path attribute from the route.
   *
   * @param typ  The type code of the path attribute to remove.
   */
void Route::remove_attrib(int type){
  pas[type] = NULL;
}

  // ----- Route::set_atomicagg --------------------------------------------- //
  /**
   * Adds the atomic aggregate path attribute to the route.
   */
void Route::set_atomicagg() {
    if (!has_atomicagg()) {
      pas[AtomicAggregate::TYPECODE] = new AtomicAggregate();
    }
}

  // ----- Route::set_aggregator -------------------------------------------- //
  /**
   * Sets the value of the aggregator path attribute.  The attribute is created
   * if it does not yet exist.
   *
   * @param nh   The AS NHI address prefix of the aggregating BGP speaker.
   * @param ipa  The IP address of the aggregating BGP speaker.
   */
void Route::set_aggregator(string nh, IPaddress * ipa) {
    if (has_aggregator()) {
      aggregator()->asnh = nh;
      aggregator()->ipaddr = ipa;
    } else {
      pas[Aggregator::TYPECODE] = new Aggregator(nh,ipa);
    }
}

  // ----- Route::set_localpref --------------------------------------------- //
  /**
   * Sets the value of the local preference path attribute.  The attribute is
   * created if it does not yet exist.
   *
   * @param pref  The value to use for the local preference attribute.
   */
void Route::set_localpref(int pref){
   if (has_localpref()) {
      ((LocalPref*)pas[LocalPref::TYPECODE])->val = pref;
    } else {
      pas[LocalPref::TYPECODE] = new LocalPref(pref);
    }
}

  // ----- Route::set_med --------------------------------------------------- //
  /**
   * Sets the value of the multiple exit discriminator path attribute.  The
   * attribute is created if it does not yet exist.
   *
   * @param v  The value to use for the MED.
   */
void Route::set_med(int v){
  if (has_med()) {
      ((MED*)pas[MED::TYPECODE])->val = v;
    } else {
      pas[MED::TYPECODE] = new MED(v);
    }
}                         

  // ----- Route::set_origin ------------------------------------------------ //
  /**
   * Sets the value of the origin path attribute.  If the attribute does not
   * yet exist, it is created.
   *
   * @param t  An origin type code.
   */
void Route::set_origin(int type){
  if (pas[Origin::TYPECODE] != NULL) {
      ((Origin*)pas[Origin::TYPECODE])->type = type;
    } else {
      pas[Origin::TYPECODE] = new Origin(type);
    }
}

  // ----- Route::set_nexthop ----------------------------------------------- //
  /**
   * Sets the next hop path attribute for this route.  Creates the attribute
   * every time in case the pointer parameter changes.
   *
   * @param nexthop  The value to which to set the next hop attribute.
   */
void Route::set_nexthop(IPaddress * nexthop){
      pas[NextHop::TYPECODE] = new NextHop(nexthop);
}

  // ----- Route::set_orig_id ----------------------------------------------- //
  /**
   * Sets the value of the originator ID path attribute.
   */
void Route::set_orig_id(IPaddress *  orig_id) {
      pas[OriginatorID::TYPECODE] = new OriginatorID(orig_id);
}

  // ----- Route::set_nlri -------------------------------------------------- //
  /**
   * Sets the value of the NLRI.
   *
   * @param ipa  The value to use for the NLRI.
   */
void Route::set_nlri(IPaddress * ipaddr){
  nlri = ipaddr;
}   

  // ----- Route::pas_bytecount --------------------------------------------------- //
  /**
   * Returns the length of path attributes of the route in byte.
   *
   * @return the length of path attributes of the route in byte
   */
int Route::pas_bytecount(){
  int  pas_bc=0;
  for (int i=1; i< Attribute::MAX_TYPECODE+1; i++) {
      if (pas[i] != NULL)    {
        pas_bc += pas[i]->bytecount();
      }
   }
   return pas_bc;
}                                                                  

  // ----- Route::incorporate_route ----------------------------------------- //
  /**
   * Incorporates the given route into this one (aggregates the two).
   *
   * @param r  The route to be aggregated into this one.
   */
void Route::incorporate_route(Route * r){   

    // - - - - - aggregate the origins - - - - -
   
    if (origin() == Origin::INC || r->origin() == Origin::INC) {
      // if either was INCOMPLETE, the aggregate must be INCOMPLETE
      set_origin(Origin::INC);
    } else if (origin() == Origin::EGP || r->origin() == Origin::EGP) {
      // else if either was EGP, the aggregate must be EGP
      set_origin(Origin::EGP);
    } else {
      // in all other cases, the aggregated value is IGP
      set_origin(Origin::IGP);
    }

    // - - - - - aggregate the AS paths - - - - -

    // I tried to follow the algorithm given in Appendix 6.8 of
    // RFC1771, but I find it unbelievably lacking in detail and
    // clarity.  Oh well, here goes ...

    // set up arrays of the ASes for convenience

    string pa1as[aspath()->length()];
    string pa2as[r->aspath()->length()];
    int pa1type[aspath()->length()];
    int pa2type[r->aspath()->length()];

    int nas1 = 0;
    for (int i=0; i<aspath()->segs.size(); i++) {
      Segment * seg = aspath()->segs[i];   
      for (int j=0; j<seg->size(); j++, nas1++) {
        pa1as[nas1]  = seg->asnhs[j];
        pa1type[nas1] = seg->type; 
      }
    }

    int nas2 = 0;
    for (int i=0; i< r->aspath()->segs.size(); i++) {
      Segment * segr = r->aspath()->segs[i];
      for (int j=0; j<segr->size(); j++, nas2++) {
        pa2as[nas2]  = segr->asnhs[j];
        pa2type[nas2] = segr->type;
      }
    }

    string pa3as [nas1+nas2];
    int pa3type [nas1+nas2];

    int nas3 = 0;
    int j;
    for (int i=0; i<nas1; i++) {
      // first, do a quick check to see if this AS appears at all in
      // the other AS path
      bool appears = false;
      for (int k=0; k<nas2; k++) {
        if ((pa2as[k]==pa1as[i]) && (pa2type[k] == pa1type[i])) {
          appears = true;
          break;
        }
      }

      if (appears) {   
        // it's in both AS paths
        j = 0;
        while (j < nas2 &&
               (pa2as[j] == "" ||
                !(pa2as[j]==pa1as[i]) ||
                pa2type[j] != pa1type[i])) {
          // put any intervening ASes into an AS_SET
          if (pa2as[j] != "") {
            pa3as[nas3]   = pa2as[j];
            pa3type[nas3] = Segment::SET;
            pa2as[j] = ""; // mark it so it's not included again
            nas3++; 
          }
          j++;
        }    

        if (j < nas2 && pa2as[j]==pa1as[i]) {
          // it's the "same AS" in the second AS path
          pa3as[nas3] = pa1as[i];
          // make it a sequence no matter what the type was before
          // (dunno if this is the correct thing to do ...)
          pa3type[nas3] = Segment::SEQ;
          pa2as[j] = ""; // mark it so it's not included again
          nas3++;  
        }
      } else {
        // it's only in the first AS path
        pa3as[nas3]   = pa1as[i];
        pa3type[nas3] = pa1type[i];
        nas3++;   
      }
    }  

    // put the remaining ASes (if any) from the second AS path into an AS_SET
    for (j=0; j<nas2; j++) {
      if (pa2as[j] != "") {
        pa3as[nas3]   = pa2as[j];
        pa3type[nas3] = Segment::SEQ;
        nas3++;
      }
    } 

    // clean out any repeats
    for (int m=0; m<nas3-1; m++) {
      for (int n=m+1; n<nas3; n++) {
        if (pa3as[m]==(pa3as[n])) {
          pa3as[m] = "";
          break;
        }
      }
    }  

    // convert from array back to path segments
    segVector segv;
    strVector strv;
    pas[ASpath::TYPECODE] = new ASpath(segv);
    int cur_seg_type = -1;
    Segment * cur_seg = NULL ;
    for (int p=0; p<nas3; p++) {
      if (pa3as[p] != "") {
        // ignore -1's (they were repeats)
        if (cur_seg_type == pa3type[p]) {
          cur_seg->append_as(pa3as[p]);
        } else {
          // we'll have to start a new segment of the opposite type
          if (cur_seg != NULL) {
            // add the previous segment to the AS path
            aspath()->append_segment(cur_seg);
          }
          // now start a new segment
          cur_seg = new Segment(pa3type[p], strv);
          cur_seg->append_as(pa3as[p]);
          cur_seg_type = pa3type[p];
        }
      }
    }
    if (cur_seg != NULL) {
      // append the last segment
      aspath()->append_segment(cur_seg);
    }  

    // - - - - - aggregate the next hops - - - - -

    //   The two routes have identical NEXT_HOP attributes, or else
    //   this routine would not have been called.  The aggregate
    //   maintains this same value for next hop.

    // - - - - - aggregate the MEDs - - - - -

    //   The two routes have identical MULTI_EXIT_DISC attributes, or
    //   else this routine would not have been called.  The aggregate
    //   maintains this same value for MULTI_EXIT_DISC.

    // - - - - - aggregate the local preference attributes - - - - -

    //   This attribute is only used for local (within the same AS)
    //   messages, so it can be set to NULL.
    remove_attrib(LocalPref::TYPECODE);

    // - - - - - aggregate the atomic aggregate attributes - - - - -

    // if either has ATOMIC_AGGREGATE, the aggregate must have ATOMIC_AGGREGATE
    if (!has_atomicagg() && r->has_atomicagg()) {
      set_atomicagg();
    }
 
    // - - - - - aggregate the aggregator attributes - - - - -

    //   this is always ignored when aggregating routes
    remove_attrib(Aggregator::TYPECODE); 
}

  // ----- Route::equals ---------------------------------------------------- //
  /**
   * Returns true only if the two routes have the same NLRI and path
   * attributes.
   *
   * @param rte  The route to compare with this one.
   * @return  true only if the two routes have the same NLRI and path
   *          attributes
   */
bool Route::equals(Route *rte){
   return ( rte != NULL &&
            typeid(*rte) == typeid(Route) &&
            nlri->equals(((Route*)rte)->nlri) &&
            equal_attribs(((Route*)rte)->pas));
}
                                                                                 
  // ----- Route::equal_attribs --------------------------------------------- //
  /**
   * Returns true only if the two routes have equivalent path attributes.
   *
   * @param attribs  The path attributes to be compared.
   * @return true only if the two sets of path attributes are equivalent
   */
bool Route::equal_attribs(Attribute ** attribs){
    bool same = true;
    for (int i=1; i< Attribute::MAX_TYPECODE+1; i++) {
      if ((pas[i] == NULL && attribs[i] != NULL) ||
          (pas[i] != NULL && attribs[i] == NULL)) {
        return false;
      }
      if (pas[i] != NULL && !pas[i]->equals(attribs[i])) {
        return false;
      }
    }
    return true;   
}

  // ----------- Route::toStream --------------------------------------------- //
  /**
   * Returns a char stream represenation of path attributes of the route.
   *
   * @return a char stream representation of path attributes of the route
   */ 
void Route::pasToStream(char * stream) {  
  int k=0;
  for (int i=1; i< Attribute::MAX_TYPECODE+1; i++) {
    if (pas[i] != NULL)    {
      int size = pas[i]->bytecount();
      pas[i]->toStream(stream+k);
      k += size;
    }
  }
}

  // ----------- Route::toStream --------------------------------------------- //
  /**
   * Returns a char stream represenation of nlri of the route.
   *
   * @return a char stream representation of nlri of the route
   */ 
void Route::nlriToStream(char * stream){
  stream[0] = nlri->prefix_len();
  nlri->masked_toStream(stream+1);   
}

  // ---------------- Route::toString ------------------------------------------ //
  /**
   * Returns a string describing the contents of the route, mainly for debug usage.
   *
   * @return a string representation of the route
   */
string Route::toString() {
    string str="";
    for (int i=1; i< Attribute::MAX_TYPECODE+1; i++) {
      if (pas[i] != NULL)         
        str += pas[i]->toString() + " ";        
    }
    str += nlri->toString();
    return str;
}
  
  

