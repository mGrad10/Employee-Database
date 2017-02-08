/*
 * employee_db.c
 *
 * This file contains code to implement an Employee Database Program
 *
 * This file is part of COMP 280, Project 01
 *
 * Authors:
 *  1. Taylor Coury (tacoury@sandiego.edu)
 *  2. Melinda Grad (mgrad@sandiego.edu)
 *
 * Date: 9/21/2016
 *
 */

#include <stdio.h>      // the C standard I/O library
#include <stdlib.h>     // the C standard library
#include <string.h>     // the C string library
#include "readfile.h"   // my file reading routines
#include <math.h>		// for binary search
#include <ctype.h>		//for toupper()
 
//constants
#define MAXFILENAME  128
#define MAXNAME      64
#define MAXEMPLOYEES 1024

//struct definition
struct Employee{

	int employeeId;
	char firstName[MAXNAME];
	char lastName[MAXNAME];
	int salary;
};

// forward declaration of functions
void getFilenameFromCommandLine(char filename[], int argc, char *argv[]);
void readAndPrintFile(char *filename);
int readDatabase( char *filename, struct Employee employeeArray[]);
void printDatabase(struct Employee employeeArray[], int array_length);
void lookupByLastName( struct Employee employeeArray[], int array_length);
void sortById(int array_length, struct Employee employeeArray[]);
void lookupById( struct Employee employeeArray[], int array_length, int sort_flag);
int binarySearch(struct Employee employeeArray[], int target, int start, int end);
void sortByLastName(int array_length, struct Employee employeeArray[]);
int addEmployee(int array_length, struct Employee employeeArray[], int sort_flag);
void shift(int length, struct Employee employeeArray[], struct Employee newEmployee, int index);
int findIndexLName(int length, struct Employee employeeArray[], struct Employee newEmployee);
int findIndexId(int length, struct Employee employeeArray[], struct Employee newEmployee);
struct Employee getEmployeeInfo();
void updateEmployee(int length, struct Employee employeeArray[], int sort_flag);
int findEmployeeToUpdate(struct Employee employeeArray[], int length);

int main (int argc, char *argv[]) {

	char filename[MAXFILENAME];
	struct Employee employeeArray[MAXEMPLOYEES];
	int end_flag = 0;
	int array_length;
	int sort_flag = 0;//0 for sorted by ID(default) and 1 for sorted by last name

	// this initializes the filename string from the command line arguments
	getFilenameFromCommandLine(filename, argc, argv);

	//read in values from the DB file
	array_length = readDatabase(filename, employeeArray);
	sortById(array_length, employeeArray);
	//main loop to prompt menu fo choices
	while(!end_flag) {
		int choice;
		printf("\nEmployee DB Menu:\n"
			"--------------------------------------------\n"
			"(1) Print the Database \n(2) Lookup by ID \n"
			"(3) Lookup by Last Name \n(4) Add an Employee \n(5) Quit \n"
			"(6) Update Employee Info \n(7) Sort by Last Name \n(8) Sort by ID \n"
			"--------------------------------------------\n");

		printf("Enter your choice : ");
		scanf("%d", &choice);

		//Check for vaild menu choice
		while(choice <1 || choice >8) { //<--- Will this while loop work? 
			printf("\nInvalid Entry!\n");
			break;
		}
		switch(choice) {

			case 1: //print database
				printDatabase(employeeArray, array_length);
				break;

			case 2: //lookup by id
				lookupById(employeeArray, array_length, sort_flag);
				break;

			case 3: //lookup by lastN
				lookupByLastName(employeeArray,array_length);
				break;

			case 4: //add employee
				array_length = addEmployee(array_length, employeeArray, sort_flag);
				break;

			case 5 : //quit
				printf("You have choosen to quit the program, goodbye!\n");
				end_flag = 1;
				break;

			case 6: //update info
				updateEmployee(array_length, employeeArray, sort_flag);
				break;

			case 7: //sort by lastN
				sort_flag = 1;
				sortByLastName(array_length, employeeArray);
				printDatabase(employeeArray, array_length);
				break;

			case 8: // sort by ID
				sort_flag = 0;
				sortById(array_length, employeeArray);
				printDatabase(employeeArray, array_length);
				break;
		}
	}
	return 0;
}
/*
 * This is a function to print the database, and the number of employees in the DB
 * @param employeeArray the array of structs to store the employee data
 * @param array_length the length of the array that was passed in as the
 * first argument
 */
void printDatabase(struct Employee employeeArray[], int array_length) {

	printf("\nNAME				ID		SALARY 		\n"
				"-------------------------------------------------------------------\n");	
	for( int i =0; i< array_length; i++){
			printf("%-15s %-15s ", employeeArray[i].firstName, employeeArray[i].lastName);
			printf("%-15d %d\n", employeeArray[i].employeeId, employeeArray[i].salary);
	}
	printf("\nNumber of Employees in the database: %d \n",array_length);
}

/*
* This is a function to find the index of the employee of a given ID
* to be updated
* @param employeeArray the array of structs to store the employee data
* @param length the length of the array that was passed in as an argument
* @return index the index to store the updated employee info
*/
int findEmployeeToUpdate(struct Employee employeeArray[], int length){
	int invaid_flag = 0;
 	int target_id =0;
 	int index=-1;

	//Validate user enters 6 digits 
 	while(!invaid_flag){
	 	printf("Enter a 6 digit employee id: ");
	 	scanf("%d", &target_id);
	 	if(target_id<100000 || target_id> 999999){
	 		printf("Please enter a 6 digit number!\n");
	 	}
	 	else{
	 		invaid_flag = 1;
	 	}
	}
	for(int i =0; i<length;i++){
		if(target_id==employeeArray[i].employeeId){
			index = i;
			break;
		}
		else{
			if(i == length-1){
				printf("\nEmployee: %d not found in DB: CANNOT be updated!\n", target_id);
				index = -1;
				break;
			}
		}
	}
	return index;
}
/*
 * This is a function that allows the user to update an existing employee's info
 * @param employeeArray the array of structs to store the employee data
 * @param length the length of the array that was passed in as an argument
 * @sort_flag to indicate whether the array has been sorted by name or ID
 */
void updateEmployee(int length, struct Employee employeeArray[], int sort_flag){
	int new_id=0, new_salary =0;
	char new_first[MAXNAME], new_last[MAXNAME];
	int index;
	int invalid_flag=0;

	index = findEmployeeToUpdate(employeeArray, length);
	if(index!=-1){
		printf("Enter a new first name, if there is no update enter 0: \n");
		scanf("%s", new_first);
		if((strcmp(new_first,"0"))!=0){
			new_first[0] = toupper(new_first[0]);
			strcpy(employeeArray[index].firstName,new_first);
		}
		printf("Enter a new last name, if there is no update enter 0: \n");
		scanf("%s", new_last);
		if((strcmp(new_last,"0"))!=0){
			new_last[0] = toupper(new_last[0]);
			strcpy(employeeArray[index].lastName,new_last);
		}
		while(!invalid_flag){
		 	printf("Enter a new ID, if there is no update enter 0: \n");
			scanf("%d", &new_id);
			if(new_id == 0){
				break;
			}
	 		if(new_id<100000 || new_id> 999999){ ///fix me!
	 		printf("Please enter a 6 digit number!\n");
	 		}
	 		else{
	 			invalid_flag = 1;
	 		}
	 	}
		if(new_id!=0){
			employeeArray[index].employeeId=new_id;
		}
		invalid_flag=0;
		while(!invalid_flag){
		 	printf("Enter a new salary, if there is no update enter 0: \n");
			scanf("%d", &new_salary);
			if(new_salary == 0){
				break;
			}
	 		if(new_salary<30000 || new_salary> 150000){ ///fix me!
	 		printf("Please enter a valid salary!\n");
	 		}
	 		else{
	 			invalid_flag = 1;
	 		}
	 	}
		if(new_salary!=0){
			employeeArray[index].salary=new_salary;
		}
	}
	if(index!=-1){
		printf("\nNAME				ID		SALARY 		\n"
					"-------------------------------------------------------------------\n");
		printf("%-15s %-15s ", employeeArray[index].firstName, employeeArray[index].lastName);
		printf("%-15d %d\n", employeeArray[index].employeeId, employeeArray[index].salary);
		//resort if DB was previously sorted
		if(sort_flag){
			sortByLastName(length, employeeArray);
		}
		else{
			sortById(length, employeeArray);
		}
	}
}

/*
 * This is a function to shift the values stored in indices of the array and
 * store the new stuct in the open index
 * @param length the length of the array that was passed in as an argument
 * @param employeeArray the array of structs to store the employee data
 * @param newEmployee the struct that stores the new employee
 * @param index the employeeArray index to put the new employee in
 */
 void shift(int length, struct Employee employeeArray[], struct Employee newEmployee, int index){
 	int i;
 	for(i = length-1; i >= index; i--){
 		employeeArray[i+1] = employeeArray[i]; 
 	}
 	employeeArray[index] = newEmployee;
 }

/*
 * This is a function to find the employeeArray index to place the new employee in
 * if sorted by ID
 * @param length the length of the array that was passed in as an argument
 * @param employeeArray the array of structs to store the employee data
 * @param newEmployee the struct that stores the new employee
 * @return the index to store the new employee
 */
 int findIndexId(int length, struct Employee employeeArray[], struct Employee newEmployee){
 	int i;
 	for(i= 0; i< length; i++){
 		if(newEmployee.employeeId > employeeArray[i].employeeId){
 			continue;
 		}
 		else
 			break;
 	}
 	return i;
 }

/*
 * This is a function to find the employeeArray index to place the new employee in
 * if sorted by last name
 * @param length the length of the array that was passed in as an argument
 * @param employeeArray the array of structs to store the employee data
 * @param newEmployee the struct that stores the new employee
 * @return the index to store the new employee
 */
 int findIndexLName(int length, struct Employee employeeArray[], struct Employee newEmployee){
 	int result, i;
 	for(i= 0; i< length; i++){
 		result = strcmp(newEmployee.lastName, employeeArray[i].lastName);
 		if(result>0){
 			continue;
 		}
 		else
 			break;
 	}
 	return i;
 }

/*
 * This is a helper function that will prompt the user for new employee information
 * that will be stored in an employee struct
 * @return newEmployee to store new employee info 
 */
 struct Employee getEmployeeInfo(){
 	struct Employee newEmployee;
	int invalid_flag = 0;

	printf("Please enter the employee's first name: ");
	scanf("%s",newEmployee.firstName);
	newEmployee.firstName[0] = toupper(newEmployee.firstName[0]);
	printf("Please enter the employee's last name: ");
	scanf("%s",newEmployee.lastName);
	newEmployee.lastName[0] = toupper(newEmployee.lastName[0]);

	while(!invalid_flag){
	 	printf("Please enter the employee's 6 digit id: ");
	 	scanf("%d", &newEmployee.employeeId);
	 	if(newEmployee.employeeId<100000 || newEmployee.employeeId> 999999){ ///fix me!
	 		printf("Please enter a 6 digit number!\n");
	 	}
	 	else{
	 		invalid_flag = 1;
	 	}
	 }
	invalid_flag = 0;
	while(!invalid_flag){
	 	printf("Please enter the employee's salary: ");
	 	scanf("%d", &newEmployee.salary);
	 	if(newEmployee.salary<30000 || newEmployee.salary> 150000){
	 		printf("Please enter a valid salary!\n");
	 	}
	 	else{
	 		invalid_flag = 1;
	 	}
	 }

		return newEmployee;
 }

 /*
 * This is a function to add a new employee to the database.
 * @param employeeArray the array of structs to store the employee data
 * @param length the length of the array that was passed in as an argument
 * @sort_flag to indicate whether the array has been sorted by name or ID
 * @return array_length the length of the array
 */
int addEmployee(int array_length, struct Employee employeeArray[], int sort_flag){
	int index=-1;

	struct Employee newEmployee = getEmployeeInfo();
	for(int i =0; i<array_length;i++){
		if(newEmployee.employeeId==employeeArray[i].employeeId){
			index = i;
			break;
		}
		else{
			if(i == array_length-1){
				index = -1;
				break;
			}
		}
	}
	if(index!=-1){ // This will not add employee without a unique ID
		printf("\nThis employee ID exists in the database: Employee NOT added\n");
		return array_length;
	}

	printf("\nNAME				ID		SALARY 		\n"
				"-------------------------------------------------------------------\n");
	printf("%-15s %-15s ", newEmployee.firstName, newEmployee.lastName);
	printf("%-15d %d\n", newEmployee.employeeId, newEmployee.salary);

	int response;
	printf("Do you really want to add this employee?(Enter 0 for NO and 1 for YES)\n");
	scanf("%d", &response);

	if(response){
		if(array_length == 1024){
			printf("The database is full. The employee cannot be added.");
			return array_length;
		}
		else{
			int index;
			if(sort_flag){
				index = findIndexLName(array_length, employeeArray, newEmployee);
				shift(array_length, employeeArray, newEmployee, index);

				return array_length+1;
			}
			else{
				index = findIndexId(array_length, employeeArray, newEmployee);
				shift(array_length, employeeArray, newEmployee, index);
				return array_length+1;
			}
		}
	}
	else{
		return array_length;
	}
}

 /*
 * This is a function to search for an employee by last name
 * @param employeeArray : The DB to search
 * @param array_length : the length of the array passed in
 */
 void lookupByLastName( struct Employee employeeArray[], int array_length) {
	
	char target_name[MAXNAME];

	printf("Enter Employee's last name (no extra spaces):");
	scanf("%s", target_name);
	target_name[0] = toupper(target_name[0]); //if last name is not capitalized

	for(int i =0; i< array_length;i++){

		if(strcmp(target_name,employeeArray[i].lastName)==0){
			printf("\nNAME				ID		SALARY 		\n"
				"-------------------------------------------------------------------\n");
			printf("%-15s %-15s ",employeeArray[i].lastName,employeeArray[i].firstName);
			printf("%-15d %d\n", employeeArray[i].employeeId,employeeArray[i].salary);
			break;
		}
		else{
			if( i == array_length-1){
				printf("\nEmployee: %s not found in DB!\n", target_name);
				break;
			}
		}
	}
	
 }
/* This is a function to search for an employee by ID
 * @param employeeArray : The DB to search
 * @param array_length : the length of the array passed in
 */
 void lookupById( struct Employee employeeArray[], int array_length, int sort_flag){
 	int invaid_flag = 0;
 	int target_id =0;
 	if(sort_flag){
 		sortById(array_length, employeeArray);
 	}
 	//Validate user enters 6 digits 
 	while(!invaid_flag){
	 	printf("Enter a 6 digit employee id: ");
	 	scanf("%d", &target_id);
	 	if(target_id<100000 || target_id> 999999){
	 		printf("Please enter a 6 digit number!\n");
	 	}
	 	else{
	 		invaid_flag = 1;
	 	}
	 }

 	int result = binarySearch(employeeArray,target_id,0,array_length);

 	if(result!= -1){
 		printf("\nNAME				ID		SALARY 		\n"
				"-------------------------------------------------------------------\n");
 		printf("%-15s %-15s ",employeeArray[result].firstName,employeeArray[result].lastName);
		printf("%-15d %d\n", employeeArray[result].employeeId,employeeArray[result].salary);
 	}
 	else{
 		printf("\nEmployee with ID: %d was not found in the DB\n", target_id);
 	}
 	if(sort_flag){
 		sortByLastName(array_length, employeeArray);
 	}
 	
 }
/*
 * This is a function to search the DB for an employee by ID using the
 * divide and conquer menthod
 * @param employeeArray The DB to sort
 * @param target the employee ID to find
 * @param start the starting value of the index range to search
 * @param end the ending value of the index range to search
 * @return middle the index which contains the target
 */
 int binarySearch(struct Employee employeeArray[], int target, int start, int end){
 	
 	if(start > end){ return -1; }

 	int middle = floor((start + end)/2);
 	int value = employeeArray[middle].employeeId;

 	if(value > target){return binarySearch(employeeArray, target, start, middle-1);}
 	if(value < target){return binarySearch(employeeArray, target, middle+1,end);}
 	return middle;

 }

 /*
 * This is a function to sort the DB by Employee ID
 * @param array_length: The length of the passed in array
 * @param employeeArray: The DB to sort
 */
 void sortById(int array_length, struct Employee employeeArray[]){

 	struct Employee temp[MAXEMPLOYEES];
 	int swap = 0;

 	for(int i=0; i< array_length-1; i++){
 		swap = 0;

 		for(int j=0; j< array_length-1-i;j++){
 			if(employeeArray[j].employeeId > employeeArray[j+1].employeeId){

 				temp[1] = employeeArray[j];
 				employeeArray[j] = employeeArray[j+1];
 				employeeArray[j+1] = temp[1];

 				swap = 1;
 			}
 		}
 		if(swap ==0){
 			break;
 		}
 	}

 }

/*
 * This is a function to sort the DB alphabetically by first name then by 
 * last name
 * @param array_length: The length of the passed in array
 * @param employeeArray: The DB to sort
 */
 void sortByLastName(int array_length, struct Employee employeeArray[]){

 	struct Employee temp[MAXEMPLOYEES];
 	int swap = 0;
 	int result;
 	int result1;

 	for(int i=0; i< array_length-1; i++){
 		swap = 0;

 		for(int j=0; j< array_length-1-i; j++){
 			result = strcmp(employeeArray[j].lastName, employeeArray[j+1].lastName);
 			if(result > 0){
 				temp[1] = employeeArray[j];
 				employeeArray[j] = employeeArray[j+1];
 				employeeArray[j+1] = temp[1];

 				swap = 1;
 			}
 			else if(result == 0){
 				result1 = strcmp(employeeArray[j].firstName, employeeArray[j+1].firstName);
 				if(result1 > 0){
 					temp[1] = employeeArray[j];
 					employeeArray[j] = employeeArray[j+1];
 					employeeArray[j+1] = temp[1];

 					swap = 1;
 				}
 			}

 		}
 		if(swap == 0){
 			break;
 		}
 	}
 }

/*
 * This is a function to read a file and store the data in the passed in array
 * @param filename: the filename to open and read
 * @param employeeArray: the array of structs to store the employee data
 * @return i the length of the database
 */
int readDatabase(char *filename, struct Employee employeeArray[]) {
	int ret = openFile(filename); //try to open the DB file
	//error check
	if(ret ==-1) { 
		printf("Cannot open file %s\n", filename);
		exit(1);
	}
	int id, salary;
	char fname[MAXNAME], lname[MAXNAME];
	int i =0;

	while (ret != -1) {    
		ret = readInt(&id);// read the first line of values from file
		if (ret) { break; }
		ret = readString(fname);
		if (ret) { break; }
		ret = readString(lname);
		if (ret) { break; }
		ret = readInt(&salary);
		if (ret == 0) { // stuff was read in okay
			
			employeeArray[i].employeeId = id;
			strcpy(employeeArray[i].firstName,fname);
			strcpy(employeeArray[i].lastName,lname);
			employeeArray[i].salary = salary;

		}
		i++;
	}
	closeFile();
	return i;
}

/*
 * This is a function to test the readfile library on a DB input file: you will
 * not really call this in your program, but use it as an example of how to use
 * the readfile library functions.
 * @param filename the filename to open and read
 */
void readAndPrintFile(char *filename) {
	printf("filename '%s'\n", filename);

	int ret = openFile(filename);  // try to open the DB file
	if (ret == -1){
		printf("bad error: can't open %s\n", filename);
		exit(1);
	}

	int id, salary;
	char fname[MAXNAME], lname[MAXNAME];

	while (ret != -1){    
		ret = readInt(&id);         // read the first line of values from file
		if (ret) { break; }
		ret = readString(fname);
		if (ret) { break; }
		ret = readString(lname);
		if (ret) { break; }
		ret = readInt(&salary);
		if (ret == 0) { // stuff was read in okay
			printf("%d %s %s %d\n", id, fname, lname, salary);

		}
	}

	closeFile();  // when all done close the file
}

/*
 *  DO NOT MODIFY THIS FUNCTION. It works "as is".
 *
 *  This function gets the filename passed in as a command line option
 *  and copies it into the filename parameter. It exits with an error 
 *  message if the command line is badly formed.
 *  @param filename the string to fill with the passed filename
 *  @param argc, argv the command line parameters from main 
 *               (number and strings array)
 */
void getFilenameFromCommandLine(char filename[], int argc, char *argv[]) {

	if (argc != 2) {
		printf("Usage: %s database_file\n", argv[0]);
		// exit function: quits the program immediately...some errors are not 
		// recoverable by the program, so exiting with an error message is 
		// reasonable error handling option in this case
		exit(1);   
	}
	if (strlen(argv[1]) >= MAXFILENAME) { 
		printf("Filename, %s, is too long, cp to shorter name and try again\n",
				filename);
		exit(1);
	}
	strcpy(filename, argv[1]);
}