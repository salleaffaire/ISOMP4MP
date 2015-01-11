#ifndef BYTE_EXTACTOR_HPP___
#define BYTE_EXTACTOR_HPP___

#include <string>

class byte_extractor {
public:
   byte_extractor(unsigned char *payload) : mPayload(payload) {}

   unsigned char get_byte() {
      return *mPayload++;
   }

   unsigned int get_4bytes() {
      unsigned int rval;

      rval = mPayload[3]     | 
	 (mPayload[2] << 8)  |
	 (mPayload[1] << 16) |
	 (mPayload[0] << 24);

      mPayload += 4;
      return rval;
   }

   unsigned long long get_nbytes(unsigned int n) {
      unsigned long long rval = 0;

      while (n != 0) {
         rval <<= 8;
         rval |= mPayload[0];
         --n;
         ++mPayload;
      }

      return rval;
   }

   void skip(unsigned int nb) {
      mPayload += nb;
   }

   unsigned char *get_payload() {
      return mPayload;
   }

private:
   unsigned char *mPayload;
};

#endif
