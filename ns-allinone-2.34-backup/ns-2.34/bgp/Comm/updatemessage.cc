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
 

#include "updatemessage.h"
#include <math.h>
#include "../Path/aspath.h"

  // ----- constructor UpdateMessage(String) ------------------------------- //
  /**
   * Constructs the update message with default values.
   *
   * @param nh The NHI address prefix of the sender of this message.
   */
UpdateMessage::UpdateMessage(string nh) 
  :BGPMessage(BGPMessage::UPDATE, nh) {
    rtes.clear();
    wds.clear();
}

  // ----- constructor UpdateMessage(String,Route) ------------------------- //
  /**
   * Constructs the update message with the given feasible route.
   *
   * @param nh   The NHI address prefix of the sender of this message.
   * @param rte  The route to advertise in this message.
   */
UpdateMessage::UpdateMessage(string nh, Route *rte) 
  :BGPMessage(BGPMessage::UPDATE, nh) {
    rtes.clear();
    wds.clear();
    add_route(rte);
}

  // ----- constructor UpdateMessage(String,IPaddress) --------------------- //
  /**
   * Constructs the update message with the given infeasible NLRI.
   *
   * @param nh   The NHI address prefix of the sender of this message.
   * @param wdnlri  The NLRI with withdraw with this message.
   */
UpdateMessage::UpdateMessage(string nh, IPaddress *wdnlri) 
  :BGPMessage(BGPMessage::UPDATE, nh) {
    rtes.clear();
    wds.clear();
    add_wd(wdnlri);
}

  // ----- constructor UpdateMessage(unsigned char*,string) --------------------- //
  /**
   * Constructs the update message with the given infeasible NLRI.
   *
   * @param stream  The char stream representation of this message.
   * @param nh  The NLRI of withdrawl with this message.
   */   
UpdateMessage::UpdateMessage(unsigned char* stream,string nh)
  :BGPMessage(BGPMessage::UPDATE, nh) {
    rtes.clear();
    wds.clear();

    // parse Unfeasible routes length (2 bytes)
    int wds_len = ((unsigned int) stream[19])*256 +(unsigned int)stream[20];
    // advance k to the first withdraw route
    int k=21;
    for (int i=0;i<wds_len;i++) {
      int wd_prefix_len = (unsigned int) stream[k++];
      IPaddress * wd = new IPaddress(stream+k,wd_prefix_len);
      add_wd(wd);
      k += wd->masked_bytes();
    }

    // parse total path attribute length (2 bytes)
    int pas_len = ((unsigned int) stream[k])*256 +(unsigned int)stream[k+1];
    // buffer the path attribute stream for the creation of nlri's
    char * pas_buffer = new char[pas_len+4];
    for (int i=0;i<pas_len+2;i++) {
      pas_buffer[i] = stream[k+i];
    }
    
    // advance k to the first nlri
    k += pas_len+2; 
    int total_length = ((unsigned int)stream[16]) * 256 +
                        (unsigned int)stream[17];                           
    // parse rtes                                 
    while (k< total_length) {
      int nlri_prefix_len = (unsigned int) stream[k++];
      IPaddress * nlri = new IPaddress(stream+k,nlri_prefix_len);
      Route * rt =  new Route(pas_buffer, nlri);
      add_route(rt);
      k += nlri->masked_bytes();
    }  
}

UpdateMessage::~UpdateMessage(){
  for(int i=0;i<wds.size();i++) {
    if (wds[i]) {
      delete wds[i];
    }  
  }
  for(int j=0;j<rtes.size();j++) {
   if (rtes[j]) {
     delete rtes[j];
   }    
  }   
}

  // ----- UpdateMessage::rte ----------------------------------------------- //
  /**
   * Returns one of the message's routes.
   *
   * @param ind  The index of the route to return.
   * @return one of the message's routes
   */
Route * UpdateMessage::rte(int ind) {
    if (rtes.size()== 0 || ind >= rtes.size()) {
      return NULL;
    }
    return rtes[ind];
}

  // ----- UpdateMessage::wd ----------------------------------------------- //
  /**
   * Returns one of the message's withdrawn route addresses.
   *
   * @param ind  The index of the withdrawn route address to return.
   * @return one of the message's withdrawn route addresses
   */
IPaddress * UpdateMessage::wd(int ind) {
    if (wds.size() == 0 || ind >= wds.size()) {
      return NULL;
    }
    return wds[ind];
}

  // ----- UpdateMessage::body_bytecount ------------------------------------ //
  /**
   * Returns the number of octets (bytes) in the message body.  It is the sum
   * of two octets for the infeasible routes length, plus a variable number of
   * octets for the withdrawn routes, plus two octets for the total path
   * attribute length, plus a variable number of octets for the path
   * attributes, plus a variable number of octets for the NLRI.
   *
   * @return the number of octets (bytes) in the message
   */
int UpdateMessage::body_bytecount(){
    int wd_octets = 0, pa_octets = 0, nlri_octets = 0;
    
    if (wds.size() != 0) {
      for (int i=0; i<wds.size(); i++) {
        // one octet specifies the length, and 0-4 for the prefix itself
        if (wds[i] != NULL)
          wd_octets += 1 + wds[i]->masked_bytes();
      }
    }

    if (rtes.size() != 0) {
      // All routes in the message must have the exact same path attributes, so
      // looking at the attributes of only the first one will suffice.
      int j=0;
      /* find the first valid route */
      while(rtes[j]==NULL && j<rtes.size()) {
        j++;
      }

      if (j!=rtes.size()) {
        pa_octets = rtes[j]->pas_bytecount();
      }

      for (int i=0; i<rtes.size(); i++) {
        // one octet specifies the length, and 0-4 for the prefix itself
        // IPaddress * nlri = rtes[i]->nlri;
        if ( rtes[i]->nlri != NULL)
          nlri_octets += 1 + rtes[i]->nlri->masked_bytes();
      }
    }
    return 4 + wd_octets + pa_octets + nlri_octets;        
}

  // ----- UpdateMessage::add_route ----------------------------------------- //
  /**
   * Adds a route to the message.
   *
   * @param rte  The route to add to the message.
   */
void UpdateMessage::add_route(Route * rte) {
      rtes.push_back(rte);
}

  // ----- UpdateMessage::add_wd -------------------------------------------- //
  /**
   * Adds the destination of a withdrawn route to this message.
   *
   * @param wd  The destination of the withdrawn route to add.
   */
void UpdateMessage::add_wd(IPaddress *wd) {
      wds.push_back(wd);
}

// ----- UpdateMessage::remove_wd ----------------------------------------- //
  /**
   * Remove withdrawn route information from the message.
   *
   * @param ipa  The IP address prefix to remove.
   * @return true only if the remove was successful
   */
bool UpdateMessage::remove_wd(IPaddress * ipa) {
    if (wds.size() == 0) {
      return false;
    }
    for (int i=0; i<wds.size(); i++) {
      if (ipa->equals(wds[i])) {
        wds[i] = NULL;
        return true;
      }
    }
    return false;
}

// ----- UpdateMessage::is_empty ------------------------------------------ //
  /**
   * Returns true only if there is information in the message.
   *
   * @return true only if there are no withdrawn routes nor any NLRI
   */
bool UpdateMessage::is_empty() {
    return (wds.size()==0 && rtes.size()==0);
}

 // ----- UpdateMessage::toString ------------------------------------------ //
  /**
   * Returns a string describing the contents of the update message.
   *
   * @return a string representation of the update message
   */
string UpdateMessage::toString() {
    string str = "wds: ";
    if ( wds.size() == 0) {
      str += "-  ads: ";
    } else {
      for (int i=0; i<wds.size(); i++) {
        if (wds[i]!=NULL)
          str += (wds[i])->toString() + " ";
      }
      str += " ads: ";
    }
    if ( rtes.size() == 0) {
      str += "-";
    } else {
      for (int i=0; i<rtes.size(); i++) {
        if (rtes[i]!=NULL)
          str += (rtes[i])->nlri->toString() + " ";
      }
      str += (rtes[0])->aspath()->toString() + " ";
    }
    return str;
}

  // ----- UpdateMessage::toStream ------------------------------------------------ //
  /**
   * Returns a char stream represenation of the message.
   *
   * @return a char stream representation of the message pointed by stream
   */
void UpdateMessage::toStream(char * stream){
    // fill in header
    BGPMessage::toStream(stream);
    
    // fill in Unfeasible routes length.
    int k=19 ;           
    stream[k++] = wds.size()/256; 
    stream[k++] = wds.size()%256;
    
    //  fill in Withdrawn routes (variable)
    for (int i=0;i<wds.size();i++) {
         stream[k++] = wds[i]->prefix_len();   
         wds[i]->masked_toStream(stream+k);
         // advance k with masked prefix of wds[i] in bytes  
         k += wds[i]->masked_bytes();  
    }
    
    // fill in Path attributes. Since all routes have the same attributes,
    //  looking in one route will be enough.      
    if (rtes.size()!=0) {
      int j=0;
      // find the first valid route 
      while(rtes[j]==NULL && j<rtes.size()) {
        j++;
      }
      if (j!=rtes.size()) {
        // thers's at least one route in rtes.
        // fill in Path attribute length and path attributes (variable) 
        int ok = rtes[j]->pas_bytecount()/256;
        stream[k++] = ok;
        ok = rtes[j]->pas_bytecount()%256;
        stream[k++] = ok ;
        rtes[j]->pasToStream(stream+k);

        k += rtes[j]->pas_bytecount();
        
        // fill in NLRI (variable)         
        for (int f=0;f<rtes.size();f++) {
          if (rtes[f]!=NULL) {
            rtes[f]->nlriToStream(stream+k);
            // advance k with the sum of masked nrli length in bytes
            //   and 1 (the length field).     
            k += rtes[f]->nlri->masked_bytes() + 1;
          }
        } 
      } // if (j!=...     
    }   //if (rtes....
}
