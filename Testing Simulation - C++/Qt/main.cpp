#include <binarytreetestingsimulation.h>
#include <main.h>

#include <QCoreApplication>
#include <QFile>

int main(int argc, char* argv[])
{
   int retCode = 0;
   QCoreApplication coreApplication(argc, argv);

   welcomeMessage();

   // Note: argv[0] is the executable name. The parameters begin with index 1.
   if (argc >= 5) {
      out << "Parsing arguments: ";
      for (int i = 1; i < argc; i++) {
         out << argv[i] << " ";
      } // end for i
      out << endl;
      retCode = parseAndRun(argv);
   } else {
      out << "Usage: " << argv[0] << " [- option] <int population size> <int group size> <decimal infection rate> <decimal test accuracy rate>" << endl;
      retCode = 1;
   } // end if

   return retCode;
}

void welcomeMessage()
{
   // http://patorjk.com/software/taag - font: Big
   out << "  ____ _______ _______ _____ "   << endl;
   out << " |  _ \\__   __|__   __/ ____|"  << endl;
   out << " | |_) | | |     | | | (___  "   << endl;
   out << " |  _ <  | |     | |  \\___ \\ " << endl;
   out << " | |_) | | |     | |  ____) |"   << endl;
   out << " |____/  |_|     |_| |_____/ "   << endl;
   out << "Binary Tree Testing Simulation"  << endl;
   out << "version 0.4"                     << endl;
   out << endl;

}

/*
   From Eitel:
   P(sick|test_pos) = P(test_pos|sick) *P(sick)  / ( P(test_pos|sick) * P(sick) + P(test_pos|not_sick) * P(not_sick) )
   P(sick)  - the prevalence-  is also f(t), depending on the infection rate.
   If you fix it (a snapshot), at let's say 1%, and the  test is 98% reliable,
   you have roughly 33% chance of being sick if you tested positive (in this case
   of 1 or more being sick)
*/


int parseAndRun(char* argv[])
{
   int retVal = 0;
   bool ok = false;
   bool quietMode = false;
   QString arg = "";
   int argPos = 1;

   // The first argument is optional.
   arg = argv[argPos];
   arg = arg.trimmed();
   if (arg.startsWith("-")) {
      // We've found an optional switch.
      // Check for quiet mode.
      if (arg.toUpper() == "-Q") {
         quietMode = true;
      }
      argPos++;
   }

   // Required argument 1: int population size
   arg = argv[argPos];
   int populationSize = arg.toInt(&ok);
   if (! ok) {
      out << "Cannot convert [" << arg << "] to type int." << endl;
      retVal = 1;
   }
   argPos++;

   // Required argument 2: int group size
   arg = argv[argPos];
   int groupSize = arg.toInt(&ok);
   if (!ok) {
      out << "Cannot convert [" << arg << "] to type int." << endl;
      retVal = 1;
   }
   argPos++;

   // Required argument 3: decimal infection rate
   arg = argv[argPos];
   double infectionRate = arg.toDouble(&ok);
   if (!ok) {
      out << "Cannot convert [" << arg << "] to type float." << endl;
      retVal = 1;
   }
   argPos++;

   // Required argument 4: decimal test accuracy rate
   arg = argv[argPos];
   double testAccuracyRate = arg.toDouble(&ok);
   if (!ok) {
      out << "Cannot convert [" << arg << "] to type float." << endl;
      retVal = 1;
   }
   argPos++;      // This is not strictly necessary. But if I add more arguments in the future, it will be welcome.

   if (retVal == 0) {
      // All passed-in arguments are ok. Let's simulate!
      // QFile outFile("btts_output.csv");
      // if (outFile.open(QFile::Append | QFile::Text)) {
         auto *btts = new BinaryTreeTestingSimulation(populationSize, groupSize, infectionRate, testAccuracyRate);
         btts->runTest(quietMode); // , outFile);
         delete btts; // Clean up the memory allocated to the btts object.
         // outFile.flush();
         // outFile.close();
      // } else {
      //    out << "Error: Cannot open output file." << endl;
      // } // end if
   } // end if

   return retVal;
}
