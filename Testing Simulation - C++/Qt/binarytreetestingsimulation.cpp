#include "binarytreetestingsimulation.h"

#include <QDebug>
#include <QRandomGenerator>
#include <QTextStream>
#include <QtMath>

// ------------
// -- Public --
// ------------

// Constructor
BinaryTreeTestingSimulation::BinaryTreeTestingSimulation(int    _populationSize,
                                                         int    _groupSize,
                                                         double _infectionRate,
                                                         double _testAccuracyRate)
{
   // Initialize class private vars from passed-in values.
   populationSize    = _populationSize;
   groupSize         = _groupSize;
   infectionRate     = _infectionRate;
   testAccuracyRate  = _testAccuracyRate;
}

// Destructor
BinaryTreeTestingSimulation::~BinaryTreeTestingSimulation()
= default;

void BinaryTreeTestingSimulation::runTest(bool quietMode)
                                       // QFile &outFile)
{
   QTextStream out(stdout);
   out << endl;
   out << "------------------------------------------------------------------" << endl;
   out << "Beginning simulation with " << endl;
   out << "   population size    = " << populationSize   << endl;
   out << "   group size         = " << groupSize        << endl;
   out << "   infection rate     = " << infectionRate    << " (" << infectionRate * 100 << "%)" << endl;
   out << "   test accuracy rate = " << testAccuracyRate << " (" << testAccuracyRate * 100 << "%)" << endl;

   // Make a list representing the population. We'll need to free this memory later.
   auto *populationList = new QList<bool>;

   // Infect the population.
   int actualInfections = 0;
   actualInfections = infectThePopulationByChoosingPeople(populationList);
   out << actualInfections << " of " << populationSize << " infected." << endl;

   //* For debugging
   if (! quietMode) {
      out << "populationList: ";
      for (int i = 0; i < populationList->length(); i++) {
         out << QString::number(populationList->at(i));
      }
      out << "\n";
   }
   // */

   // Determine groupings.
   div_t result = div(populationSize, groupSize);
   int numberOfGroups = result.quot;
   int extraGroupSize = result.rem;
   out << "Testing " << numberOfGroups << " groups of " << groupSize;
   if (extraGroupSize > 0) {
      // We'll need one extra group for the very last bunch.
      out << " and one group of " << extraGroupSize;
   }
   out << "." << endl;

   // Test...
   testUsageMap.clear();
   int cum_infectionsDetected = 0;
   int cum_testsUsed          = 0;
   int cum_falseNegatives     = 0;
   int cum_falsePositives     = 0;

   int popIndex = 0; // To iterate over the entire population
   int testNum = 0;

   // ... numberOfGroups whole groups plus the last partial group if necessary.
   while (popIndex < populationSize) {
      testNum++;
      // Make a list for this testing group and show it.
      auto *groupList = new QList<bool>;
      if (! quietMode) {
         out << "Test " << testNum << " ";
      }

      int peopleInGroup = groupSize;
      if (peopleInGroup > populationSize - popIndex) {
         // This is last (partial) group.
         numberOfGroups++;
         peopleInGroup = populationSize - popIndex;
      }

      // Make the group.
      for (int j = 0; j < peopleInGroup; j++) {
         if (! quietMode) {
            out << QString::number(populationList->at(popIndex));
         }
         groupList->append(populationList->at(popIndex));
         popIndex++;
      } // end for j
      if (! quietMode) {
         out << endl;
      }

      // Test. (For science!)
      GroupTestResults gtr = {};  // No, not this one: https://en.wikipedia.org/wiki/GTR_(band) .
      gtr = testPool(0, groupList, quietMode);

      // Update usage stats.
      if (testUsageMap.contains(gtr.testsUsed)) {
         testUsageMap[gtr.testsUsed] = testUsageMap[gtr.testsUsed] + 1;
      } else {
         testUsageMap.insert(gtr.testsUsed,1);
      }

      // Update cumulative counts.
      cum_infectionsDetected += gtr.infectionsDetected;
      cum_testsUsed          += gtr.testsUsed;
      cum_falseNegatives     += gtr.falseNegatives;
      cum_falsePositives     += gtr.falsePositives;
   } // while (popIndex < populationSize)

   // Compute the EXPECTED results.
   double pctExpected_noInfections      = 0;
   double pctExpected_1infection        = 0;
   double pctExpected_2orMoreInfections = 0;
   computeExpectedResults(pctExpected_noInfections, pctExpected_1infection, pctExpected_2orMoreInfections);

   // Report ALL of the results.
   out << "--Actual Results--" << endl;
   out << "  false negatives: " << cum_falseNegatives << " (test accuracy " << testAccuracyRate * 100 << "%)" << endl;
   out << "  false positives: " << cum_falsePositives << " (test accuracy " << testAccuracyRate * 100 << "%)" << endl;
   out << "  " << cum_testsUsed << " tests used to detect " << cum_infectionsDetected << " of " << actualInfections << " actual infections." << endl;

   // TODO: Use qSetFieldWidth(int width) to make a table of the results.

   double pctAcc = 0.0;
   QMapIterator<int, int> i(testUsageMap);
   while (i.hasNext()) {
       i.next();
       double tm  = i.value();
       double pct = (tm / numberOfGroups) * 100;  // Um... numberOfGroups *could* be 0.
       out << "  used " << i.key() << " tests " << i.value() << " times, which covered " << pct << "% of the groups ";

       double expectedPct;
       switch (i.key()) {
          case 1:  expectedPct = pctExpected_noInfections; break;
          case 7:  expectedPct = pctExpected_1infection; break;
          case 11: expectedPct = pctExpected_2orMoreInfections; break;
          default: expectedPct = -1;
       }
       if (expectedPct >= 0) {
          double expectedOccurrences = qCeil(numberOfGroups * (expectedPct/100));
          out << "(expected " << expectedPct  << "% = " << expectedOccurrences << " occurrences; ";
          double pctError = 0;
          if (expectedOccurrences > 0) {
             pctError = (abs(i.value() - expectedOccurrences) / expectedOccurrences) * 100;
          }
          out << pctError << "% error.)";
       } else {
          out << "(This is unexpected. ";
          if (extraGroupSize > 0) {
             out << "It could be due to the final partial group of size " << extraGroupSize;
          } else {
             out << " You should investigate";
          }
          out << ".)";
       }
       out << endl;
       pctAcc += pct;
   }
   out << "------------------------------------------------------------------" << endl;
   out << "  Total " << pctAcc << "%" << endl;
   out << endl;
   // out << actualInfections << "," << cum_infectionsDetected << "," << cum_falseNegatives << "," << cum_falsePositives << "," << cum_testsUsed << endl;

   // QTextStream fileOut(&outFile);
   // fileOut << actualInfections << "," << cum_infectionsDetected << "," << cum_falseNegatives << "," << cum_falsePositives << "," << cum_testsUsed << endl;

   // Free the memory we allocated earlier.
   delete populationList;
}


// -------------
// -- Private --
// -------------

int BinaryTreeTestingSimulation::infectThePopulationByChoosingInfections(QList<bool>* populationList)
{
   // For each person, use the infection rate to determine whether or not they are infected.
   int infected = 0;
   for (int i = 0; i < populationSize; i++) {
      // QRandomGenerator::global() is a pointer to a shared QRandomGenerator
      // that was seeded (by the system?) using securelySeeded().
      // The generateDouble() proc generates one random qreal in the range [0, 1) (that is, inclusive of zero and exclusive of 1).
      qreal value = QRandomGenerator::global()->generateDouble();
      bool isInfected = false;
      if ( value <= infectionRate ) {
         isInfected = true;
         infected++;
      } // end if
      populationList->append(isInfected);
   } // end for i
   return infected;
}

int BinaryTreeTestingSimulation::infectThePopulationByChoosingPeople(QList<bool>* populationList)
{
   // Infect <infection rate * population size> random people.
   int infected = 0;

   // Build the population list with the first <numberOfPeopleToInfect> people infected.
   int numberOfPeopleToInfect = populationSize * infectionRate;  // maybe use round() or ceil()
   for (int i = 0; i < populationSize; i++) {
      if (i < numberOfPeopleToInfect) {
         populationList->append(true);
         infected++;
      } else {
         populationList->append(false);
      }
   } // end for i

   // Then shuffle them.  This assures that we never randomly infect the same person more than once.
   // We need help from the C++ standard library here. (Unless there are Qt analogs that I could not find, which is entirely possible.)

   std::random_device rd;   // a uniformly-distributed integer random number generator that produces non-deterministic random numbers
                            // https://en.cppreference.com/w/cpp/numeric/random/random_device

   std::mt19937 gen(rd());  // mersenne_twister_engine is a random number engine based on
                            // Mersenne Twister algorithm (https://en.wikipedia.org/wiki/Mersenne_Twister)
                            // https://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine

   std::shuffle(populationList->begin(), populationList->end(), gen);  // https://en.cppreference.com/w/cpp/algorithm/random_shuffle

   return infected;
}

BinaryTreeTestingSimulation::GroupTestResults BinaryTreeTestingSimulation::testPool(int level,
                                                                                    QList<bool>* groupList,
                                                                                    bool quietMode)
{
   QTextStream out(stdout);
   GroupTestResults gtr = {};
   int pool_infectionsDetected = 0;
   int pool_testsUsed          = 0;
   int pool_falseNegatives     = 0;
   int pool_falsePositives     = 0;

   bool infectionFound = false;

   // Indent to current recursion level.
   if (! quietMode) {
      for (int i = 0; i < level; i++) {
         out << " ";
      }
      out << level << ": Testing group of size " << groupList->count() << " : ";
   }

   if ( (level < divideLevel) && (groupList->count() > 1) ) {
      // Recursive case. Test group or subgroup members in a pool.

      // Use one test for this group/pool.
      pool_testsUsed++;

      // Look for infection(s).
      for (int i = 0; i < groupList->count(); i++) {
         if ( performTest(groupList->at(i), pool_falseNegatives, pool_falsePositives) ) {
            infectionFound = true;
         }
      } // end for

      if (infectionFound) {
         // There is at least one infection detected in the group so we need more testing.
         if (! quietMode) {
            out << "infection found." << endl;
         }

         if (level < (divideLevel-1)) {
            // We're NOT yet at the maximum divide level,
            // so split the list and test both halves of the group at the next level.

            // Figure out how to divide the group in two.
            int part1size = 0;
            int part2size = 0;
            if (groupList->count() % 2 == 0) {
               // There are an even number of items in the group list.
               part1size = groupList->count() / 2;
               part2size = groupList->count() / 2;
            } else {
               // There are an odd number of items in the group list.
               part1size = (groupList->count()-1) / 2;
               part2size = part1size+1;
            } // end if

            // Divide the group in two.
            QList<bool> part1group = groupList->mid(0, part1size);
            QList<bool> part2group = groupList->mid(part1size, part2size);

            // Test the first subgroup with a recursive call.
            gtr = testPool(level+1, &part1group, quietMode);
            pool_infectionsDetected += gtr.infectionsDetected;
            pool_testsUsed          += gtr.testsUsed;
            // pool_falseNegatives     += gtr.falseNegatives; // These are accumulated in pass-by-reference
            // pool_falsePositives     += gtr.falsePositives; // parameters sent to the performTest() proc.

            // Test the second subgroup with a recursive call.
            gtr = testPool(level+1, &part2group, quietMode);
            pool_infectionsDetected += gtr.infectionsDetected;
            pool_testsUsed          += gtr.testsUsed;
            // pool_falseNegatives     += gtr.falseNegatives; // These are accumulated in pass-by-reference
            // pool_falsePositives     += gtr.falsePositives; // parameters sent to the performTest() proc.

         } else {
            // We ARE at the maximum divide level,
            // so DON'T split the list, just test them all with a recursive call.
            gtr = testPool(level+1, groupList, quietMode);
            pool_infectionsDetected += gtr.infectionsDetected;
            pool_testsUsed          += gtr.testsUsed;
            // pool_falseNegatives     += gtr.falseNegatives; // These are accumulated in pass-by-reference
            // pool_falsePositives     += gtr.falsePositives; // parameters sent to the performTest() proc.
         } // end if level < divideLevel-1

      } else {
         if (! quietMode) {
            out << "no infections found." << endl;
         }
      } // end if infection found

   } else {
      // Base case: (level >= divideLevel) or (groupList->count() == 1)
      // We will test each member individually.

      pool_testsUsed += groupList->count();
      for (int i = 0; i < groupList->count(); i++) {
         if ( performTest(groupList->at(i), pool_falseNegatives, pool_falsePositives) ) {
            pool_infectionsDetected++;
         }
      } // end for i
      if (! quietMode) {
         out << "(individual tests) ";
         out << pool_infectionsDetected << " infection(s) found." << endl;
      }
   } // end if level < divideLevel

   gtr.infectionsDetected = pool_infectionsDetected;
   gtr.testsUsed          = pool_testsUsed;
   gtr.falseNegatives     = pool_falseNegatives;
   gtr.falsePositives     = pool_falsePositives;
   return gtr;
}

bool BinaryTreeTestingSimulation::performTest(bool individualIsInfected,
                                              int &falseNegatives,
                                              int &falsePositives)
{
   bool retVal = individualIsInfected;
   qreal value = QRandomGenerator::global()->generateDouble(); // See QRandomGenerator::global() docs and notes above.
   if ( value > testAccuracyRate ) {
      // This test is NOT accurate.
      // Flip the result...
      retVal= !individualIsInfected;
      // .. and tally the error.
      if (individualIsInfected) {
         falseNegatives++;
      } else {
         falsePositives++;
      } // end if
   } // end if
   return retVal;
}

void BinaryTreeTestingSimulation::computeExpectedResults(double &pctExpected_noInfections,
                                                         double &pctExpected_1infection,
                                                         double &pctExpected_2orMoreInfections)
{
   QTextStream out(stdout);

   //* Compute the APPROXIMATE expected results based on selection WITH replacement. Binomial distribution.
   out << "--Expected results based on selection WITH replacement (binomial distribution)--" << endl;
   pctExpected_noInfections   = qPow( (1-infectionRate), groupSize ) * 100;                    // e.g., 2% IR: .98^8 = 0.85076
   pctExpected_2orMoreInfections = qPow( (infectionRate), 2) * 100;                            // e.g., 2% IR: .02^2 = 0.0004
   pctExpected_1infection  = 100 - (pctExpected_noInfections + pctExpected_2orMoreInfections); // e.g., 2% IR:  1-(0.85076+0.0004) = 0.14883

   out << "  Likelihood of NO infections " << pctExpected_noInfections   << "%" << endl;
   out << "  Likelihood of  1 infection  " << pctExpected_1infection  << "%" << endl;
   out << "  Likelihood of 2+ infections " << pctExpected_2orMoreInfections << "%" << endl;
   //*/


   /* Conversation with Glomski:
      - -- -  -
      You're right it's 0.98 to the 8th if you sample WITH replacement.
      If you sample without replacement, then the percentage chance of an all-negative pool of 8 drops,
      but not as much as
         (98/100)*(97/99)*(96/98)*(95/97)*(94/96)*(93/95)*(92/94)*(91/93) .
      That's because we don't know that there are exactly two positives in every group of 100,
      but we do know there are 120 positives in 6000. Hypergeometric distribution!

      The overall effect is that the "cost" of each successive negative in our sample of 8 is
      less when choosing from a bigger, but on-average-equally-as-infected population of 6000:
         - binomial distribution: .98^8 = .85076 (like you said)
         - hypergeometric distribution w/ total population of 100:
               (98/100)*(97/99)*(96/98)*(95/97)*(94/96)*(93/95)*(92/94)*(91/93) = 0.84566 (like you said)
         - hypergeometric distribution w/ total population of 6000 gives .85068
      - -- -  -

      Hypergeometric calculators to test the output of the below code:
         https://aetherhub.com/Apps/HyperGeometric
         https://stattrek.com/online-calculator/hypergeometric.aspx
      Use values: 6000, 8, 120, [0,1,2] and compare to our output.
   */

   //* Compute the EXACT expected results based on selection WITHOUT replacement. Hypergeometric distribution.
   out << "--Expected results based on selection WITHOUT replacement (hypergeometric distribution)--" << endl;

   int totalPopulation = populationSize;
   int totalInfected = populationSize * infectionRate;
   int totalNotInfected = populationSize - (populationSize * infectionRate);

   // 0 infections
   pctExpected_noInfections = 1;  // 0 would be bad becuase we're multiplying.
   double pr_thisSelection = 0.0;
   for (int i = 0; i < groupSize; i++) {
      pr_thisSelection = float(totalNotInfected) / float(totalPopulation);
      pctExpected_noInfections = pctExpected_noInfections * pr_thisSelection;
      totalNotInfected--;
      totalPopulation--;
   }
   pctExpected_noInfections = pctExpected_noInfections * 100;

   // 1 infection
   pctExpected_1infection = 0;
   double pctExpectedThisPass = 1;
   for (int i = 0; i < groupSize; i++) {
      totalPopulation = populationSize;
      totalInfected = populationSize * infectionRate;
      totalNotInfected = populationSize - (populationSize * infectionRate);
      pr_thisSelection = 0.0;
      pctExpectedThisPass = 1;
      for (int j = 0; j < groupSize; j++) {
         // The person at position i is infected.
         if (j == i) {
            pr_thisSelection = float(totalInfected) / float(totalPopulation);
            totalInfected--;
         } else {
            pr_thisSelection = float(totalNotInfected) / float(totalPopulation);
            totalNotInfected--;
         }
         pctExpectedThisPass = pctExpectedThisPass * pr_thisSelection;
         totalPopulation--;
      }
      pctExpected_1infection = pctExpected_1infection + pctExpectedThisPass;
   }
   pctExpected_1infection = pctExpected_1infection * 100;

   // 2+ infections
   pctExpected_2orMoreInfections  = 100 - (pctExpected_noInfections + pctExpected_1infection);
   //*/

   out << "  Likelihood of NO infections " << pctExpected_noInfections   << "%" << endl;
   out << "  Likelihood of  1 infection  " << pctExpected_1infection  << "%" << endl;
   out << "  Likelihood of 2+ infections " << pctExpected_2orMoreInfections << "%" << endl;
}
