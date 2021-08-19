#include "BinaryTreeTestingSimulation.h"

#include <iostream>
#include <list>
#include <cmath>
#include <random>

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
{
   std::cout << std::endl;
   std::cout << "------------------------------------------------------------------" << std::endl;
   std::cout << "Beginning simulation with " << std::endl;
   std::cout << "   population size    = " << populationSize   << std::endl;
   std::cout << "   group size         = " << groupSize        << std::endl;
   std::cout << "   infection rate     = " << infectionRate    << " (" << infectionRate * 100 << "%)" << std::endl;
   std::cout << "   test accuracy rate = " << testAccuracyRate << " (" << testAccuracyRate * 100 << "%)" << std::endl;

   // Make a vector representing the population.
   std::vector<bool> populationVector;

   // Infect the population.
   int actualInfections = 0;
   actualInfections = infectThePopulationByChoosingPeople(populationVector);
   // actualInfections = infectThePopulationByChoosingInfections(populationVector);
   std::cout << actualInfections << " of " << populationSize << " infected." << std::endl;

   //* For debugging
   if (! quietMode) {
      std::cout << "populationVector: ";
      for (int i = 0; i < populationVector.size(); i++) {
         std::cout << populationVector.at(i) << " ";
      }
      std::cout << "\n";
   }
   // */

   // Determine groupings.
   div_t result = div(populationSize, groupSize);
   int numberOfGroups = result.quot;
   int extraGroupSize = result.rem;
   std::cout << "Testing " << numberOfGroups << " groups of " << groupSize;
   if (extraGroupSize > 0) {
      // We'll need one extra group for the very last bunch.
      std::cout << " and one group of " << extraGroupSize;
   }
   std::cout << "." << std::endl;

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
      std::list<bool> groupList;
      if (! quietMode) {
         std::cout << "Test " << testNum << " ";
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
            std::cout << populationVector.at(popIndex);
         }
         groupList.push_back(populationVector.at(popIndex));
         popIndex++;
      } // end for j
      if (! quietMode) {
         std::cout << std::endl;
      }

      // Test. (For science!)
      GroupTestResults gtr = {};  // No, not this one: https://en.wikipedia.org/wiki/GTR_(band) .
      gtr = testPool(0, groupList, quietMode);

      // Update usage stats.
      auto item = testUsageMap.find(gtr.testsUsed);
      if (item != testUsageMap.end()) {
         // Found. Update.
         testUsageMap[gtr.testsUsed] = testUsageMap[gtr.testsUsed] + 1;
      } else {
         // NOT found. Insert.
         testUsageMap.insert({gtr.testsUsed,1});
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
   std::cout << "--Actual Results--" << std::endl;
   std::cout << "  false negatives: " << cum_falseNegatives << " (test accuracy " << testAccuracyRate * 100 << "%)" << std::endl;
   std::cout << "  false positives: " << cum_falsePositives << " (test accuracy " << testAccuracyRate * 100 << "%)" << std::endl;
   std::cout << "  " << cum_testsUsed << " tests used to detect " << cum_infectionsDetected << " of " << actualInfections << " actual infections." << std::endl;

   // TODO: Use qSetFieldWidth(int width) to make a table of the results.

   double pctAcc = 0.0;

   for (auto it = testUsageMap.begin(); it != testUsageMap.end(); it++) {
      double tm  = it->second;
      double pct = (tm / numberOfGroups) * 100;  // Um... numberOfGroups *could* be 0.
      std::cout << "  used " << it->first << " tests " << it->second << " times, which covered " << pct << "% of the groups ";

      double expectedPct;
      switch (it->first) {
         case 1:  expectedPct = pctExpected_noInfections; break;
         case 7:  expectedPct = pctExpected_1infection; break;
         case 11: expectedPct = pctExpected_2orMoreInfections; break;
         default: expectedPct = -1;
      }
      if (expectedPct >= 0) {
         double expectedOccurrences = ceil(numberOfGroups * (expectedPct/100));
         std::cout << "(expected " << expectedPct  << "% = " << expectedOccurrences << " occurrences; ";
         double pctError = 0;
         if (expectedOccurrences > 0) {
            pctError = (abs(it->second - expectedOccurrences) / expectedOccurrences) * 100;
         }
         std::cout << pctError << "% error.)";
      } else {
         std::cout << "(This is unexpected. ";
         if (extraGroupSize > 0) {
            std::cout << "It could be due to the final partial group of size " << extraGroupSize;
         } else {
            std::cout << " You should investigate";
         }
         std::cout << ".)";
      }
      std::cout << std::endl;
      pctAcc += pct;
   }
   std::cout << "------------------------------------------------------------------" << std::endl;
   std::cout << "  Total " << pctAcc << "%" << std::endl;
   std::cout << std::endl;
   // std::cout << actualInfections << "," << cum_infectionsDetected << "," << cum_falseNegatives << "," << cum_falsePositives << "," << cum_testsUsed << std::endl;

   // QTextStream filestd::cout(&std::coutFile);
   // filestd::cout << actualInfections << "," << cum_infectionsDetected << "," << cum_falseNegatives << "," << cum_falsePositives << "," << cum_testsUsed << std::endl;

   // Free the memory we allocated earlier.
   // delete or clear populationVector vector?
}


// -------------
// -- Private --
// -------------

int BinaryTreeTestingSimulation::infectThePopulationByChoosingInfections(std::vector<bool> &populationVector)
{
   std::random_device rd;   // a uniformly-distributed integer random number generator that produces non-deterministic random numbers
   // https://en.cppreference.com/w/cpp/numeric/random/random_device

   std::mt19937 gen(rd());  // mersenne_twister_engine is a random number engine based on
   // Mersenne Twister algorithm (https://en.wikipedia.org/wiki/Mersenne_Twister)
   // https://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine

   std::uniform_real_distribution<> u_r_dist(0, 1.0);
   // Sets the distribution limits to the range [0, 1) (that is, inclusive of zero and exclusive of 1).
   // See https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution .

   // For each person, use the infection rate to determine whether or not they are infected.
   int infected = 0;
   for (int i = 0; i < populationSize; i++) {
      // Generate one random double in the range [0, 1) (that is, inclusive of zero and exclusive of 1).
      double value = u_r_dist(gen);
      bool isInfected = false;
      if ( value <= infectionRate ) {
         isInfected = true;
         infected++;
      } // end if
      populationVector.push_back(isInfected);
   } // end for i
   return infected;
}

int BinaryTreeTestingSimulation::infectThePopulationByChoosingPeople(std::vector<bool> &populationVector)
{
   // Infect <infection rate * population size> random people.
   int infected = 0;

   // Build the population list with the first <numberOfPeopleToInfect> people infected.
   int numberOfPeopleToInfect = populationSize * infectionRate;  // maybe use round() or ceil()
   for (int i = 0; i < populationSize; i++) {
      if (i < numberOfPeopleToInfect) {
         populationVector.push_back(true);
         infected++;
      } else {
         populationVector.push_back(false);
      }
   } // end for i

   // Then shuffle them.  This assures that we never randomly infect the same person more than once.
   // We need help from the C++ standard library here.

   std::random_device rd;   // a uniformly-distributed integer random number generator that produces non-deterministic random numbers
   // https://en.cppreference.com/w/cpp/numeric/random/random_device

   std::mt19937 gen(rd());  // mersenne_twister_engine is a random number engine based on
   // Mersenne Twister algorithm (https://en.wikipedia.org/wiki/Mersenne_Twister)
   // https://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine

   std::shuffle(populationVector.begin(), populationVector.end(), gen);
   // https://en.cppreference.com/w/cpp/algorithm/random_shuffle

   return infected;
}

BinaryTreeTestingSimulation::GroupTestResults BinaryTreeTestingSimulation::testPool(int level,
                                                                                    const std::list<bool>& groupList,
                                                                                    bool quietMode)
{
   GroupTestResults gtr = {};
   int pool_infectionsDetected = 0;
   int pool_testsUsed          = 0;
   int pool_falseNegatives     = 0;
   int pool_falsePositives     = 0;

   bool infectionFound = false;

   // Indent to current recursion level.
   if (! quietMode) {
      for (int i = 0; i < level; i++) {
         std::cout << " ";
      }
      std::cout << level << ": Testing group of size " << groupList.size() << " : ";
   }

   if ( (level < divideLevel) && (groupList.size() > 1) ) {
      // Recursive case. Test group or subgroup members in a pool.

      // Use one test for this group/pool.
      pool_testsUsed++;

      // Look for infection(s).
      for (bool b : groupList) {
         if ( performTest(b, pool_falseNegatives, pool_falsePositives) ) {
            infectionFound = true;
         } // end if
      } // end for

      if (infectionFound) {
         // There is at least one infection detected in the group so we need more testing.
         if (! quietMode) {
            std::cout << "infection found." << std::endl;
         }

         if (level < (divideLevel-1)) {
            // We're NOT yet at the maximum divide level,
            // so split the list and test both halves of the group at the next level.

            // Figure out how to divide the group in two.
            int part1size = 0;
            if (groupList.size() % 2 == 0) {
               // There are an even number of items in the group list.
               part1size = groupList.size() / 2;
            } else {
               // There are an odd number of items in the group list.
               part1size = (groupList.size()-1) / 2;
            } // end if

            // Divide the group in two.
            std::list<bool> part1group;
            std::list<bool> part2group;
            int i = 0;
            for (bool b : groupList) {
               if (i < part1size) {
                  part1group.push_back(b);
               } else {
                  part2group.push_back(b);
               }
               i++;
            }

            // Test the first subgroup with a recursive call.
            gtr = testPool(level+1, part1group, quietMode);
            pool_infectionsDetected += gtr.infectionsDetected;
            pool_testsUsed          += gtr.testsUsed;
            // pool_falseNegatives     += gtr.falseNegatives; // These are accumulated in pass-by-reference
            // pool_falsePositives     += gtr.falsePositives; // parameters sent to the performTest() proc.

            // Test the second subgroup with a recursive call.
            gtr = testPool(level+1, part2group, quietMode);
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
            std::cout << "no infections found." << std::endl;
         }
      } // end if infection found

   } else {
      // Base case: (level >= divideLevel) or (groupList->count() == 1)
      // We will test each member individually.

      pool_testsUsed += groupList.size();
      for (bool b : groupList) {
         if ( performTest(b, pool_falseNegatives, pool_falsePositives) ) {
            pool_infectionsDetected++;
         } // end if
      } // end for
      if (! quietMode) {
         std::cout << "(individual tests) ";
         std::cout << pool_infectionsDetected << " infection(s) found." << std::endl;
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
   // qreal value = QRandomGenerator::global()->generateDouble(); // See QRandomGenerator::global() docs and notes above.
   double value = 0; // TODO: Fix this.
   if ( value > testAccuracyRate ) {
      // This test is NOT accurate.
      // Flip the result...
      retVal= !individualIsInfected;
      // ... and tally the error.
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
   //* Compute the APPROXIMATE expected results based on selection WITH replacement. Binomial distribution.
   std::cout << "--Expected results based on selection WITH replacement (binomial distribution)--" << std::endl;
   pctExpected_noInfections   = pow( (1-infectionRate), groupSize ) * 100;                    // e.g., 2% IR: .98^8 = 0.85076
   pctExpected_2orMoreInfections = pow( (infectionRate), 2) * 100;                            // e.g., 2% IR: .02^2 = 0.0004
   pctExpected_1infection  = 100 - (pctExpected_noInfections + pctExpected_2orMoreInfections); // e.g., 2% IR:  1-(0.85076+0.0004) = 0.14883

   std::cout << "  Likelihood of NO infections " << pctExpected_noInfections   << "%" << std::endl;
   std::cout << "  Likelihood of  1 infection  " << pctExpected_1infection  << "%" << std::endl;
   std::cout << "  Likelihood of 2+ infections " << pctExpected_2orMoreInfections << "%" << std::endl;
   //*/

   /* Conversation with Glomski:
      - -- -  -
      You're right it's 0.98 to the 8th if you sample WITH replacement.
      If you sample withstd::cout replacement, then the percentage chance of an all-negative pool of 8 drops,
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

      Hypergeometric calculators to test the std::coutput of the below code:
         https://aetherhub.com/Apps/HyperGeometric
         https://stattrek.com/online-calculator/hypergeometric.aspx
      Use values: 6000, 8, 120, [0,1,2] and compare to our std::coutput.
   */

   //* Compute the EXACT expected results based on selection WITHOUT replacement. Hypergeometric distribution.
   std::cout << "--Expected results based on selection WITHOUT replacement (hypergeometric distribution)--" << std::endl;

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

   std::cout << "  Likelihood of NO infections " << pctExpected_noInfections   << "%" << std::endl;
   std::cout << "  Likelihood of  1 infection  " << pctExpected_1infection  << "%" << std::endl;
   std::cout << "  Likelihood of 2+ infections " << pctExpected_2orMoreInfections << "%" << std::endl;
}

