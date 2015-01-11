#ifndef MP4_PARSER_HPP___
#define MP4_PARSER_HPP___

#include <string>
#include <list>
#include <map>
#include <algorithm>
#include <memory>
#include <iostream>

#include "byteextractor.hpp"
#include "bit_structure.hpp"

class mp4atom_type {
public:
   explicit mp4atom_type() : 
      mName("xxxx"),
      mIsFull(false),
      mIsRoot(false) {}

   mp4atom_type(std::string s, bool isf, bool isr) :
      mName(s),
      mIsFull(isf),
      mIsRoot(isr) {}


   std::string mName;
   bool        mIsFull;
   bool        mIsRoot;
};

class mp4atom {
public:
   explicit mp4atom() : 
      mName("xxxx"), 
      mPayLoad((unsigned char *)0),
      mSize(0) {}

   mp4atom(std::string n, unsigned char *p, unsigned int s) : 
      mName(n), 
      mPayLoad(p),
      mSize(s) {}

   ~mp4atom() {
	 std::cout << mName << " of size " << mSize << " bytes" << std::endl;      
   }

private:
   std::string    mName;
   unsigned char *mPayLoad;
   unsigned int   mSize;
};

class mp4tree {
public:
   mp4tree() {
      // Root boxes
      //                            Name    isFull isRoot
      mTypes["moov"] = mp4atom_type("moov", false, true);
      mTypes["trak"] = mp4atom_type("trak", false, true);
      mTypes["edts"] = mp4atom_type("edts", false, true);
      mTypes["mdia"] = mp4atom_type("mdia", false, true);
      mTypes["minf"] = mp4atom_type("minf", false, true);
      mTypes["dinf"] = mp4atom_type("dinf", false, true);
      mTypes["stbl"] = mp4atom_type("stbl", false, true);
      mTypes["vmex"] = mp4atom_type("vmex", false, true);
      mTypes["moof"] = mp4atom_type("moof", false, true);
      mTypes["traf"] = mp4atom_type("traf", false, true);
      mTypes["mfra"] = mp4atom_type("mfra", false, true);
      mTypes["skip"] = mp4atom_type("skip", false, true);
      mTypes["udta"] = mp4atom_type("udta", false, true);
      mTypes["strk"] = mp4atom_type("strk", false, true);
      mTypes["meta"] = mp4atom_type("meta", false, true);
      mTypes["dinf"] = mp4atom_type("dinf", false, true);
      mTypes["ipro"] = mp4atom_type("ipro", false, true);
      mTypes["sinf"] = mp4atom_type("sinf", false, true);
      mTypes["flin"] = mp4atom_type("flin", false, true);
      mTypes["paen"] = mp4atom_type("paen", false, true);

      // Leaf boxes
      //                            Name    isFull isRoot
      mTypes["mdhd"] = mp4atom_type("mdhd", true, false);      
   }
   ~mp4tree() {}

   bool is_root(std::string s) {
      bool rval = false;
      
      // Look for the box name - return the root type
      auto it = mTypes.find(s);
      if (it != mTypes.end()) {
	 rval = (it->second).mIsRoot;
      }

      return rval;
   }

   bool parseboxtree(unsigned char *payload, unsigned int size) {      
      bool rval = true;
      
      byte_extractor be(payload);

      while (size > 0) {
         // Read the total size of the box - including size+name+payload
	 unsigned int s = be.get_4bytes();

         // Read the name
	 std::string t;
         t.resize(4);
         t[0] = be.get_byte();
         t[1] = be.get_byte();
         t[2] = be.get_byte();
         t[3] = be.get_byte();

         // Push the box in the atom list
         mAtoms.emplace_back(std::shared_ptr<mp4atom>(new mp4atom(t, 
                                                                  be.get_payload(), 
                                                                  s)));
         
         // If the box can be decomposed recursively call parsebox(..)
	 if (is_root(t)) {
	    parseboxtree(be.get_payload(), s - 8);
	 }

         // Size is reduced by s
	 size -= s;
         // Skipping only s-8 because we have already read 4 bytes for the name
         // and 4 bytes for the size
	 be.skip(s - 8);
      }

      return rval;
   }

private:
   std::map<std::string, mp4atom_type> mTypes;
   std::list<std::shared_ptr<mp4atom>> mAtoms;
};

class mp4parser {
public:
   mp4parser() {
      bit_structure *bs = new bit_structure(); 

      bs->add_node(bit_structure_node{bsn_comp, "version",              1, 1, 9}); //0
      bs->add_node(bit_structure_node{bsn_pop,  "creation_time_l",     32, 0, 0}); //1 
      bs->add_node(bit_structure_node{bsn_pop,  "creation_time_h",     32, 0, 0}); //2
      bs->add_node(bit_structure_node{bsn_pop,  "moficication_time_l", 32, 0, 0}); //3
      bs->add_node(bit_structure_node{bsn_pop,  "moficication_time_h", 32, 0, 0}); //4
      bs->add_node(bit_structure_node{bsn_pop,  "time_scale",          32, 0, 0}); //5
      bs->add_node(bit_structure_node{bsn_pop,  "duration_l",          32, 0, 0}); //6
      bs->add_node(bit_structure_node{bsn_pop,  "duration_h",          32, 0, 0}); //7
      bs->add_node(bit_structure_node{bsn_jump, "",                    13, 0, 0}); //8
      bs->add_node(bit_structure_node{bsn_pop,  "creation_time",       32, 0, 0}); //9
      bs->add_node(bit_structure_node{bsn_pop,  "moficication_time",   32, 0, 0}); //10
      bs->add_node(bit_structure_node{bsn_pop,  "time_scale",          32, 0, 0}); //11
      bs->add_node(bit_structure_node{bsn_pop,  "duration",            32, 0, 0}); //12
      bs->add_node(bit_structure_node{bsn_pop,  "pad_language",        16, 0, 0}); //13
      bs->add_node(bit_structure_node{bsn_pop,  "predefined",          16, 0, 0}); //14
      bs->add_node(bit_structure_node{bsn_end,  "",                     0, 0, 0}); //15
      
      mParsingAlgos["mdhd"] = std::shared_ptr<bit_structure>(bs);
   }
   ~mp4parser() {
      
   }

   bool parse_box(mp4tree &tree, std::string box_name) {
      bool rval = 0;

      // Find a bit structure

      // Find the box in the tree

      // Apply parser

      return 0;
   }

   
private:
   std::map<std::string, std::shared_ptr<bit_structure>> mParsingAlgos;

   bit_structure_parser mBSParser;
};


#endif
