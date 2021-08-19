## Qt-based C++ Sample run

<pre>
> <b>./BTTS -q 1000 8 0.02 1.00</b>
</pre>

````
  ____ _______ _______ _____
 |  _ \__   __|__   __/ ____|
 | |_) | | |     | | | (___
 |  _ <  | |     | |  \___ \
 | |_) | | |     | |  ____) |
 |____/  |_|     |_| |_____/
Binary Tree Testing Simulation
version 0.4

Parsing arguments: -q 1000 8 0.02 1.00

------------------------------------------------------------------
Beginning simulation with
   population size    = 1000
   group size         = 8
   infection rate     = 0.02 (2%)
   test accuracy rate = 1 (100%)
20 of 1000 infected.
Testing 125 groups of 8.
--Expected results based on selection WITH replacement (binomial distribution)--
  Likelihood of NO infections 85.0763%
  Likelihood of  1 infection  14.8837%
  Likelihood of 2+ infections 0.04%
--Expected results based on selection WITHOUT replacement (hypergeometric distribution)--
  Likelihood of NO infections 85.0275%
  Likelihood of  1 infection  13.9819%
  Likelihood of 2+ infections 0.990641%
--Actual Results--
  false negatives: 0 (test accuracy 100%)
  false positives: 0 (test accuracy 100%)
  245 tests used to detect 20 of 20 actual infections.
  used 1 tests 105 times, which covered 84% of the groups (expected 85.0275% = 107 occurrences; 1.86916% error.)
  used 7 tests 20 times, which covered 16% of the groups (expected 13.9819% = 18 occurrences; 11.1111% error.)
------------------------------------------------------------------
  Total 100%
```
