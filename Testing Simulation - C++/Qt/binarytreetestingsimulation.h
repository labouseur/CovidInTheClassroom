#ifndef BINARYTREETESTINGSIMULATION_H
#define BINARYTREETESTINGSIMULATION_H

#include <QMap>
#include <QFile>

class BinaryTreeTestingSimulation
{

public:
   BinaryTreeTestingSimulation(int    _populationSize,
                               int    _groupSize,
                               double _infectionRate,
                               double _testAccuracyRate);
   ~BinaryTreeTestingSimulation();

   void runTest(bool quietMode);
                // QFile &outFile


private:

   int    populationSize   = 0;
   int    groupSize        = 0;
   double infectionRate    = 0.0;
   double testAccuracyRate = 0.0;

   int divideLevel = 2;

   QMap<int,int> testUsageMap = {};

   struct GroupTestResults
   {
      int infectionsDetected = 0;
      int testsUsed          = 0;
      int falseNegatives     = 0;
      int falsePositives     = 0;
   };

   int infectThePopulationByChoosingInfections(QList<bool>* populationList);
   int infectThePopulationByChoosingPeople(QList<bool>* populationList);

   GroupTestResults testPool(int level,
                             QList<bool>* groupList,
                             bool quietMode);

   bool performTest(bool individualIsInfected,
                    int &falseNegatives,
                    int &falsePositives);

   void computeExpectedResults(double &pctExpected_noInfections,
                               double &pctExpected_1infection,
                               double &pctExpected_2orMoreInfections);
};

#endif // BINARYTREETESTINGSIMULATION_H
