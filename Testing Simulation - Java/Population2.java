//Functions: Generates, infects, and holds the infected population based on the testing accuracy, infection rate, and population size; computes and displays expected probabilities

import java.util.*;
import java.lang.Math;

public class Population2 {
	
	/*GLOBAL VARIABLES...........................................................................................*/

	//General metrics (parameters)
	private int populationSize;

	private double infectionRate;

    //Usually these two values are different, but they can be the same and are usually known for a given test
	private double testingSensitivity; //P(T+ | D+), also known as Testing Accuracy
    private double testingSpecificity; //P(T- | D-)

	//Derived attributes
	private int numberInfected;
	//Marginal probabilities
	private double testPosRate;
	private double testNegRate;

	//Detection probabilities (conditional)
	private double truePos;
    private double falsePos;
    private double trueNeg;
    private double falseNeg;
	
	//Create an ArrayList for the population size
	//"People" will be represented as 1's (Infected) and 0's (Clear)
	ArrayList<Integer> simPopulation = new ArrayList<>();
	

	/*GLOBAL VARIABLES...........................................................................................*/
	
	/*NO ARGS CONSTRUCTOR */
	Population2(){
		TextColors.message("PLEASE ENTER PERCENTAGES AS DECIMALS LIKE 0.05 = 5%");
		TextColors.message("ALL INPUT VALUES ARE VALIDATED BEFORE PROCEEDING\n");

		//Opening the scanner stream for the user to input
		Scanner input = new Scanner(System.in);

		//Take and store valid user input for infection rate, testing accuracy, and population size
		validateInfectionRate(input);
		validateTestingAccuracy(input);
		validatePopulation(input);

        //Compute and store join probabilities
		truePos = roundOut(testingSensitivity * infectionRate);
		falsePos = roundOut((1-testingSensitivity)*(1-infectionRate));
		trueNeg = roundOut(testingSpecificity * (1-infectionRate));
		falseNeg = roundOut((1-testingSpecificity) * infectionRate);

		//Compute marginal probabilities
		testNegRate = roundOut(trueNeg + falseNeg);
		testPosRate = roundOut(truePos + falsePos);

		//Prints out the probabilities for all six variables for clarity
		printIndividualProb();

		System.out.println("INFECTING THE POPULATION........");
		infectPopulation();
		
	}

	/*METHODS*/
	private void infectPopulation(){
		//Find how many people should be infected according to the IR
		//Round because we cannot have partial people and the IR does not need to be an even division (occurs with small populations and specific IRs)
		numberInfected = (int)Math.round(populationSize * infectionRate);

		for(int i = 0; i < populationSize; i++) {
			
			//Assigning Infected or Cleared
			if(i < numberInfected) {
				simPopulation.add(1);
				
			}
			else {
				simPopulation.add(0);
				
			}
			
		} //End i loop

		//Shuffle the simPopulation so we do not know where they are located
		Collections.shuffle(simPopulation);

	}

	private void validatePopulation(Scanner input){
		boolean validInput = false;

		//Validates the population size input by the user
		while(!validInput) {
			System.out.print("Enter the population size as an integer: ");
			populationSize = input.nextInt();

			//Population must have at least 1 person
			if(populationSize < 1){
				TextColors.warning("The population being tested must be at least 1 person\n");

			}
			//Accepted population size
			else if(populationSize >= 1){
				TextColors.verified("Population size Accepted\n");
				validInput = true;

			}
			else {
				//Stops so the loop does not continue to run when there is a software issue
				TextColors.error("DEVELOPMENT ERROR: AIS > Population > validatePopulation");
				return;

			}

		} //End while
		
	}

	private void validateInfectionRate(Scanner input){
		boolean validInput = false;

		//Ensure the infection rate is not greater than or equal to 50% and is non-negative
		while(!validInput){
			System.out.print("Enter the infection rate: ");
			infectionRate = input.nextDouble();

			//Invalid values
			if (infectionRate <= 0 || infectionRate >= 1){
				TextColors.warning("Infection Rate must be between 0 and 1\n");

			} 
			//Ineffective value
			else if(infectionRate >= 0.5){
				TextColors.warning("Pooled Testing becomes ineffective at this infection rate. To conserve tests, please test individually");
				TextColors.warning("Please only use Pooled Testing when infection rates are less than 50%\n");
				
			}
			//Accepted values
			else if(infectionRate < 0.5){
				TextColors.verified("Infection Rate Accepted\n");
				validInput = true;

			}
			else{
				//Stops so the loop does not continue to run when there is a software issue
				TextColors.error("DEVELOPMENT ERROR: AIS > Population > validateInfectionRate");
				return;


			}

		} //End while

	}

	private void validateTestingAccuracy(Scanner input){
		boolean validInput = false;

		//Ensure the testing sensitivity is non-negative and over 50%
		while(!validInput){
			System.out.print("Enter the testing accuracy (sensitivity, TPR): ");
			testingSensitivity = input.nextDouble();

			//Invalid values
			if(testingSensitivity <= 0 || testingSensitivity > 1){
				TextColors.warning("Testing Accuracy must be between 0 and 1. A testing accuracy of 100 percent is permitted\n");

			}
			//Ineffective values
			else if(testingSensitivity <= 0.75){
				TextColors.warning("Pooled Testing becomes ineffective at this testing accuracy. If applicable, choose an alternative test. Otherwise, please test individually");
				TextColors.warning("Please only use Pooled Testing when testing accuracy is at least 75%\n");

			}
			//Accepted values 
			else if(testingSensitivity > 0.75 & testingSensitivity <= 1){
				TextColors.verified("Testing Accuracy Accepted\n");
				validInput = true;

			}
			else{
				//Stops so the loop does not continue to run when there is a software issue
				TextColors.error("DEVELOPMENT ERROR: AIS > Population > validateInfectionRate > Testing Sensitivity");
				return;

			}

		} //End while

        validInput = false;

        //Ensure the testing specificity is non-negative and over 50%
		while(!validInput){
			System.out.print("Enter the testing specificity (TNR): ");
			testingSpecificity = input.nextDouble();

			//Invalid values
			if(testingSpecificity <= 0 || testingSpecificity > 1){
				TextColors.warning("Testing Specificity must be between 0 and 1. A testing specificity of 100 percent is permitted\n");

			}
			//Ineffective values
			else if(testingSpecificity <= 0.75){
				TextColors.warning("Pooled Testing becomes ineffective at this testing specificity. If applicable, choose an alternative test. Otherwise, please test individually");
				TextColors.warning("Please only use Pooled Testing when testing specificity is at least 75%\n");

			}
			//Accepted values 
			else if(testingSpecificity > 0.75 & testingSpecificity <= 1){
				TextColors.verified("Testing Specificity Accepted\n");
				validInput = true;

			}
			else{
				//Stops so the loop does not continue to run when there is a software issue
				TextColors.error("DEVELOPMENT ERROR: AIS > Population > validateInfectionRate > Testing Specificity");
				return;

			}

		} //End while

	}

	//Rounds a double to four decimal places
	public double roundOut(double a){
		return Math.round(a * 10000.0) / 10000.0;

	}

	public void printIndividualProb(){
		TextColors.message("*Disclaimer: All values have been rounded out to four decimal places*");
		TextColors.bannerBreak();
		System.out.println("Individual True " + TextColors.red("Positive") + ": " + truePos);
		System.out.println("Individual False " + TextColors.red("Positive") + ": " + falsePos);
		System.out.println("Individual True " + TextColors.green("Negative") + ": " +  trueNeg);
		System.out.println("Individual False " + TextColors.green("Negative") + ": " + falseNeg);
		TextColors.bannerBreak();
		System.out.println("Individual Testing " + TextColors.green("Negative") + " Probability: " + testNegRate);
		System.out.println("Individual Testing " + TextColors.red("Positive") + " Probability: " + testPosRate);
		TextColors.bannerBreak();
		System.out.println();

	}

	/*GETTERS*/
	public ArrayList<Integer> getSimPopulation(){
		return simPopulation;
		
	}
	
	public int getNumberInfected() {
		return numberInfected;
		
	}

	public int getPopulationSize(){
		return populationSize;

	}

	public double getInfectionRate(){
		return infectionRate;

	}

	public double getTestNegRate(){
		return testNegRate;

	}

	public double getTestPosRate(){
		return testPosRate;

	}

	public double getTestingSensitivity(){
		return testingSensitivity;

	}

	public double getTestingSpecificity(){
        return testingSpecificity;

    }

	public double getTruePos(){
		return truePos;

	}

	public double getFalsePos(){
		return falsePos;

	}

	public double getTrueNeg(){
		return trueNeg;

	}

	public double getFalseNeg(){
		return falseNeg;

	}

	//NO SETTERS, NOT APPLICABLE

}



