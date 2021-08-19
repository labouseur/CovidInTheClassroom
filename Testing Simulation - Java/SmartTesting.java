//Functions: Points to a constructed Population, optimizes Pool size, asks user for the number of splits they want (levels), builds the Pools, tests
//Notes: 1 split means testing original Pools once, with no splitting down

import java.util.*;
import java.lang.Math;

public class SmartTesting {

	private int levels;
	private int poolSize = 0;

	//Needed for the testing sequence
	private int maxLevels;
	private int maxSplits;
	private int testsUsed = 0;
	
	//Will hold <testTruePos, testFalsePos, testFalseNeg, testTrueNeg> in that order in an ArrayList of Integers for each split
	private ArrayList<ArrayList<Integer>> metrics = new ArrayList<>();
	private Population2 population;
	private ArrayList<Pool> originalPools = new ArrayList<>();

	//Stores the Pools (original Pools and sub-Pools) found positive so we can output them to the user
	private ArrayList<Pool> positivePools = new ArrayList<>();

	//Holds the number of Pools (or individuals, in the final case) tested at every level so we can verify output
	private ArrayList<Integer> numberTestedPerLevel = new ArrayList<>();

	private ArrayList<Pool> falseNegativePools = new ArrayList<>();
    
    SmartTesting(){}

    SmartTesting(Population2 pop){
		//Storing the population for general use
		population = pop;

		//Setting the pool size (will be uniform for the population)
		if(pop.getPopulationSize() == 1){
			poolSize = 1;

		}
		else{
			optimizePoolSize();

		}

		//Declare the number of splits for testing (and the testing levels)
		validateLevels();

		buildPools();

		//Point the testPools list to the original list so we can move down the levels in the (i) loop
		ArrayList<Pool> testPools = originalPools;

		//Goes through the levels of testing declared by the user
		for(int i = 0; i < levels; i++){
			ArrayList<Pool> infected = new ArrayList<>();

			//For the level-specific testing measures
			int testFalseNeg = 0;
			int testTrueNeg = 0;
			int testTruePos = 0;
			int testFalsePos = 0;

			for(int j = 0; j < testPools.size(); j++){

				//Generate an infection value
				double infectedVal = Math.random();

                //Given the test specs, the probability of testing positive or negative is conditional of the actual Pool status
                if(!testPools.get(j).getGroup().contains(1)){
                    //NEGATIVE POOL
                    //Pool does not actually contain a positive individual and is negative, but still must be classified based on testing specs

                    if(infectedVal <= Math.pow(pop.getTestingSpecificity(), testPools.get(j).getGroup().size())){
                        //Correct classification TN
                        //Modifying Pool status
                        testPools.get(j).setNegative();
                        testPools.get(j).setCaseType(TextColors.green("True Negative"));

                        testTrueNeg++;

                    }
                    else{
                        //Incorrect but not dangerous classification FP
                        //Modifying Pool status
                        testPools.get(j).setPositive();
						testPools.get(j).setCaseType(TextColors.yellow("False Positive"));

						testFalsePos++;

                        positivePools.add(testPools.get(j));

                        //Need to account for the Pools that do not split all the way down and instead need to force individual Pools
                        //If we don't need to split the Pools into individuals, then we need to check if we can split into regular Left and Right Pools
                        if(i == levels - 2 && maxLevels != levels && levels != 1){
                            testPools.get(j).createIndividualPools();
                            for(int k = 0; k < testPools.get(j).getIndividualPools().size(); k++){
                                infected.add(testPools.get(j).getIndividualPools().get(k));

                            } //End k loop

                        }
                        else if(testPools.get(j).getGroup().size() != 1){
                            testPools.get(j).setSubPools();
                            infected.add(testPools.get(j).getLeft());
                            infected.add(testPools.get(j).getRight());

                        }

                    }

                }
                else{
                    //POSITIVE POOL
                    //Pool contains one or more infections, but must still be classified based on testing specs

                    if(infectedVal <= Math.pow(pop.getTestingSensitivity(), testPools.get(j).getGroup().size())){
                        //Correct classification TP
                        //Modifying Pool status
                        testPools.get(j).setPositive();
                        testPools.get(j).setCaseType(TextColors.green("True Positive"));

                        testTruePos++;

                        positivePools.add(testPools.get(j));

                        //Need to account for the Pools that do not split all the way down and instead need to force individual Pools
                        //If we don't need to split the Pools into individuals, then we need to check if we can split into regular Left and Right Pools
                        if(i == levels - 2 && maxLevels != levels && levels != 1){
                            testPools.get(j).createIndividualPools();
                            for(int k = 0; k < testPools.get(j).getIndividualPools().size(); k++){
                                infected.add(testPools.get(j).getIndividualPools().get(k));

                            } //End k loop

                        }
                        else if(testPools.get(j).getGroup().size() != 1){
                            testPools.get(j).setSubPools();
                            infected.add(testPools.get(j).getLeft());
                            infected.add(testPools.get(j).getRight());

                        }

                    }
                    else{   
                        //Incorrect and dangerous classification FN
                        //Modifying Pool Status
                        testPools.get(j).setNegative();
                        testPools.get(j).setCaseType(TextColors.red("False Negative"));

						testFalseNeg++;

						falseNegativePools.add(testPools.get(j));

                    }


                }//End registering the Pools are Negative or Positive

				//Increment the test counter
				testsUsed++;

			} //End j loop

			//Store the number of Pools (or individual Pools) we're testing at this level
			numberTestedPerLevel.add(testPools.size());

			//Pointing the testPools we want to test in the next level to the current infected list
			testPools.clear();
			testPools.addAll(infected);

			//Updating the level metrics according to what we tracked
			metrics.get(i).set(0, testTruePos);
			metrics.get(i).set(1, testFalsePos);
			metrics.get(i).set(2, testFalseNeg);
			metrics.get(i).set(3, testTrueNeg);

		} //End i loop

		printResults();

    }

	private void optimizePoolSize(){
		//The higher the remainder, the more uniform the distribution of people across Pools
		int optimalModulo = 0;
		int tempPoolSize = 0;
		
		//Determining the largest size of the Pool for worthwhile testing
		//Stepping backwards is faster than going forwards
		for(int k = 12; k >= 2; k--){
			double poolNegRate = Math.pow((population.getTrueNeg()+ population.getFalseNeg()), k);

			//Needed for proper k-th rooting
			double doubleK = (double) k;

			//Once we find the first value where the condition is satisfied, we know this is the largest pool size
			if(poolNegRate > (1.0/ Math.pow(doubleK, 1.0/doubleK))){
				tempPoolSize = k;
				break;

			}
			else if(poolNegRate <= (1/Math.pow(doubleK, 1.0/doubleK))){
				//The pool is still too big for testing to be efficient

			}
			else{
				//Stops the for loop so it doesn't continue to run if there are issues
				TextColors.error("DEVELOPMENT ERROR: AIS > Population > optimizePoolSize");
				return;

			}

		} //End k loop

		//Finds the size with the most even distribution with the restriction of the largest pool size
		//Also works backwards so we find perfect bigger pool sizing first, since we want to save as many tests as possible
		for(int i = tempPoolSize; i >= 2; i--){
			if(population.getPopulationSize() % i == 0) {
				poolSize = i;
				break;
				
			} 
			else if(optimalModulo < population.getPopulationSize() % i){
				poolSize = i;
				optimalModulo = population.getPopulationSize() % i;
				
			}

		} //End i loop

		TextColors.message("Based on the Infection Rate and Testing Accuracy, the largest Pool size for efficient Pooled Testing is " + poolSize + "\n");
		
		//Pool size override switch
		Scanner input4 = new Scanner(System.in);
		System.out.print("Would you like to override the Pool size? Y/N: ");
		String overridePoolSize = input4.nextLine();

		if(overridePoolSize.equalsIgnoreCase("Y")){
			System.out.print("Enter new Pool Size: ");
			int newPoolSize = input4.nextInt();
			poolSize = newPoolSize;
			System.out.println();

		}else{
			//Do nothing

		}


	}

	//Creates the Pools according to whatever the pool size was declared as
	//Forced uniform Pool sizing so the entire population will be accounted for
	private void buildPools(){
		Pool tempPool = new Pool();

		for(int i = 0 ; i < population.getPopulationSize(); i++){
			tempPool.addPerson(population.getSimPopulation().get(i));
			
			//Adding the Pool and creating a blank one OR adding the final pool
			if(tempPool.getGroup().size() == poolSize || i == population.getPopulationSize() - 1){
				//Setting the front-most part of the ID as the index it will be
				tempPool.getID().add(originalPools.size());
				originalPools.add(tempPool);
				tempPool = new Pool();

			}
			

		} //End i loop

	}

	//Take the number of levels to the testing sequence and verify them
	//Also build the metrics ArrayList that holds all <truePos, falsePos, falseNeg, trueNeg> values for each level so we can index in and increment as needed
	private void validateLevels(){
		//Initialize the Pools based on the number of levels desired for this testing sequence
		Scanner input2 = new Scanner(System.in);
		boolean validInput = false;

		//Given the size of the original Pool, we can only split so many times, and that should be log base 2 of the Pool size
		//Need to round in the event the Pool size is not even
		maxSplits = (int)Math.round((Math.log(poolSize) / Math.log(2)));

		//The max number of levels will be one more than the maxSplits since we count the original Pools as the first level
		maxLevels = maxSplits + 1;
		TextColors.message("The Pool can be split a maximum of " + maxSplits + " times, so the maximum number of testing levels is " + maxLevels);

		TextColors.bannerBreak();
		TextColors.message("1 TESTING LEVELS = INDIVIDUAL TESTING \n" +
		"2 TESTING LEVEL = ORIGINAL POOLS > INDIVIDUAL TESTING \n" + 
		"3 TESTING LEVELS = ORIGINAL POOLS > SUB-POOLS > INDIVIDUAL TESTING \n" +
		"4 TESTING LEVELS = ORIGINAL POOLS > SUB-POOLS > SUB-SUB-POOLS > INDIVIDUAL TESTING \n" +
		"ECT...\n");
		TextColors.bannerBreak();

		//Validate the number of levels input by the user
		while(!validInput){
			System.out.print("Please enter the number of desired testing levels: ");
     		levels = input2.nextInt();

			//Must be at least one level
			if(levels <= 0){
				TextColors.warning("There must be 1 or more testing levels\n");

			}
			else if(levels > maxLevels){
				TextColors.warning("The number of testing levels declared is more than the number of available levels\n");

			}
			else if (levels > 0 & levels <= maxLevels) {
				//We had optimized the Pool size assuming the user did not pick individual testing, so we must declare the pool size here
				if(levels == 1){
					poolSize = 1;

				}

				TextColors.verified("Number of Testing Levels Accepted\n");
				validInput = true;

			}
			else{
				//Stops so the loop does not continue to run when there is a software issue
				TextColors.error("DEVELOPMENT ERROR: AIS > Testing > validateSplits");
				return;

			}

		} //End while

		buildMetricsList();

	}

	//Creates placeholders for each level with truePos, falsePos, falseNeg, and trueNeg counts set originally to 0
	private void buildMetricsList(){
		ArrayList<Integer> levelMetrics = new ArrayList<>(Arrays.asList(0,0,0,0));
		metrics.add(levelMetrics);
		
		//Append on the other lists for the other levels
		if(levels != 1){
			for(int i = 1; i < levels; i++){
				levelMetrics = new ArrayList<>(Arrays.asList(0,0,0,0));
				metrics.add(levelMetrics);
	
			} //End i loop

		}
		
	}

	//Prints and formats the metrics to the user in the form of a coincidence matrix
	//Prints and formats the Pools that registered Positive (false and true)
	//Prints and formats the Pools that incorrectly registered (false positive and false negative)
	//Computes total positive and negative cases along with percentages
	public void printResults(){
		//Descriptive message about the color coding
		TextColors.message("GREEN indicates correct classifications \nYELLOW indicates incorrect but not dangerous classifications (false positive) \nRED indicates incorrect and dangerous classifications (false negative)\n");

		//When you're testing large populations you may only want the metrics
		Scanner input3 = new Scanner(System.in);
		System.out.print("Do you want to see Pool classifications? Y/N: ");
		String printOutAnswer = input3.nextLine();

		if(printOutAnswer.equalsIgnoreCase("Y")){
			//If there were no positive pools then nothing will print about them
			if(positivePools.size() != 0){
				TextColors.bannerBreak();
				System.out.println("TRUE POSITIVE POOLS\n");

			}
			
			//Purely for console formatting purposes
			int h = 0;
			boolean newLine = false;

			for(int j = 0; j < positivePools.size(); j++){

				if(positivePools.get(j).getCaseType().equalsIgnoreCase(TextColors.green("True Positive"))){
					System.out.print(positivePools.get(j).getID() + " - " + positivePools.get(j).getCaseType() + " ");
					newLine = false;
					h++;

				}

				if(h % 3 == 0 && !newLine || j == positivePools.size() - 1){
					System.out.println();
					newLine = true;

				}

			}//End j loop


			//Prints out the break and message only if needed
			if(positivePools.size() != 0 && falseNegativePools.size() != 0){
				TextColors.bannerBreak();

			}

			if(falseNegativePools.size() != 0){
				System.out.println("FALSE NEGATIVE POOLS\n");

			}
			
			//m allows for proper console formatting
			for(int k = 0, m = 1; k < falseNegativePools.size(); k++, m++){
				System.out.print(falseNegativePools.get(k).getID() + " - " + falseNegativePools.get(k).getCaseType() + " ");

				//Starts a new line when 5 Pools have been printed out
				if(m % 3 == 0){
					System.out.println();
				}

			}//End k loop

			System.out.println("\n");

		}

		TextColors.bannerBreak();

		//Metrics
		for(int i = 0; i < metrics.size(); i++){
			int levelNumber = i + 1;

			//If 1, then we did individual testing, which we previously considered Level 0 but we incremented to force through testing
			//The metrics list and the numberTestedPerLevel will be the same size, so we can use i to index in correctly
			if(i == levels - 1){
				System.out.println("LEVEL " + levelNumber + " METRICS (INDIVIDUAL TESTING) - " + numberTestedPerLevel.get(i) + " Individual Pools");

			}
			else{
				System.out.println("LEVEL " + levelNumber + " METRICS - " + numberTestedPerLevel.get(i) + " Pools");

			}
			
			TextColors.bannerBreak();

			int totalPos = metrics.get(i).get(0) + metrics.get(i).get(1);
			double totalPosRate = roundOut((double)totalPos/(double)numberTestedPerLevel.get(i));

			int totalNeg = metrics.get(i).get(2) + metrics.get(i).get(3);
			double totalNegRate = roundOut((double)totalNeg/(double)numberTestedPerLevel.get(i));

			//True Pos is in first position, False Pos is in second, False Neg is in the third, True Neg is in the fourth
			System.out.println("| " + TextColors.green("True Positive") + ": " + metrics.get(i).get(0) + " | " + TextColors.yellow("False Positive") + ": " + metrics.get(i).get(1) + " | " + " Total = " + totalPos + " > " + totalPosRate + "\n" +
			"| " + TextColors.red("False Negative") + ": " + metrics.get(i).get(2) + " | " + TextColors.green("True Negative") + ": " + metrics.get(i).get(3) + " | " + "Total = " + totalNeg + " > " + totalNegRate + "\n");

			//Creates a break
			System.out.println();

			//Prints the conditional probabilities based on this simulation for how likely it is for you to have or not have the infection based on your test results
			System.out.println("| " + TextColors.green("P(D+|T+)") + ": " + roundOut(metrics.get(i).get(0)/(double)totalPos) + " | " + TextColors.yellow("P(D-|T+)") + ": " + roundOut(metrics.get(i).get(1)/(double)totalPos) + "\n" +
			"| " + TextColors.red("P(D+|T-)") + ": " + roundOut(metrics.get(i).get(2)/(double)totalNeg) + " | " + TextColors.green("P(D-|T-)") + ": " + roundOut(metrics.get(i).get(3)/(double)totalNeg) + "\n");

			//Creates a break
			System.out.println();

		} //Ends i loop

		TextColors.bannerBreak();
		System.out.println("Number of tests used: " + testsUsed);
		TextColors.bannerBreak();

	}

	//Rounds a double to four decimal places
	public double roundOut(double a){
		return Math.round(a * 10000.0) / 10000.0;

	}

	//GETTERS
	public int getLevels(){
		return levels;

	}

	public int getPoolSize() {
		return poolSize;
		
	}

	public ArrayList<ArrayList<Integer>> getMetrics(){
		return metrics;

	}

	public int getMaxLevels(){
		return maxLevels;

	}

	public int getMaxSplits(){
		return maxSplits;

	}

	public ArrayList<Pool> getOriginalPools(){
		return originalPools;

	}
	
	public ArrayList<Pool> getPositivePools(){
		return positivePools;

	}

	public ArrayList<Pool> getFalseNegativePools(){
		return falseNegativePools;

	}

	public int getTestsUsed(){
		return testsUsed;

	}

}

