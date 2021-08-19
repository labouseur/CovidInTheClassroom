/*
 * Author: Hope Neveux
 * Collaborator: Dr. Alan Labouseur
 * Date Created: 3/25/2021
 * Due Date: 5/28/2021
 * Project: Adaptive Infection Simulator (AIS)
 * Purpose: Applying Bayesian statistics to combinatoral group testing.
 *          Analyzes risk and identifies sources of potential test waste and inefficiency.
 *          Reports 
 * 
 * List of Imports: 
 * 		java.io*	-	Read user input (takes population values) via Scanner
 * 		java.util.*	-	ArrayLists, Integer
 * 		java.lang.Math - Math operations
 * 
 */

public class main {

	public static void main(String[] args) {
	
		welcome();
		
		/*START: SIMULATION*/
		
		//Population sim1Pop = new Population();
		//Testing sim1 = new Testing(sim1Pop);
		//miniTest propHyperG2 = new miniTest(sim1Pop);
		//HGTesting propHyperG4 = new HGTesting(sim1Pop);

		Population2 sim2Pop = new Population2();
		SmartTesting smartTest = new SmartTesting(sim2Pop);
		//SmartHGTesting smartHGT = new SmartHGTesting(sim2Pop);

		/**END: SIMULATION*/

	}
	
	private static void welcome() {
		//Ensures the bannerBreak is uniform
		String bannerBreak = "******************************************";
		
		System.out.println(bannerBreak);

		TextColors.welcome("WELCOME TO THE ADAPTIVE INFECTION SIMULATOR (AIS) \n" +
		"CREATED BY HOPE G NEVEUX IN COLLABORATION WITH DR. ALAN LABOUSEUR MARCH 2021 \n" + 
		"SIMULATES STATIC POPULATIONS WITH PARAMETER INFECTION RATES AND TESTING ACCURACY");

		System.out.println(bannerBreak);
		System.out.println("INITIALIZING.....");
		System.out.println();	//Console break line for readability
		
	}

}
