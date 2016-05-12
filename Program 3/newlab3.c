#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

unsigned int get_size(char *bp);
void allocate(char* bp, int size);
unsigned int is_allocated(char *bp);
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
int block_number = 1;
int blockCount = 0;
char* block_array[400];


unsigned int get_size(char *bp){
	unsigned char *bp2 =  (unsigned char*)bp;
	return *bp2/2;

}
void allocate(char* bp, int size){
	unsigned char *bp2 =  (unsigned char*)bp;
	*bp2 = (size<<1)+1;
}

unsigned int is_allocated(char *bp){
	unsigned char *bp2 =  (unsigned char*)bp;
	return *bp2%2;
}
void printstuff() {
	int i;
	for(i = 0; i < 10; i++) {
		printf("%d. [%d]\n", i, heap[i]);
	}
}


//insert
void insert(int size) {
	if (size > 127){
		printf("%d is invalid!\n", size);
		return;
	}
	// copy pointer
	char *bp = heap;
	// copy head pointer

	// when do i want to stop
	// get_size = 0 && is_allocated == 0
	// OR
	// get_size = size && is_allocated == 0
	while(!((get_size(bp) == 0) && is_allocated(bp) ==0) || (get_size(bp) ==size &&  is_allocated(bp)==0)  ) {
		// tmp_size = get_size(bp)
		// bp = bp + 1 + tmp_size
		// check end, end = heap+400; if bp > end
		bp += get_size(bp)+1;
	}
	allocate(bp,size);
	block_array[block_number++]= bp;
	printf("%d\n", ++blockCount);
}
//free

void free_block(int bn){
	// for (int i = 1; i < 5;i++){
	// 	printf("%p\n", block_array[i]);
	// }
	unsigned char *delete_bp =  (unsigned char*)block_array[bn];
	*delete_bp = *delete_bp-1;
	block_array[bn] = 0;

}

//bl 
void blocklist(){
	char *bp = heap;
	printf("Size\tAlloc\tStart\t\tEnd\n");
	while(!((get_size(bp) == 0) && is_allocated(bp) ==0)){

		 printf("%d\t%s\t%p\t%p\n", get_size(bp), is_allocated(bp)? "yes":"no", bp, bp+get_size(bp));
		 bp += get_size(bp)+1;
	}

}

//wh

void write_heap(int bn, char letter, int copies){
	char *bp = block_array[bn];
	bp+=1;
	int i;
	for (i = 0; i < copies; i++, bp++){
		*bp = letter;
	}

}

//ph
void print_heap(int bn, int copies){
	char *bp = block_array[bn];
	bp+=1;
	int i;
	for (i = 0; i < copies; i++, bp++){
		printf("%c",*bp);
	}
	printf("\n");

}

//cmd lne

int main(){
	heap = malloc(sizeof(char) * 400);
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
		else if (numberCounter == 2){
			inputToCommandTwice(commandVariable, secondaryVariable);
		}
		else if (numberCounter == 3){
			inputToCommandThree(commandVariable, secondaryVariable, thirdVariable);
		}
		else if (numberCounter == 4){
			inputToCommandFourth(commandVariable, secondaryVariable, thirdVariable, fourthVariable);
		}
		else{
			continue;
		}
	}
}

// int main(){
// 	heap = malloc(sizeof(char) * 400);

// 	char * block_pointer = heap;
// 	insert(2);
// 	insert(2);
// 	//allocate(block_pointer, 5);
// 	printstuff();
// 	write_heap(1,'a',2);
// 	printf("%d\n", get_size(heap));
// 	printf("%d\n", is_allocated(heap));
// 	print_heap(1,2);
// 	// blocklist();
// 	// blocklist();
// 	// printstuff();
// 	free(heap);
// }

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
		free_block(number);

	}
	else{
		printf("%s is not a valid command!\n", first);
	}
}

void inputToCommandThree(char* first, char* second, char* third){
	if (strcmp(first, "printheap") == 0){
        int number = atoi(second);
        int number_2 = atoi(third);
		print_heap(number, number_2);
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
		write_heap(number, letterToUse, number_2);
	}
	else{
		printf("%s is not a valid command!\n", first);
	}
}