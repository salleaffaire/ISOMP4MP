#ifndef BIT_STRUCTURE_HPP___
#define BIT_STRUCTURE_HPP___

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>

#include "bitextractor.hpp"

enum BSN_TYPE {
   bsn_pop  = 0x10,
   bsn_jump = 0x20,
   bsn_comp = 0x30,
   bsn_end  = 0xF0
};

class bit_structure_node {
public:

   ~bit_structure_node() {
      //std::cout << " BSN D" << std::endl;
   }

   BSN_TYPE     mType;   
   std::string  mName;
   unsigned int mValues[3];
};

class bit_structure {
public:
   bit_structure() {
      mBSNVector.reserve(32);
      //std::cout << " BS C" << std::endl;
   }
   ~bit_structure() {
      //std::cout << " BS D" << std::endl;
   }
   bit_structure(const bit_structure &bsn) {
      //std::cout << " BS CC" << std::endl;
   }
   bit_structure &operator=(const bit_structure &bsn) {
      //std::cout << " BS CC" << std::endl;
      return *this;
   }

   void add_node(bit_structure_node &&bsn) {
      mBSNVector.emplace_back(bsn);
   }

   bit_structure_node &operator[](unsigned int index) {
      return mBSNVector[index];
   }

   
private:
   std::vector<bit_structure_node> mBSNVector;
};

class bit_structure_parser {
public:
   bit_structure_parser() {}
   ~bit_structure_parser() {}

   bool parse(bit_structure &bs, unsigned char *data, unsigned int size) {
      // Empty the map
      mVars.clear();

      // Create a byte extractor
      bit_extractor be(data, size);

      // Run the parser following the bit_structure
      unsigned int current_index = 0;
      BSN_TYPE bt;
      unsigned long long value;
      while ((bt == bs[current_index].mType) != bsn_end) {
         switch (bt) {
         case bsn_pop:
            value = be.get_bits(bs[current_index].mValues[0]);
            mVars[bs[current_index].mName] = 
               std::pair<unsigned long long, unsigned long long>(value, value);
            ++current_index;
            break;
         case bsn_jump:
            current_index = bs[current_index].mValues[0];
            break;
         case bsn_comp:
            value = mVars[bs[current_index].mName].second;
            if (value == bs[current_index].mValues[0]) {
               current_index = bs[current_index].mValues[1];
            } 
            else {
               current_index = bs[current_index].mValues[2];
            }
            break;
         }
      }
   }

   bool output() {
      for (auto &element: mVars) {
         std::cout << std::setw(30) << std::setfill(' ') << element.first << " "
                   << element.second.first << std::endl;
      }
   }

private:
   std::map<std::string, std::pair<unsigned long long, unsigned long long>> mVars;
   
};

#endif
