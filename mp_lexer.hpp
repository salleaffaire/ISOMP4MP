#ifndef MPLEXER_HPP___
#define MPLEXER_HPP___

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <memory>

#define MAX_TOKEN_SIZE 256

#define MIN(X,Y) ((X < Y) ? (X) : (Y))

enum MP_LEXER_STATE {
   mp_l_ok     = 0x0000,
   mp_l_uninit = 0x0001,
   mp_l_error  = 0xF000
};

enum MP_LEXER_ERROR {
   mp_l_error_unknown = 0x0000,
   mp_l_error_file    = 0x0001, // Input file can't be opened
   mp_l_error_syntax  = 0x0002
};

enum MP_TOKEN_TYPE {
   mp_token_name, 

   mp_token_while,
   mp_token_if,
   mp_token_break,

   mp_token_var,
   mp_token_proc,

   mp_token_slash,
   mp_token_plus,
   mp_token_minus,
   mp_token_star,
   mp_token_leftpar,
   mp_token_rightpar,
   mp_token_leftbrace,
   mp_token_rightbrace,
   mp_token_leftbracket,
   mp_token_rightbracket,
   mp_token_semicolon,

   mp_token_literal
};

struct mp_token {
   MP_TOKEN_TYPE   mType;
   char            mValue[MAX_TOKEN_SIZE];
};


class mp_lexer {
public:
   explicit mp_lexer() :
      mPayload((char *)0), mPayloadSize(0),
      mState(mp_l_uninit), mError(mp_l_error_unknown) {
      init();
   }

   explicit mp_lexer(std::string filename) :
      mPayload((char *)0), mPayloadSize(0),
      mState(mp_l_uninit), mError(mp_l_error_unknown) { 

      // Read the file into memory
      // ------------------------------------------
      std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
      if (in.fail()) {
         mState = mp_l_error;
         mError = mp_l_error_file;
      }
      else {
         mState = mp_l_ok;
         
         mPayloadSize = in.tellg();

         // std::cout << "File size of : " << filesize << " bytes." << std::endl;

         // return at the begining
         in.seekg(0, in.beg);

         // Allocate mem ory for file
         mPayload = new char [mPayloadSize];

         // Read 
         in.read((char *)mPayload, mPayloadSize);
         in.close();
      }

      init();
   }

   ~mp_lexer() {
      if (mPayload) delete []mPayload;
   }

   bool init() {
      // Token names map
      mTokenTypeNames[mp_token_name]    = "Name";

      mTokenTypeNames[mp_token_while]   = "While";  
      mTokenTypeNames[mp_token_break]   = "Break";  
      mTokenTypeNames[mp_token_if]      = "If";  
      mTokenTypeNames[mp_token_var]     = "Var";  
      mTokenTypeNames[mp_token_proc]    = "Proc";  

      mTokenTypeNames[mp_token_slash]   = "Slash";  
      mTokenTypeNames[mp_token_plus]    = "Plus";  
      mTokenTypeNames[mp_token_minus]   = "Minus"; 
      mTokenTypeNames[mp_token_star]    = "Star";  

      mTokenTypeNames[mp_token_leftpar]       = "Left Parenthese";  
      mTokenTypeNames[mp_token_rightpar]      = "Right Parenthese";  
      mTokenTypeNames[mp_token_leftbrace]     = "Left Brace";  
      mTokenTypeNames[mp_token_rightbrace]    = "Right Brace";  
      mTokenTypeNames[mp_token_leftbracket]   = "Left Bracket";  
      mTokenTypeNames[mp_token_rightbracket]  = "Right Bracket";  
      mTokenTypeNames[mp_token_semicolon]     = "Semicolon";  

      mTokenTypeNames[mp_token_literal]  = "String Literal";  

      // Reserved words map
      mReservedWords["while"] = mp_token_while;
      mReservedWords["break"] = mp_token_break;
      mReservedWords["if"]    = mp_token_if;     
      mReservedWords["var"]   = mp_token_var;
      mReservedWords["proc"]  = mp_token_proc;

      // Error message map
      mErrorMessages[mp_l_error_syntax] = "Syntax";
   }

   bool mp_isalpha(const char c) {
      return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         (c == '_');
   }

   bool mp_isdecdigit(const char c) {
      return (c >= '0' && c <= '9');
   }

   bool mp_ishexdigit(const char c) {
      return mp_isdecdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
   }

   bool mp_readalpha(char **pc, mp_token *token) {
      bool rval = true;

      // Token name is set to name - will be changed to reserved if part of the 
      // reserved words
      token->mType = mp_token_name;
      
      // Store first caracter
      unsigned int current_char = 1;
      token->mValue[0] = **pc;
      ++(*pc);

      while ((mp_isalpha(**pc) || mp_isdecdigit(**pc)) && (current_char < MAX_TOKEN_SIZE)) {
         token->mValue[current_char] = **pc;
         current_char++;
         ++(*pc);
      }
      token->mValue[current_char] = 0x00;
      
      return rval;
   }

   bool mp_readcomment_line(char **pc) {
      bool rval = true;
      
      // Skip the double slash '//'
      ++(*pc);
      ++(*pc);

      while ('\n' != **pc) {
         ++(*pc);
      }
      
      return rval;
   }

   bool mp_readoperator(char **pc, mp_token *token) {
      bool rval = true;      
      
      if ('/' == **pc) {
         token->mType = mp_token_slash;
         ++(*pc);
      } 
      else if ('-' == **pc) {
         token->mType = mp_token_minus;
         ++(*pc);         
      }
      else if ('+' == **pc) {
         token->mType = mp_token_plus;
         ++(*pc);         
      }
      else if ('*' == **pc) {
         token->mType = mp_token_star;
         ++(*pc);         
      }
      else if ('(' == **pc) {
         token->mType = mp_token_leftpar;
         ++(*pc);
      }
      else if (')' == **pc) {
         token->mType = mp_token_rightpar;
         ++(*pc);
      }
      else if ('{' == **pc) {
         token->mType = mp_token_leftbrace;
         ++(*pc);
      }
      else if ('}' == **pc) {
         token->mType = mp_token_rightbrace;
         ++(*pc);
      }
      else if ('[' == **pc) {
         token->mType = mp_token_leftbracket;
         ++(*pc);
      }
      else if (']' == **pc) {
         token->mType = mp_token_rightbracket;
         ++(*pc);
      }
      else if (';' == **pc) {
         token->mType = mp_token_semicolon;
         ++(*pc);
      }

      return rval; 
   }

   bool mp_readliteral(char **pc, mp_token *token, unsigned int charsleft) {
      bool rval = true;
      
      // Token name is set to name - will be changed to reserved if part of the 
      // reserved words
      token->mType = mp_token_literal;
      
      // Skip first caracter " 
      unsigned int current_char = 0;
      ++(*pc);

      const unsigned int limit = MIN(MAX_TOKEN_SIZE, charsleft);
      //std::cout << "Chars Left = " << charsleft << std::endl;
      //std::cout << "Chars Limit = " << limit << std::endl;
     
      while (('"' != **pc) && (current_char < limit)) {
         // If escaped 
         if ('\\' == **pc) {
            ++(*pc);
            if ('t' == **pc) {
               token->mValue[current_char] = '\t';   // Tab            
            }
            else if ('n' == **pc) {
               token->mValue[current_char] = '\n';   // New line
            }
            else if ('r' == **pc) {
               token->mValue[current_char] = '\r';   // Carriage Return
            }
            else if ('"' == **pc) {
               token->mValue[current_char] = '"';    // Double quote
            }
            else if ('\\' == **pc) {
               token->mValue[current_char] = '\\';   // Backslash
            }
         }
         else {
            token->mValue[current_char] = **pc;
         }
         
         current_char++;
         ++(*pc);
      }

      if ('"' != **pc) {
         mState = mp_l_error;
         mError = mp_l_error_syntax;
         rval = false;
      }
      else {
         // Skip last caracter "
         ++(*pc);
      }


      return rval;       
   }

   bool run() {
      if (mp_l_ok == mState) {

         char *pc = mPayload;
         const char *pe = mPayload + mPayloadSize;

         bool need_new_token = true;
         mp_token *current_token;

         while (pc < pe) {
            std::cout << *pc;
            if (need_new_token) {
               current_token = new mp_token;
               current_token->mValue[0] = 0x00;
               need_new_token = false;
            }

            // If a name
            if (mp_isalpha(*pc)) {
               if (mp_readalpha(&pc, current_token)) {
                  mTokenList.push_back(std::shared_ptr<mp_token>(current_token));
                  need_new_token = true;
               }
            }
            // If a comment
            else if (*pc == '/') {
               // Peek ahead -  
               if (*(pc+1) == '/') {
                  mp_readcomment_line(&pc);
               }
               else {
                  if (mp_readoperator(&pc, current_token)) {
                     mTokenList.push_back(std::shared_ptr<mp_token>(current_token));
                     need_new_token = true;
                  }       
               }
            }
            else if ((*pc == '*') || (*pc == '+') || (*pc == '-') || 
                     (*pc == '(') || (*pc == ')') || (*pc == '{') || 
                     (*pc == '}') || (*pc == '[') || (*pc == ']') ||
                     (*pc == ';')) {
               if (mp_readoperator(&pc, current_token)) {
                  mTokenList.push_back(std::shared_ptr<mp_token>(current_token));
                  need_new_token = true;
               }
            }
            else if (*pc == '"') {
               if (mp_readliteral(&pc, current_token, pe-pc)) {
                  mTokenList.push_back(std::shared_ptr<mp_token>(current_token));
                  need_new_token = true;
               }
               else {
                  err();
               }
            }
            // It's something else
            else {
               // Advance the pointer  
               ++pc;
            }
         }
      }
      else {
         err();
      }
   }
   
   void output_token_list() {
      std::cout << std::endl;
      std::cout << "TOKEN LIST" << std::endl;
      std::cout << "-------------------------------------------" << std::endl;
      for (auto &a : mTokenList) {
         std::cout << mTokenTypeNames[a->mType] << " : " << a->mValue << std::endl; 
      }
   }
   
private:
   char        *mPayload;
   unsigned int mPayloadSize;

   MP_LEXER_STATE mState;
   MP_LEXER_ERROR mError;

   std::list<std::shared_ptr<mp_token>> mTokenList;
   std::map<std::string, MP_TOKEN_TYPE> mReservedWords;
   std::map<MP_LEXER_ERROR, std::string> mErrorMessages;

   // This is mostly used for debugging
   std::map<MP_TOKEN_TYPE, std::string> mTokenTypeNames;

   bool err() {
      std::cout << std::endl;
      std::cout << "Error " << mError << std::endl;
   }
};

#endif
