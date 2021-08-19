## Sample run

<pre>
******************************************
WELCOME TO THE ADAPTIVE INFECTION SIMULATOR (AIS)
CREATED BY HOPE G NEVEUX IN COLLABORATION WITH DR. ALAN LABOUSEUR MARCH 2021
SIMULATES STATIC POPULATIONS WITH PARAMETER INFECTION RATES AND TESTING ACCURACY
******************************************
INITIALIZING.....

PLEASE ENTER PERCENTAGES AS DECIMALS LIKE 0.05 = 5%
ALL INPUT VALUES ARE VALIDATED BEFORE PROCEEDING

Enter the infection rate: <b>0.02</b>
Infection Rate Accepted

Enter the testing accuracy (sensitivity, TPR): <b>1.0</b>
Testing Accuracy Accepted

Enter the testing specificity (TNR): <b>1.0</b>
Testing Specificity Accepted

Enter the population size as an integer: <b>1000</b>
Population size Accepted

*Disclaimer: All values have been rounded out to four decimal places*
--------------------------------------------------------
Individual True Positive: 0.02
Individual False Positive: 0.0
Individual True Negative: 0.98
Individual False Negative: 0.0
--------------------------------------------------------
Individual Testing Negative Probability: 0.98
Individual Testing Positive Probability: 0.02
--------------------------------------------------------

INFECTING THE POPULATION........
Based on the Infection Rate and Testing Accuracy, the largest Pool size for efficient Pooled Testing is 10

Would you like to override the Pool size? Y/N: y
Enter new Pool Size: 8

The Pool can be split a maximum of 3 times, so the maximum number of testing levels is 4
--------------------------------------------------------
1 TESTING LEVELS = INDIVIDUAL TESTING
2 TESTING LEVEL = ORIGINAL POOLS > INDIVIDUAL TESTING
3 TESTING LEVELS = ORIGINAL POOLS > SUB-POOLS > INDIVIDUAL TESTING
4 TESTING LEVELS = ORIGINAL POOLS > SUB-POOLS > SUB-SUB-POOLS > INDIVIDUAL TESTING
ECT...

--------------------------------------------------------
Please enter the number of desired testing levels: 3
Number of Testing Levels Accepted

GREEN indicates correct classifications
YELLOW indicates incorrect but not dangerous classifications (false positive)
RED indicates incorrect and dangerous classifications (false negative)

Do you want to see Pool classifications? Y/N: y
--------------------------------------------------------
TRUE POSITIVE POOLS

[2] - True Positive [4] - True Positive [8] - True Positive
[27] - True Positive [28] - True Positive [31] - True Positive
[32] - True Positive [52] - True Positive [60] - True Positive
[63] - True Positive [72] - True Positive [78] - True Positive
[79] - True Positive [86] - True Positive [99] - True Positive
[112] - True Positive [115] - True Positive [122] - True Positive
[2, L] - True Positive [4, L] - True Positive [4, R] - True Positive
[8, L] - True Positive [27, R] - True Positive [28, R] - True Positive
[31, L] - True Positive [32, R] - True Positive [52, L] - True Positive
[60, L] - True Positive [63, R] - True Positive [72, L] - True Positive
[78, L] - True Positive [78, R] - True Positive [79, L] - True Positive
[86, L] - True Positive [99, R] - True Positive [112, R] - True Positive
[115, R] - True Positive [122, R] - True Positive [2, L, 0] - True Positive
[4, L, 1] - True Positive [4, R, 3] - True Positive [8, L, 0] - True Positive
[27, R, 2] - True Positive [28, R, 1] - True Positive [31, L, 1] - True Positive
[32, R, 3] - True Positive [52, L, 0] - True Positive [60, L, 0] - True Positive
[63, R, 2] - True Positive [72, L, 0] - True Positive [78, L, 0] - True Positive
[78, R, 3] - True Positive [79, L, 0] - True Positive [86, L, 2] - True Positive
[99, R, 2] - True Positive [112, R, 3] - True Positive [115, R, 3] - True Positive
[122, R, 0] - True Positive


--------------------------------------------------------
LEVEL 1 METRICS - 125 Pools
--------------------------------------------------------
| True Positive: 18 | False Positive: 0 |  Total = 18 > 0.144
| False Negative: 0 | True Negative: 107 | Total = 107 > 0.856


| P(D+|T+): 1.0 | P(D-|T+): 0.0
| P(D+|T-): 0.0 | P(D-|T-): 1.0


LEVEL 2 METRICS - 36 Pools
--------------------------------------------------------
| True Positive: 20 | False Positive: 0 |  Total = 20 > 0.5556
| False Negative: 0 | True Negative: 16 | Total = 16 > 0.4444


| P(D+|T+): 1.0 | P(D-|T+): 0.0
| P(D+|T-): 0.0 | P(D-|T-): 1.0


LEVEL 3 METRICS (INDIVIDUAL TESTING) - 80 Individual Pools
--------------------------------------------------------
| True Positive: 20 | False Positive: 0 |  Total = 20 > 0.25
| False Negative: 0 | True Negative: 60 | Total = 60 > 0.75


| P(D+|T+): 1.0 | P(D-|T+): 0.0
| P(D+|T-): 0.0 | P(D-|T-): 1.0


--------------------------------------------------------
Number of tests used: 241
--------------------------------------------------------
</pre>

