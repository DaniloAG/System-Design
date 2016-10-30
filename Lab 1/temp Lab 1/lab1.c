/*
**	Name:			Danilo Gonzalez
**	Student ID:		28017253
**	Assignment:		Lab 1: Record Keeper
**	Date/Due Date:	April 11, 2016
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Creating the structure for the linked list.
struct Node{
	char name[100];
	char streetAddress[100];
	char phoneNumber[100];
	struct Node* next;
} LN;


// Global Variable in order to make the linked list available
// for every function without passing it as a parameter. 
struct Node* head = NULL;

// Takes in the file name (ex. hello.txt) and checks if it exists
// in the current directory, if not then it returns 404, which would
// print out an error and reprompt. 
int readFunction(char toOpen[]){
	FILE* toCheck;
	toCheck = fopen(toOpen, "r");
	if (toCheck == NULL){
		// printf("File provided does not exist.\n");
		return 404;
	}
	fclose(toCheck);
	return 1;
}

// Function that takes in the Node to insert and traverses
// the linked list to find the last node and inserts it there.
void addToEndLinkedList(struct Node* tempNode){
	if (head == NULL){
		head = tempNode;
	}
	else{
		struct Node* memoryNode = head;
		while (memoryNode->next != NULL){
			memoryNode = memoryNode->next;
		}
		memoryNode->next = tempNode;
	}
}

// Takes in the name of the file and assumes it already exists, since
// we already ran the check. It then adds each line to the end of the
// linked list until it reaches EOF.
void readMemory(char toOpen[]){
	FILE* toInput = fopen(toOpen, "r");
	char tempString[100];
	char* currentString = fgets(tempString, 100, toInput);		
	char* toPrint;
	const char tabChar[2] = "\t";

	while (currentString != NULL){
		tempString[strlen(tempString) - 1] = '\0';				// Set the last char to \0 instead of \n
		struct Node* tempNode = malloc(sizeof(LN));
	 	int nodeVariableTracker = 1;
	 	toPrint = strtok(currentString, tabChar);
	 	while (toPrint != NULL){
			if (nodeVariableTracker == 1){
				strcpy(tempNode->name, toPrint);
				nodeVariableTracker++;
			}
			else if (nodeVariableTracker == 2){
				strcpy(tempNode->streetAddress, toPrint);
				nodeVariableTracker++;
			}
			else{
				strcpy(tempNode->phoneNumber, toPrint);
				nodeVariableTracker = 1;
			}
	 		toPrint = strtok(NULL, tabChar);
	 	}
	 	tempNode->next = NULL;
		addToEndLinkedList(tempNode);
	 	currentString = fgets(tempString, 100, toInput);
	}
	fclose(toInput);
}

// char* str_concat(char* str1, char* str2, char* str3) {
// 	char* str;
// 	int first = 0, second = 0, third = 0;
// 	for (first; str1[first] != '\0'; first++) {
// 		str[first] = str1[first];
// 	}
// 	str[first++] = "\t";
// 	for (second; str2[second] != '\0'; second++) {
// 		str[first + second] = str2[second];
// 	}
// 	str[first + second] = "\t";
// 	second++;

// 	for (third; str3[third] != '\0'; third++){
// 		str[first + second + third] = str3[third];
// 	}
// 	str[first + second + third] = '\0';
	
// 	return str;
// }

void writeFunction(char toOpen[]){
	FILE* toWrite = fopen(toOpen, "w");
	struct Node* tempNode = head;
	const char* tabChar = "\t";
	while (tempNode != NULL){
		char* first = tempNode->name;
		char* second = tempNode->streetAddress;
		char* third = tempNode->phoneNumber;

		// char* toInsertChar;
		// toInsertChar = strcat(toInsertChar, first);
		// // printf("%s\n", first);
		// // printf("%s\n", toInsertChar);
		// toInsertChar = strcat(toInsertChar, tabChar);
		// //printf("%s\n", toInsertChar);
		// toInsertChar = strcat(toInsertChar, second);
		// //printf("%s\n", toInsertChar);
		// toInsertChar = strcat(toInsertChar, tabChar);
		// //printf("%s\n", toInsertChar);
		// toInsertChar = strcat(toInsertChar, third);
		// //printf("%s\n", toInsertChar);

		if (tempNode->next != NULL){
			// toInsertChar = strcat(toInsertChar, "\n");
			fprintf(toWrite, "%s\t%s\t%s\n", first, second, third);
		}
		else{
			fprintf(toWrite, "%s\t%s\t%s", first, second, third);
		}
		// fwrite(toInsertChar, sizeof(toInsertChar), sizeof(toInsertChar), toWrite);
		// fprintf(toWrite, "%s", toInsertChar);
		tempNode = tempNode->next;
	}
	fclose(toWrite);
}


void printFunction(){
	//printf("Within Print\n");
	struct Node* tempNode = head;
	int frontNumber = 1;
	while (tempNode != NULL){
		printf("%d %s\t%s\t%s", frontNumber, tempNode->name, tempNode->streetAddress, tempNode->phoneNumber);
		printf("\n");
		frontNumber++;
		tempNode = tempNode->next;
	}
	
}

void deleteFunction(char numberDeleted[]){
	struct Node* tempNode = head;
	int frontNumber = 1;
	int wantDeleted = atoi(numberDeleted);
	if (frontNumber == wantDeleted){
		head = head->next;
	}
	else{
		while (tempNode != NULL){
			frontNumber++;
			if (frontNumber == wantDeleted){
				struct Node* toDelete = tempNode->next;
				tempNode->next = tempNode->next->next;
				free(toDelete);
				break;
			}
			else{
				tempNode = tempNode->next;
			}
		}
	}
}

void freeLinkedList(){
	struct Node* tempNode = head;
	while (tempNode != NULL){
		struct Node* toDelete = tempNode;
		tempNode = tempNode->next;
		free(toDelete);
	}
	head = NULL;
}

void quitFunction(){
	freeLinkedList();
}


int commandsPrimary(char* command){
	if (strcmp(command, "print") == 0){
		//printf("IN HERE3\n %s", command);
		printFunction();
		return 1;
	}
	else if (strcmp(command, "quit") == 0){
		//printf("IN HERE5\n %s", command);
		quitFunction();
		return -1;
	}
	else{
		//printf("IN HERE6\n %s", command);
		printf("Not a valid command!\n");
		return 0;
	}

}

int commandsSecondary(char* command, char* secondary){
	//printf("%s\n", command);
	if (strcmp(command, "read") == 0){
		int toReturn = readFunction(secondary);
		//FILE* toRead = readFunction(secondary);
		//printf("IN HERE\n %s", command);
		//readMemory(secondary);
		return toReturn;
	}
	else if (strcmp(command, "write") == 0){
		//printf("IN HERE2\n %s", command);
		//writeFunction(secondary);
		return 2;
	}
	else if (strcmp(command, "delete") == 0){
		//printf("IN HERE4\n %s", command);
		//deleteFunction(secondary);
		return 3;
	}
	else if (strcmp(command, "print") == 0){
		//printf("IN HERE3\n %s", command);
		//printFunction();
		return 4;
	}
	else if (strcmp(command, "quit") == 0){
		//printf("IN HERE5\n %s", command);
		//quitFunction();
		return -1;
	}
	else{
		//printf("IN HERE6\n %s", command);
		//printf("Not a valid command!\n");
		return 0;
	}
}

void numberToFunction(int number, char* secondary){
	if (number == 1){
		readMemory(secondary);
	}
	else if (number == 2){
		writeFunction(secondary);
	}
	else if (number == 3){
		deleteFunction(secondary);
	}
	else if (number == 4){
		printFunction();
	}
	else if (number == 0){
		printf("Not a valid command!\n");
	}
	else if (number == 404){
		printf("The file you provided does not exist.\n");
	}
}

int main(){
	while (true){
		char tempString[100];
		char* commandVariable;
		char* secondaryVariable = "";
		char* tempPointer;
		int loopTerminator;

		printf("> ");
		fgets(tempString, 100, stdin);
		tempString[strlen(tempString) - 1] = '\0';
		//printf("%s\n", entireString);
		tempPointer = strtok(tempString, " ");
		commandVariable = tempPointer;
		if (tempPointer != NULL){
			//printf("%s", tempPointer);
			// if (tempPointer == NULL){
			// 	loopTerminator = commandsPrimary(commandVariable);
			// 	tempPointer = strtok(NULL, " ");
			// }
			tempPointer = strtok(NULL, " ");
			secondaryVariable = tempPointer;
		}
		loopTerminator = commandsSecondary(commandVariable, secondaryVariable);
		if (loopTerminator == -1){
			freeLinkedList();
			break;
		}
		else{
			numberToFunction(loopTerminator, secondaryVariable);
		}
	}
	return 0;
}
