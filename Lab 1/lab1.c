/*
**	Name:			Danilo Gonzalez
**	Student ID:		28017253
**	Assignment:		Lab 1: Record Keeper
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct Node{
	char* name;
	char* address;
	char* phone;
	struct Node* next;
} LN;

struct Node* head = NULL;

void readFunction(char* toOpen);
void writeFunction(char* toOpen);
void printFunction();
void deleteFunction(char* numberDeleted);
void quitFunction();
int readChecker(char* toOpen);
void addToLinkedList(struct Node* toInsertNodeLN);
void freeLinkedList();
int commandsPrimary(char* command);
int commandsSecondary(char* command, char* secondary);
void numberToFunction(int number, char* secondary);


int main(){
	char delimiters[] = "\t ";
	while (1){
		char tempString[100];
		char* commandVariable;
		char* secondaryVariable = "";
		char* tempPointer;
		int loopTerminator;

		//printf("> ");
		fgets(tempString, 100, stdin);
		if (strcmp(tempString, "\n") == 0){
			continue;
		}
		tempString[strlen(tempString) - 1] = '\0';
		tempPointer = strtok(tempString, delimiters);
		commandVariable = tempPointer;
		if (tempPointer != NULL){
			tempPointer = strtok(NULL, delimiters);
			secondaryVariable = tempPointer;
		}
		loopTerminator = commandsSecondary(commandVariable, secondaryVariable);
		if (loopTerminator == -1){
			quitFunction();
			break;
		}
		else{
			numberToFunction(loopTerminator, secondaryVariable);
		}
	}
	return 0;
}

// -------------------- Command Functions --------------------

void readFunction(char* toOpen){
	FILE* toInsert = fopen(toOpen, "r");
	char toPrint[100];
	const char tabChar[2] = "\t";
	head = NULL;
	while (fgets(toPrint, 100, toInsert) != NULL){
		if (toPrint[strlen(toPrint) - 1] == '\n'){
			toPrint[strlen(toPrint) - 1] = '\0';
		}
		char* toInsertNode = strtok(toPrint, tabChar);
		int nodeInsertTracker = 1;
		struct Node* tempNode = malloc(sizeof(LN));
		while (toInsertNode != NULL){
			if (nodeInsertTracker == 1){
				tempNode->name = malloc(strlen(toInsertNode + 1));
				strcpy(tempNode->name, toInsertNode);
				nodeInsertTracker++;
			}
			else if (nodeInsertTracker == 2){
				tempNode->address = malloc(strlen(toInsertNode + 1));
				strcpy(tempNode->address, toInsertNode);
				nodeInsertTracker++;
			}
			else if (nodeInsertTracker == 3){
				tempNode->phone = malloc(strlen(toInsertNode + 1));
				strcpy(tempNode->phone, toInsertNode);
				nodeInsertTracker = 1;
			}
			toInsertNode = strtok(NULL, tabChar);
		}
		tempNode->next = NULL;
		addToLinkedList(tempNode);
	}
	fclose(toInsert);
}

void writeFunction(char* toOpen){
	FILE* toWrite = fopen(toOpen, "w");
	struct Node* tempNode = head;
	const char* tabChar = "\t";
	while (tempNode != NULL){
		char* first = tempNode->name;
		char* second = tempNode->address;
		char* third = tempNode->phone;
		fprintf(toWrite, "%s\t%s\t%s\n", first, second, third);
		// if (tempNode->next != NULL){
		// 	fprintf(toWrite, "%s\t%s\t%s\n", first, second, third);
		// }
		// else{
		// 	fprintf(toWrite, "%s\t%s\t%s", first, second, third);
		// }
		tempNode = tempNode->next;
	}
	fclose(toWrite);
}

void printFunction(){
	struct Node* tempNode = head;
	int frontNumber = 1;
	while (tempNode != NULL){
		printf("%d\t%s\t%s\t%s\n", frontNumber, tempNode->name, tempNode->address, tempNode->phone);
		frontNumber++;
		tempNode = tempNode->next;
	}
}

void deleteFunction(char* numberDeleted){
	struct Node* tempNode = head;
	int frontNumber = 1;
	int wantDeleted = atoi(numberDeleted);
	if (head != NULL){
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
	else{
		printf("No values in memory!\n");
	}
}

void quitFunction(){
	freeLinkedList();
}

//	-------------------- HELPER FUNCTIONS --------------------


int readChecker(char* toOpen){
	FILE* toCheck;
	toCheck = fopen(toOpen, "r");
	if (toCheck == NULL){
		return 404;
	}
	fclose(toCheck);
	return 1;
}

void addToLinkedList(struct Node* toInsertNodeLN){
	if (head == NULL){
		head = toInsertNodeLN;
	}
	else{
		struct Node* tempNode = head;
		while (tempNode->next != NULL){
			tempNode = tempNode->next;
		}
		tempNode->next = toInsertNodeLN;
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

int commandsPrimary(char* command){
	if (strcmp(command, "print") == 0){
		printFunction();
		return 1;
	}
	else if (strcmp(command, "quit") == 0){
		quitFunction();
		return -1;
	}
	else{
		printf("Not a valid command!\n");
		return 0;
	}

}

int commandsSecondary(char* command, char* secondary){
	if (strcmp(command, "read") == 0){
		int toReturn = readChecker(secondary);
		return toReturn;
	}
	else if (strcmp(command, "write") == 0){
		return 2;
	}
	else if (strcmp(command, "delete") == 0){
		return 3;
	}
	else if (strcmp(command, "print") == 0){
		return 4;
	}
	else if (strcmp(command, "quit") == 0){
		return -1;
	}
	else{
		return 0;
	}
}

void numberToFunction(int number, char* secondary){
	if (number == 1){
		readFunction(secondary);
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