//Functions: Container for partitions of the Population for which has a status of positive or negative, an ID, people, and potentially Left and Right partitions for later testing

import java.util.*;
import java.lang.Math;

public class Pool{
	//Holds the overall identity of the Pool
	//First spot will have the integer number, the remaining spots will indicate the path down the tree
	ArrayList<Object> id = new ArrayList<>();
	
	//Pointers to the left and right side of the pools
	//May not need to be created if the pool is not positive or this Pool is already a sub-Pool
	//Use setSubPools to create both left and right versions of this Pool
	Pool left;
	Pool right;

	//If the maxlevel split is not met, we may need to have more than 2 Pools so they are kept in a collection here
	//Won't need to be used unless the original pool size is not even
	ArrayList<Pool> individuals = new ArrayList<>();
	
	//It is dangerous to make an assumption about the infection of a pool even with a low infection rate
	//We assume neither positive nor negative to be safe
	boolean positive;
	String caseType;
	ArrayList<Integer> group = new ArrayList<>();
	
	//No args-constructor
	Pool(){}
	
	//A pool can be created by adding a list of people
	//This is useful for creating a new pool from 
	Pool(ArrayList<Integer> people){
		group.addAll(people);
		
	}
	
	/*METHODS*/
	public void addPerson(int person) {
		group.add(person);
		
	}
	
	public boolean isEmpty() {
		if(group.size() == 0) {
			return true;
			
		}
		else {
			return false;
			
		}
		
	}

	//We found in testing the overall Pool was positive
	//Create the sub-pools and set them as the appropriate pointers
	public void setSubPools() {
		//Find the half mark so we can split the Pool and make 2 sub-pools
		//Floored since the Pool won't always be divisible by 2
		int half = Math.floorDiv(group.size(), 2);
		
		//Create the left and right pools
		left = new Pool();
		right = new Pool();
		
		//Run through the original group we have and set the left and right by half mark
		for(int i = 0; i < group.size(); i++) {
			if(i < half) {
				left.addPerson(this.group.get(i));
				
			}
			else if(i >=  half) {
				right.addPerson(this.group.get(i));
				
			}
			else {
				TextColors.error("DEVELOPMENT ERROR: AIS > Pool > setSubPools");
				
			}
			
		}//End i loop

		//The left pool will always be filled first if we have an odd number of people in a Pool
		left.id.addAll(this.id);
		left.id.add("L");

		//To indicate the final left pool has only one person in it
		if(left.getGroup().size() == 1){
			left.id.add("(I)");

		}

		//The right may not have any people in it so we first check before assigning it anything
		//If there is nothing there then we reset the right to be null
		if(right.getGroup().size() != 0){
			right.id.addAll(this.id);
			right.id.add("R");

			//To indicate the final right Pool only has one person in it
			if(right.getGroup().size() == 1){
				right.id.add("(I)");
	
			}

		}
		else{
			right = null;

		}
		
	}

	//We may not have declared maxlevel levels, in which case we need to build Pools of one person
	public void createIndividualPools(){
		for(int i = 0; i < group.size(); i++){
			Pool onePerson = new Pool();
			onePerson.addPerson(this.group.get(i));

			onePerson.id.addAll(this.id);
			onePerson.id.add(i);
			individuals.add(onePerson);
			
		}
		
	}
	
	/*GETTERS*/
	public ArrayList<Object> getID() {
		return id;
		
	}
	
	
	public boolean isPositive() {
		return positive;
		
	}
	
	public ArrayList<Integer> getGroup() {
		return group;
		
	}
	
	public Pool getLeft() {
		return left;
		
	}
	
	public Pool getRight() {
		return right;
		
	}

	public String getCaseType(){
		return caseType;

	}

	public ArrayList<Pool> getIndividualPools(){
		return individuals;

	}
	
	/*SETTERS*/
	//No setID, since we can use ArrayList methods to add to the list
	
	public void setPositive() {
		this.positive = true;
		
	}
	
	public void setNegative() {
		this.positive = false;
		
	}

	public void setCaseType(String caseType){
		this.caseType = caseType;

	}
	
}