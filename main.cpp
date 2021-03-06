#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "bitextractor.hpp"

#include "mp4parser.hpp"

// TEST CLASSES
// ---------------------------------
class ATOM {
public:
   ATOM() {
      std::cout << "ATOM C" << std::endl;
   }
   ATOM(const ATOM &x) {
      std::cout << "ATOM CC" << std::endl;
   }
   ~ATOM() {
      std::cout << "ATOM D" << std::endl;
   }
};

class TEST {
public:
   TEST() {
      mV.reserve(32);
   }
   void add(ATOM &&x) {
      mV.emplace_back(std::move<ATOM&>(x));
   }
   std::vector<ATOM> mV;
};
// ------------------------------------

int main(int argc, char *argv[]) {

   // TESTS
   // -----------------------------------
   #if 0
   TEST t;
   
   t.add(ATOM());
   #endif

   std::string filename = "sample_mpeg4.mp4"; 

   // Read the file into memory
   // ------------------------------------------
   std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
   unsigned int filesize = in.tellg();

   std::cout << "File size of : " << filesize << " bytes." << std::endl;

   // return at the begining
   in.seekg(0, in.beg);

   // Allocate mem ory for file
   unsigned char *filedata = new unsigned char [filesize];

   // Read 
   in.read((char *)filedata, filesize);
   in.close();

   // Test bit extractor
   #if 0
   bit_extractor be(filedata, filesize);
   unsigned int code0 = be.get_bits(8);
   unsigned int code1 = be.get_bits(4);
   unsigned int code2 = be.get_bits(4);
   unsigned int code3 = be.get_bits(5);
   unsigned int code4 = be.get_bits(5);
   unsigned int code5 = be.get_bits(6);

   std::cout << bit_code{code0, 8} << std::endl;
   std::cout << bit_code{code1, 4} << std::endl;
   std::cout << bit_code{code2, 4} << std::endl;
   std::cout << bit_code{code3, 5} << std::endl;
   std::cout << bit_code{code4, 5} << std::endl;
   std::cout << bit_code{code5, 6} << std::endl;  
   #endif

   #if 0
   // Create the ISO MP4 box tree 
   mp4tree mp4t;
   mp4t.parseboxtree(filedata, filesize);

   // Instanciate an MP4 parser  
   mp4parser mp4p;
   #endif

   delete [] filedata;

   return 0;
}
