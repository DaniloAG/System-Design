#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void addFunction(char* first, char* second){
	int number = atoi(first);
	int secondNumber = atoi(second);
	printf("%d\n", number + secondNumber);
}


int main(int argc, char* argv[]){
	addFunction(argv[1], argv[2]);
	// std::cout << addFunction(argv[0], argv[1]) << std::endl;
	return 0;
}