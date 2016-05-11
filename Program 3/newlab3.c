#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char* heap;
int block_number = 1;
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
	for(int i = 0; i < 10; i++) {
		printf("%d. [%d]\n", i, heap[i]);
	}
}


//insert
void insert(int size) {
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
}
//free

void free_block(int bn){
	for (int i = 1; i < 5;i++){
		printf("%p\n", block_array[i]);
	}
	unsigned char *delete_bp =  (unsigned char*)block_array[bn];
	*delete_bp = *delete_bp-1;
	block_array[bn] = 0;

}

//bl 
void blocklist(){
	char *bp = heap;
	printf("Size\tAlloc\tStart\tEnd\n");
	while(!((get_size(bp) == 0) && is_allocated(bp) ==0)){

		 printf("%d\t%s\t%p\t%p\n", get_size(bp)+1, is_allocated(bp)? "yes":"no", bp, bp+get_size(bp));
		 bp += get_size(bp)+1;
	}

}

//wh

void write_heap(int bn, char letter, int copies){
	char *bp = block_array[bn];
	bp+=1;
	for (int i = 0; i < copies; i++, bp++){
		*bp = letter;
	}

}

//ph
void print_heap(int bn, int copies){
	char *bp = block_array[bn];
	bp+=1;
	for (int i = 0; i < copies; i++, bp++){
		printf("%c",*bp);
	}
	printf("\n");

}

//cmd lne

int main(){
	heap = malloc(sizeof(char) * 400);

	char * block_pointer = heap;
	insert(2);
	insert(2);
	//allocate(block_pointer, 5);
	printstuff();
	write_heap(1,'a',2);
	printf("%d\n", get_size(heap));
	printf("%d\n", is_allocated(heap));
	print_heap(1,2);
	// blocklist();
	// blocklist();
	// printstuff();


}