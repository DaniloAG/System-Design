/*
	Gonzalez, Danilo: 28017253
	Orozco, Jonathan: 66888405
	Almazan, Adrian: 19437291
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

unsigned short get_size(char *bp);
void allocate(char* bp, unsigned short size);
unsigned short is_allocated(char *bp);
void printstuff();
void insert(int size);
void free_block(int bn);
void blocklist();
void write_heap(int bn, char letter, int copies);
void print_heap(int bn, int copies);
int inputToCommandOne(char* first);
void inputToCommandTwice(char* first, char* second);
void inputToCommandThree(char* first, char* second, char* third);
void inputToCommandFourth(char* first, char* second, char* third, char* fourth);

char* heap;
char* max_heap;
int block_number = 1;
int blockCount = 1;
int memCounter = 0;
int totalNumber = 400;
int tempNumber;
int boolChecker = 0;


int globalCounter = 1;


unsigned short get_size(char *bp){
	unsigned short bp2 =  (unsigned short)(
		((unsigned char) bp[0]) <<8 | 
		((unsigned char)bp[1])
		);
	return bp2 >>7 ;

}
void allocate(char* bp, unsigned short size){
	size = (size << 7 )+ globalCounter++;
	bp[1] = size & 0xFF;
	bp[0]= (size>>8 ) & 0xFF;	 
}

void de_allocate(char *pointer) {
    pointer[1] = pointer[1] & ~0x01;
}

unsigned short is_allocated(char *bp){
		unsigned short bp2 =  (unsigned short)(
		((unsigned char) bp[0]) <<8 | 
		((unsigned char)bp[1])
		);
		return bp2 & 0x7f;
}



void insert(int size) {
	if (size > 398 || size <= 0){
		printf("%d is invalid!\n", size);
		return;
	}
	int tempCounter = memCounter + size + 2;
	//printf("temp counter: %d\n", tempCounter);
	if (tempCounter > 400){
		printf("Memory overloaded!\n");
		return;
	}
	char *bp = heap;

	while( ! ( (get_size(bp) == 0 && is_allocated(bp) ==0) || (get_size(bp) ==size &&  is_allocated(bp)==0))  ) {
		if (bp > max_heap){
			printf("Not enough space!\n");
			return;
		}

		if(size<get_size(bp)-2 && is_allocated(bp) ==0 ){
			int old_size = get_size(bp)-size-2;
			allocate(bp,size);
			printf("%d\n", blockCount++);
			tempNumber = size;
			
			bp += get_size(bp) + 2 ;
			allocate(bp, old_size);
			boolChecker = 1;
			free_block(--globalCounter);
			boolChecker = 0;
			return;
		}
		bp += get_size(bp) + 2;
	}

	if (bp >= max_heap){
		printf("Not enough space!\n");
		return;
	}
	allocate(bp,size);
	memCounter = memCounter + size + 2;
	// printf("totalNumber before: %d\n", totalNumber);
	if (totalNumber > 0){
		totalNumber = totalNumber - size - 2;
	}
	// printf("totalNumber after: %d\n", totalNumber);
	printf("%d\n", blockCount++);
}


void free_block(int bn){
	char *bp = heap;
	while( ! ( (get_size(bp) == 0 && is_allocated(bp) ==0) || (is_allocated(bp) == bn) || bp >= heap +400)  ) {
	
		bp += get_size(bp)+2;
	}
	
	if (is_allocated(bp) == bn){
		if (boolChecker == 0){
			memCounter = memCounter - get_size(bp) - 2;
		}
		else{
			memCounter = memCounter + tempNumber + 2; 
		}
		bp[1] = bp[1] & 0x80;
		return;
	}

	if ((get_size(bp) == 0 && is_allocated(bp) ==0) || bp >= heap +400) {
		printf("Not Found\n");
		return;
	}
}


void blocklist(){
	printf("Size\tAlloc\tStart\t\tEnd\n");

	char *current_index = heap;
	unsigned short current_size;
	while(current_index < max_heap && (get_size(current_index) != 0)){
		current_size = get_size(current_index);
		 printf("%d\t%s\t%p\t%p\n", get_size(current_index) + 2, is_allocated(current_index)? "yes":"no", current_index, current_index+get_size(current_index)+1);
		 current_index += current_size + 2;
	}
	// if (totalNumber != 0 && totalNumber > 0){
	// 	printf("%d\t%s\t%p\t%p\n", totalNumber, is_allocated(bp)? "yes":"no", bp, max_heap);
	// }
}



void write_heap(int bn, char letter, int copies){
	char *bp = heap;
	

	while( ! ( (get_size(bp) == 0 && is_allocated(bp) ==0) || (is_allocated(bp) == bn) || bp >= heap +400)  ) {
		bp += get_size(bp)+2;
	}

	if ((get_size(bp) == 0 && is_allocated(bp) ==0) || bp >= heap +400) {
		printf("Not Found\n");
		return;
	}
	if (copies > get_size(bp)){
		printf("WriteHeap: Not enough space to write.\n");
		return;
	}

	bp += 2;

	int i;
	for (i = 0; i < copies; i++, bp++){
		*bp = letter;
	}

}


void print_heap(int bn, int copies){
	char *bp = heap;
	

	while( ! ( (get_size(bp) == 0 && is_allocated(bp) ==0) || (is_allocated(bp) == bn) || bp >= heap +400)  ) {
		bp += get_size(bp)+2;
	}

	if ((get_size(bp) == 0 && is_allocated(bp) ==0) || bp >= heap +400) {
		printf("Not Found\n");
		return;
	}
	bp += 2;
	int i;
	for (i = 0; i < copies; i++, bp++){

		printf("%c",*bp);
	}
	printf("\n");

}

//cmd lne

int main(){
	heap = malloc(sizeof(char) * 400);
	max_heap = heap + 400;
	allocate(heap, 398);
	de_allocate(heap);
	char delimiters[] = "\t \n";
	while (1){
		char tempString[100];
		char* commandVariable;
		char* secondaryVariable = "";
		char* thirdVariable = "";
		char* fourthVariable = "";
		int numberCounter = 1;

		char* tempPointer;

		printf("> ");
		fgets(tempString, 100, stdin);
		if (strcmp(tempString, "\n") == 0){
			continue;
		}
		tempString[strlen(tempString) - 1] = '\0';
		tempPointer = strtok(tempString, delimiters);
		commandVariable = tempPointer;
		while (tempPointer != NULL){
			tempPointer = strtok(NULL, delimiters);
			if (tempPointer == NULL){
				break;
			}
			if (numberCounter == 1){
				secondaryVariable = tempPointer;
				numberCounter++;
			}
			else if (numberCounter == 2){
				thirdVariable = tempPointer;
				numberCounter++;
			}
			else if (numberCounter == 3){
				fourthVariable = tempPointer;
				numberCounter++;
			}
			else{
				numberCounter++;
			}
		}


		if (numberCounter == 1){
			int toQuit = inputToCommandOne(commandVariable);
			if (toQuit == 1){
                free(heap);
				break;
			}
			else if (toQuit == 0){
				blocklist();
			}
			else{
				printf("%s is not a valid command!\n", commandVariable);
			}
		}
		else if (numberCounter == 2 ){
			inputToCommandTwice(commandVariable, secondaryVariable);
		}
		else if (numberCounter == 3){
			inputToCommandThree(commandVariable, secondaryVariable, thirdVariable);
		}
		else if (numberCounter == 4){
			inputToCommandFourth(commandVariable, secondaryVariable, thirdVariable, fourthVariable);
		}
		else if (numberCounter == 5){
			printf("Not a valid command!\n");
			continue;
		}
		else{
			continue;
		}
	}
}

int inputToCommandOne(char* first){
	if (strcmp(first, "quit") == 0){
		return 1;
	}
	else if (strcmp(first, "blocklist") == 0){
		return 0;
	}
	else{
		return -1;
	}
}

void inputToCommandTwice(char* first, char* second){
	if (strcmp(first, "allocate") == 0){
		int number = atoi(second);
		insert(number);
	
	}
	else if (strcmp(first, "free") == 0){
        int number = atoi(second);
		free_block(number + 1);

	}
	else{
		printf("%s is not a valid command!\n", first);
	}
}

void inputToCommandThree(char* first, char* second, char* third){
	if (strcmp(first, "printheap") == 0){
        int number = atoi(second);
        int number_2 = atoi(third);
		print_heap(number + 1, number_2);
	}
	else{
		printf("%s is not a valid command!\n", first);
	}
}

void inputToCommandFourth(char* first, char* second, char* third, char* fourth){
	if (strcmp(first, "writeheap") == 0){
        int number = atoi(second);
        int number_2= atoi(fourth);
        char letterToUse = *third;
		write_heap(number + 1, letterToUse, number_2);
	}
	else{
		printf("%s is not a valid command!\n", first);
	}
}