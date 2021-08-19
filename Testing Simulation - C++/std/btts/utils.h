//
// Utils.h
//

#include <string>

std::string allTrim(std::string str) {
   /*
      From https://www.thecrazyprogrammer.com/2021/01/c-string-trim.html
      Named allTrim() because I have fond memories of Clipper.
   */
   const char* typeOfWhitespaces = " \t\n\r\f\v";
   str.erase(str.find_last_not_of(typeOfWhitespaces) + 1);
   str.erase(0,str.find_first_not_of(typeOfWhitespaces));
   return str;
}
