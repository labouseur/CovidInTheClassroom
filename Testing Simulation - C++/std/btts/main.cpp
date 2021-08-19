#include "BinaryTreeTestingSimulation.h"
#include "main.h"
#include "utils.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
   int retCode = 0;

   welcomeMessage();

   // Note: argv[0] is the executable name. The parameters begin with index 1.
   if (argc >= 5) {
      std::cout << "Parsing arguments: ";
      for (int i = 1; i < argc; i++) {
         std::cout << argv[i] << " ";
      } // end for i
      std::cout << std::endl;
      parseAndRun(argv);
   } else {
      std::cout << "Usage: " << argv[0] << " [- option] <int population size> <int group size> <decimal infection rate> <decimal test accuracy rate>" << std::endl;
      retCode = 1;
   } // end if

   return retCode;
}

void welcomeMessage()
{
   // http://patorjk.com/software/taag - font: Big
   std::cout << "  ____ _______ _______ _____ "   << std::endl;
   std::cout << " |  _ \\__   __|__   __/ ____|"  << std::endl;
   std::cout << " | |_) | | |     | | | (___  "   << std::endl;
   std::cout << " |  _ <  | |     | |  \\___ \\ " << std::endl;
   std::cout << " | |_) | | |     | |  ____) |"   << std::endl;
   std::cout << " |____/  |_|     |_| |_____/ "   << std::endl;
   std::cout << "Binary Tree Testing Simulation"  << std::endl;
   std::cout << "version 0.5"                     << std::endl;
   std::cout << std::endl;

}

int parseAndRun(char* argv[])
{
   int retVal = 0;
   bool quietMode = false;
   std::string arg = "";
   int argPos = 1;

   // The first argument is optional.
   arg = argv[argPos];
   arg = allTrim(arg);
   if (arg.substr(0,1) == "-") {
      // We've found an optional switch.
      // Check for quiet mode.
      if ( (arg.substr(1,1) == "q") ||
           (arg.substr(1,1) == "Q") ) {
         quietMode = true;
      }
      argPos++;
   }

   // Required argument 1: int population size
   arg = argv[argPos];
   int populationSize = 0;
   try {
      populationSize = std::stoi(arg ,nullptr,10);
   } catch (const std::exception& e) {
      std::cout << "Cannot convert [" << arg << "] to type int."  << " (" <<  e.what() << ")" << std::endl;
      retVal = 1;
   }
   argPos++;

   // Required argument 2: int group size
   arg = argv[argPos];
   int groupSize = 0;
   try {
      groupSize = std::stoi(arg ,nullptr,10);
   } catch (const std::exception& e) {
      std::cout << "Cannot convert [" << arg << "] to type int."  << " (" <<  e.what() << ")" << std::endl;
      retVal = 1;
   }
   argPos++;

   // Required argument 3: decimal infection rate
   arg = argv[argPos];
   double infectionRate = 0.0;
   try {
      infectionRate = std::stod(arg ,nullptr);
   } catch (const std::exception& e) {
      std::cout << "Cannot convert [" << arg << "] to type double."  << " (" <<  e.what() << ")" << std::endl;
      retVal = 1;
   }
   argPos++;

   // Required argument 4: decimal test accuracy rate
   arg = argv[argPos];
   double testAccuracyRate =0.0;
   try {
      testAccuracyRate = std::stod(arg ,nullptr);
   } catch (const std::exception& e) {
      std::cout << "Cannot convert [" << arg << "] to type double." << " (" <<  e.what() << ")" << std::endl;
      retVal = 1;
   }
   argPos++;      // This is not strictly necessary. But if I add more arguments in the future, it will be welcome.

   if (retVal == 0) {
      // All passed-in arguments are ok. Let's simulate!
      auto *btts = new BinaryTreeTestingSimulation(populationSize, groupSize, infectionRate, testAccuracyRate);
      btts->runTest(quietMode);
      delete btts; // Clean up the memory allocated to the btts object.
   } // end if

   return retVal;
}
