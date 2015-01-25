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
   // General names
   mp_token_name, 

   // Reserved words
   mp_token_while,
   mp_token_if,
   mp_token_break,

   mp_token_var,
   mp_token_struct,
   mp_token_proc,

   mp_token_bit,
   mp_token_list,
   mp_token_sizeof,
   mp_token_in,

   // Operators
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
   mp_token_dot,
   mp_token_equal,
   mp_token_notequal,
   mp_token_greater,
   mp_token_lesser,
   mp_token_greaterequal,
   mp_token_lesserequal,
   mp_token_rightarrow,
   mp_token_not,
   mp_token_or,
   mp_token_and,
   mp_token_xor,
   mp_token_bwnot,
   mp_token_bwor,
   mp_token_bwand,
   mp_token_bwxor,
   mp_token_leftshift,
   mp_token_rightshift,
   mp_token_assignment,
   
   // Numerals
   mp_token_numeral,

   // String Literals
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
      mTokenTypeNames[mp_token_name]          = "Name";

      // Reserved words
      mTokenTypeNames[mp_token_while]         = "While";  
      mTokenTypeNames[mp_token_break]         = "Break";  
      mTokenTypeNames[mp_token_if]            = "If";  
      mTokenTypeNames[mp_token_var]           = "Var";  
      mTokenTypeNames[mp_token_proc]          = "Proc"; 
      mTokenTypeNames[mp_token_struct]        = "Struct";       
      mTokenTypeNames[mp_token_in]            = "In"; 
      mTokenTypeNames[mp_token_sizeof]        = "Sizeof"; 

      mTokenTypeNames[mp_token_list]          = "List";  
      mTokenTypeNames[mp_token_bit]           = "Bit"; 

      // Operators
      mTokenTypeNames[mp_token_slash]         = "Slash";  
      mTokenTypeNames[mp_token_plus]          = "Plus";  
      mTokenTypeNames[mp_token_minus]         = "Minus"; 
      mTokenTypeNames[mp_token_star]          = "Star";  
      mTokenTypeNames[mp_token_leftpar]       = "Left Parenthese";  
      mTokenTypeNames[mp_token_rightpar]      = "Right Parenthese";  
      mTokenTypeNames[mp_token_leftbrace]     = "Left Brace";  
      mTokenTypeNames[mp_token_rightbrace]    = "Right Brace";  
      mTokenTypeNames[mp_token_leftbracket]   = "Left Bracket";  
      mTokenTypeNames[mp_token_rightbracket]  = "Right Bracket";  
      mTokenTypeNames[mp_token_semicolon]     = "Semicolon";  
      mTokenTypeNames[mp_token_dot]           = "Dot";  
      mTokenTypeNames[mp_token_equal]         = "Equal";  
      mTokenTypeNames[mp_token_notequal]      = "Not Equal";  
      mTokenTypeNames[mp_token_greater]       = "Greater";  
      mTokenTypeNames[mp_token_lesser]        = "Lesser";  
      mTokenTypeNames[mp_token_greaterequal]  = "Greater or Equal";  
      mTokenTypeNames[mp_token_lesserequal]   = "Lesser or Equal";
      mTokenTypeNames[mp_token_rightarrow]    = "Right Arrow";  
      mTokenTypeNames[mp_token_not]           = "Not";  
      mTokenTypeNames[mp_token_or]            = "Or";  
      mTokenTypeNames[mp_token_and]           = "And";  
      mTokenTypeNames[mp_token_xor]           = "Xor";  
      mTokenTypeNames[mp_token_bwnot]         = "Bitwise Not";  
      mTokenTypeNames[mp_token_bwor]          = "Bitwise Or";  
      mTokenTypeNames[mp_token_bwand]         = "Bitwise And";  
      mTokenTypeNames[mp_token_bwxor]         = "Bitwise Xor";  
      mTokenTypeNames[mp_token_leftshift]     = "Left Shift";  
      mTokenTypeNames[mp_token_rightshift]    = "Right Shift";  
      mTokenTypeNames[mp_token_assignment]    = "Assignment";

      // Constant - Numerals
      mTokenTypeNames[mp_token_numeral]       = "Numeral";  

      // String literals
      mTokenTypeNames[mp_token_literal]       = "String Literal";  

      // Reserved words map
      mReservedWords["while"]  = mp_token_while;
      mReservedWords["break"]  = mp_token_break;
      mReservedWords["if"]     = mp_token_if;     
      mReservedWords["var"]    = mp_token_var;
      mReservedWords["proc"]   = mp_token_proc;
      mReservedWords["struct"] = mp_token_struct;
      mReservedWords["in"]     = mp_token_in;
      mReservedWords["sizeof"] = mp_token_sizeof;
      mReservedWords["bit"]    = mp_token_bit;
      mReservedWords["list"]   = mp_token_list;

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

   bool mp_readdecnumber(char **pc, mp_token *token) {
      bool rval = true;

      // Token name is set to name - will be changed to reserved if part of the 
      // reserved words
      token->mType = mp_token_numeral;

      unsigned int current_char = 0;

      while ((mp_isdecdigit(**pc) || ('.' == **pc)) && (current_char < MAX_TOKEN_SIZE)) {
         token->mValue[current_char] = **pc;
         ++current_char;
         ++(*pc);
      }

      return rval;
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
         ++current_char;
         ++(*pc);
      }
      token->mValue[current_char] = 0x00;

      // Try to find a reserved word
      auto it = mReservedWords.find(token->mValue);
      if (it != mReservedWords.end()) {
         token->mType = it->second;
      }
      
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
      
      switch (**pc) {
      case '/':
         token->mType = mp_token_slash;
         ++(*pc);
         break;
 
      case '-':
         token->mType = mp_token_minus;
         ++(*pc);         
         if ('>' == **pc) {
            token->mType = mp_token_rightarrow;
            ++(*pc);
         }
         break;
      
      case '+':
         token->mType = mp_token_plus;
         ++(*pc);
         break;

      case '*':
         token->mType = mp_token_star;
         ++(*pc);         
         break;

      case '(':
         token->mType = mp_token_leftpar;
         ++(*pc);
         break;

      case ')':
         token->mType = mp_token_rightpar;
         ++(*pc);
         break;

      case '{':
         token->mType = mp_token_leftbrace;
         ++(*pc);
         break;

      case '}':
         token->mType = mp_token_rightbrace;
         ++(*pc);
         break;

      case '[':
         token->mType = mp_token_leftbracket;
         ++(*pc);
         break;

      case ']':
         token->mType = mp_token_rightbracket;
         ++(*pc);
         break;

      case ';':
         token->mType = mp_token_semicolon;
         ++(*pc);
         break;

      case '.':
         token->mType = mp_token_dot;
         ++(*pc);
         break;

      case '~':
         token->mType = mp_token_bwnot;
         ++(*pc);
         break;

      case '=':
         token->mType = mp_token_assignment;
         ++(*pc);
         if ('=' == **pc) {
            token->mType = mp_token_equal;
            ++(*pc);
         }
         break;

      case '!':
         token->mType = mp_token_not;
         ++(*pc);
         if ('=' == **pc) {
            token->mType = mp_token_notequal;
            ++(*pc);
         }
         break;

      case '^':
         token->mType = mp_token_bwxor;
         ++(*pc);
         if ('^' == **pc) {
            token->mType = mp_token_xor;
            ++(*pc);
         }
         break;

      case '&':
         token->mType = mp_token_bwand;
         ++(*pc);
         if ('&' == **pc) {
            token->mType = mp_token_and;
            ++(*pc);
         }
         break;

      case '|':
         token->mType = mp_token_bwor;
         ++(*pc);
         if ('|' == **pc) {
            token->mType = mp_token_or;
            ++(*pc);
         }
         break;

      case '>':
         token->mType = mp_token_greater;
         ++(*pc);
         if ('=' == **pc) {
            token->mType = mp_token_greaterequal;
            ++(*pc);
         }
         break;

      case '<':
         token->mType = mp_token_lesser;
         ++(*pc);
         if ('=' == **pc) {
            token->mType = mp_token_lesserequal;
            ++(*pc);
         }
         break;
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
         
         ++current_char;
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
            // If an operator
            else if ((*pc == '*') || (*pc == '+') || (*pc == '-') || 
                     (*pc == '(') || (*pc == ')') || (*pc == '{') || 
                     (*pc == '}') || (*pc == '[') || (*pc == ']') ||
                     (*pc == ';') || (*pc == '.') || (*pc == '<') ||
                     (*pc == '>') || (*pc == '=') || (*pc == '!') ||
                     (*pc == '^') || (*pc == '&') || (*pc == '|')) {
               if (mp_readoperator(&pc, current_token)) {
                  mTokenList.push_back(std::shared_ptr<mp_token>(current_token));
                  need_new_token = true;
               }
            }
            // If a string literal
            else if (*pc == '"') {
               if (mp_readliteral(&pc, current_token, pe-pc)) {
                  mTokenList.push_back(std::shared_ptr<mp_token>(current_token));
                  need_new_token = true;
               }
               else {
                  err();
               }
            }
            // If a number
            else if (mp_isdecdigit(*pc)) {
               if (('0' == *pc) && (('x' == *(pc+1)) || ('X' == *(pc+1)))) {
                  // TODO HEX
               } 
               else {
                  if (mp_readdecnumber(&pc, current_token)) {
                     mTokenList.push_back(std::shared_ptr<mp_token>(current_token));
                     need_new_token = true;
                  }
                  else {
                     err();
                  }
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
