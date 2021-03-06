#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>


/* ----------------------------------- HEADERS FROM BOOK -----------------------------------*/

#define NEXT_FITx

/* $begin mallocmacros */
/* Basic constants and macros */
#define WSIZE       4       /* Word and header/footer size (bytes) */ //line:vm:mm:beginconst
#define DSIZE       8       /* Double word size (bytes) */
#define CHUNKSIZE  (1<<12)  /* Extend heap by this amount (bytes) */  //line:vm:mm:endconst 

#define MAX(x, y) ((x) > (y)? (x) : (y))  

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc)) //line:vm:mm:pack

/* Read and write a word at address p */
#define GET(p)       (*(unsigned int *)(p))            //line:vm:mm:get
#define PUT(p, val)  (*(unsigned int *)(p) = (val))    //line:vm:mm:put

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)                   //line:vm:mm:getsize
#define GET_ALLOC(p) (GET(p) & 0x1)                    //line:vm:mm:getalloc

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)       ((char *)(bp) - WSIZE)                      //line:vm:mm:hdrp
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) //line:vm:mm:ftrp

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) //line:vm:mm:nextblkp
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) //line:vm:mm:prevblkp
/* $end mallocmacros */

#define MAX_HEAP (20*(1<<20))

/* Global variables */
static char *heap_listp = 0;  /* Pointer to first block */  
#ifdef NEXT_FIT
static char *rover;           /* Next fit rover */
#endif

/* Function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);
static void printblock(void *bp); 
static void checkheap(int verbose);
static void checkblock(void *bp);
             
void *mem_sbrk(int incr);

void mem_deinit(void);
void mem_reset_brk(void); 
void *mem_heap_lo(void);
void *mem_heap_hi(void);
size_t mem_heapsize(void);
size_t mem_pagesize(void);

int mm_init(void);
void *mm_malloc (size_t size);
void mm_free (void *ptr);
void mem_init(void);
/* $end mallocinterface */

void *mm_realloc(void *ptr, size_t size);
void *mm_calloc (size_t nmemb, size_t size);
void mm_checkheap(int verbose);

static char *mem_heap;     /* Points to first byte of heap */ 
static char *mem_brk;      /* Points to last byte of heap plus 1 */
static char *mem_max_addr; /* Max legal heap addr plus 1*/ 


/* ----------------------------------------------------------------------- */

void blocklistFunction();
void *allocateFunction(size_t size, int);
void freeFunction(int num);
void printheapFunction(int block_number, int bytes);
void writeheapFunction(int block_number, char* letter, int number);
int inputToCommandOne(char* first);
void inputToCommandTwice(char* first, char* second);
void inputToCommandThree(char* first, char* second, char* third);
void inputToCommandFourth(char* first, char* second, char* third, char* fourth);


static char *block_heap[100];
static int number_counter[100];
static int numberCounterGlobal = 0;
static int block_num = 0;

int main(){
    mem_init();
    mm_init();
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
                free(mem_heap);
				break;
			}
			else if (toQuit == 0){
				blocklistFunction();
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


/* ----------------------------------- HELPER FUNCTIONS WE IMPLEMENTED -----------------------------------*/

void blocklistFunction(){
    if (block_heap[0] == NULL){
        return;
    }
    int i = 0;
    int counter = 0;
    void* bp;
    printf("Size\tAlloc\tStart\tEnd\n");
    for (bp = block_heap[i]; i < block_num;  bp = NEXT_BLKP(bp)){
        if (GET_ALLOC(HDRP(block_heap[i]))==1){
            printf("%d\tyes\t0x%x\t0x%x\n", number_counter[counter], HDRP(block_heap[i]) ,FTRP(block_heap[i]) + 3);
        }else{
            printf("%d\tno\t0x%x\t0x%x\n", number_counter[counter], HDRP(block_heap[i]) ,FTRP(block_heap[i]) + 3);
           
        // if (GET_ALLOC(HDRP(block_heap[i]))==1){
        //     printf("%d\tyes\t0x%x\t0x%x\n", GET_SIZE(HDRP(block_heap[i])), HDRP(block_heap[i]) ,FTRP(block_heap[i]) + 4);
        // }else{
        //     printf("%d\tno\t0x%x\t0x%x\n", GET_SIZE(HDRP(block_heap[i])), HDRP(block_heap[i]) ,FTRP(block_heap[i]) + 4);
           
        }
        counter++;
        i++;
    }
	
}

void *allocateFunction(size_t size, int block_number){
	// printf("allocate!\n");
    void *block_ptr;
    block_ptr = mm_malloc(size);

    printf("%d\n", block_number + 1);
    number_counter[numberCounterGlobal] = size;
    numberCounterGlobal++;
    return block_ptr;

}

void freeFunction(int num){
	mm_free(block_heap[num - 1]);
}

void printheapFunction(int block_number, int bytes){
	void *bp = block_heap[block_number - 1];
	int i;
    for (i = 0; i < bytes; i++){
        printf("%c", GET(bp));
        bp++;
    }
    printf("\n");
}

void writeheapFunction(int block_number, char* letter, int number){
    // printf("writing %s\n ",letter);
    char letterToUse = *letter;
    void *bp = block_heap[block_number - 1];
    int i;
    for (i = 0; i< number;  i++){
        PUT(bp, letterToUse);
        bp++;
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
		block_heap[block_num]= allocateFunction(number, block_num);
		block_num++;
	}
	else if (strcmp(first, "free") == 0){
        int number = atoi(second);
		freeFunction(number);

	}
	else{
		printf("%s is not a valid command!\n", first);
	}
}

void inputToCommandThree(char* first, char* second, char* third){
	if (strcmp(first, "printheap") == 0){
        int number = atoi(second);
        int number_2 = atoi(third);
		printheapFunction(number, number_2);
	}
	else{
		printf("%s is not a valid command!\n", first);
	}
}

void inputToCommandFourth(char* first, char* second, char* third, char* fourth){
	if (strcmp(first, "writeheap") == 0){
        int number = atoi(second);
        int number_2= atoi(fourth);
		writeheapFunction(number, third, number_2);
	}
	else{
		printf("%s is not a valid command!\n", first);
	}
}

/* -----------------------------------------------------------------------------------------*/

/* ----------------------------------- FUNCTIONS FROM BOOK ---------------------------------*/

int mm_init(void) 
{
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1) //line:vm:mm:begininit
        return -1;
    PUT(heap_listp, 0);                          /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); /* Prologue header */ 
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); /* Prologue footer */ 
    PUT(heap_listp + (3*WSIZE), PACK(0, 1));     /* Epilogue header */
    heap_listp += (2*WSIZE);                     //line:vm:mm:endinit  
    /* $end mminit */

#ifdef NEXT_FIT
    rover = heap_listp;
#endif
    /* $begin mminit */

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) 
        return -1;
    return 0;
}
/* $end mminit */

/* 
 * mm_malloc - Allocate a block with at least size bytes of payload 
 */
/* $begin mmmalloc */
void *mm_malloc(size_t size) 
{
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;      

    /* $end mmmalloc */
    if (heap_listp == 0){
        mm_init();
    }
    /* $begin mmmalloc */
    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)                                          //line:vm:mm:sizeadjust1
        asize = 2*DSIZE;                                        //line:vm:mm:sizeadjust2
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE); //line:vm:mm:sizeadjust3

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {  //line:vm:mm:findfitcall
        place(bp, asize);                  //line:vm:mm:findfitplace
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize,CHUNKSIZE);                 //line:vm:mm:growheap1
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)  
        return NULL;                                  //line:vm:mm:growheap2
    place(bp, asize);                                 //line:vm:mm:growheap3
    return bp;
} 
/* $end mmmalloc */

/* 
 * mm_free - Free a block 
 */
/* $begin mmfree */
void mm_free(void *bp)
{
    /* $end mmfree */
    if (bp == 0) 
        return;

    /* $begin mmfree */
    size_t size = GET_SIZE(HDRP(bp));
    /* $end mmfree */
    if (heap_listp == 0){
        mm_init();
    }
    /* $begin mmfree */

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

/* $end mmfree */
/*
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 */
/* $begin mmfree */
static void *coalesce(void *bp) 
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {            /* Case 1 */
        return bp;
    }

    else if (prev_alloc && !next_alloc) {      /* Case 2 */
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size,0));
    }

    else if (!prev_alloc && next_alloc) {      /* Case 3 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    else {                                     /* Case 4 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + 
            GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    /* $end mmfree */
#ifdef NEXT_FIT
    /* Make sure the rover isn't pointing into the free block */
    /* that we just coalesced */
    if ((rover > (char *)bp) && (rover < NEXT_BLKP(bp))) 
        rover = bp;
#endif
    /* $begin mmfree */
    return bp;
}
/* $end mmfree */

/*
 * mm_realloc - Naive implementation of realloc
 */
void *mm_realloc(void *ptr, size_t size)
{
    size_t oldsize;
    void *newptr;

    /* If size == 0 then this is just free, and we return NULL. */
    if(size == 0) {
        mm_free(ptr);
        return 0;
    }

    /* If oldptr is NULL, then this is just malloc. */
    if(ptr == NULL) {
        return mm_malloc(size);
    }

    newptr = mm_malloc(size);

    /* If realloc() fails the original block is left untouched  */
    if(!newptr) {
        return 0;
    }

    /* Copy the old data. */
    oldsize = GET_SIZE(HDRP(ptr));
    if(size < oldsize) oldsize = size;
    memcpy(newptr, ptr, oldsize);

    /* Free the old block. */
    mm_free(ptr);

    return newptr;
}

/* 
 * mm_checkheap - Check the heap for correctness
 */
void mm_checkheap(int verbose)  
{ 
    checkheap(verbose);
}

/* 
 * The remaining routines are internal helper routines 
 */

/* 
 * extend_heap - Extend heap with free block and return its block pointer
 */
/* $begin mmextendheap */
static void *extend_heap(size_t words) 
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE; //line:vm:mm:beginextend
    if ((long)(bp = mem_sbrk(size)) == -1)  
        return NULL;                                        //line:vm:mm:endextend

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* Free block header */   //line:vm:mm:freeblockhdr
    PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */   //line:vm:mm:freeblockftr
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */ //line:vm:mm:newepihdr

    /* Coalesce if the previous block was free */
    return coalesce(bp);                                          //line:vm:mm:returnblock
}
/* $end mmextendheap */

/* 
 * place - Place block of asize bytes at start of free block bp 
 *         and split if remainder would be at least minimum block size
 */
/* $begin mmplace */
/* $begin mmplace-proto */
static void place(void *bp, size_t asize)
/* $end mmplace-proto */
{
    size_t csize = GET_SIZE(HDRP(bp));   

    if ((csize - asize) >= (2*DSIZE)) { 
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize-asize, 0));
        PUT(FTRP(bp), PACK(csize-asize, 0));
    }
    else { 
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}
/* $end mmplace */

/* 
 * find_fit - Find a fit for a block with asize bytes 
 */
/* $begin mmfirstfit */
/* $begin mmfirstfit-proto */
static void *find_fit(size_t asize)
/* $end mmfirstfit-proto */
{
    /* $end mmfirstfit */

#ifdef NEXT_FIT 
    /* Next fit search */
    char *oldrover = rover;

    /* Search from the rover to the end of list */
    for ( ; GET_SIZE(HDRP(rover)) > 0; rover = NEXT_BLKP(rover))
        if (!GET_ALLOC(HDRP(rover)) && (asize <= GET_SIZE(HDRP(rover))))
            return rover;

    /* search from start of list to old rover */
    for (rover = heap_listp; rover < oldrover; rover = NEXT_BLKP(rover))
        if (!GET_ALLOC(HDRP(rover)) && (asize <= GET_SIZE(HDRP(rover))))
            return rover;

    return NULL;  /* no fit found */
#else 
    /* $begin mmfirstfit */
    /* First-fit search */
    void *bp;

    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) {
            return bp;
        }
    }
    return NULL; /* No fit */
#endif
}

static void printblock(void *bp) 
{
    size_t hsize, halloc, fsize, falloc;

    checkheap(0);
    hsize = GET_SIZE(HDRP(bp));
    halloc = GET_ALLOC(HDRP(bp));  
    fsize = GET_SIZE(FTRP(bp));
    falloc = GET_ALLOC(FTRP(bp));  

    if (hsize == 0) {
        printf("%p: EOL\n", bp);
        return;
    }

    printf("%p: header: [%ld:%c] footer: [%ld:%c]\n", bp, 
           hsize, (halloc ? 'a' : 'f'), 
           fsize, (falloc ? 'a' : 'f')); 
}

static void checkblock(void *bp) 
{
    if ((size_t)bp % 8)
        printf("Error: %p is not doubleword aligned\n", bp);
    if (GET(HDRP(bp)) != GET(FTRP(bp)))
        printf("Error: header does not match footer\n");
}

/* 
 * checkheap - Minimal check of the heap for consistency 
 */
void checkheap(int verbose) 
{
    char *bp = heap_listp;

    if (verbose)
        printf("Heap (%p):\n", heap_listp);

    if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp)))
        printf("Bad prologue header\n");
    checkblock(heap_listp);

    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (verbose) 
            printblock(bp);
        checkblock(bp);
    }

    if (verbose)
        printblock(bp);
    if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp))))
        printf("Bad epilogue header\n");
}

/* 
 * mem_init - Initialize the memory system model
 */
void mem_init()
{
    mem_heap = (char *)malloc(MAX_HEAP);
    mem_brk = (char *)mem_heap;               
    mem_max_addr = (char *)(mem_heap + MAX_HEAP); 
}

/* 
 * mem_sbrk - Simple model of the sbrk function. Extends the heap 
 *    by incr bytes and returns the start address of the new area. In
 *    this model, the heap cannot be shrunk.
 */
void *mem_sbrk(int incr) 
{
    char *old_brk = mem_brk;

    if ( (incr < 0) || ((mem_brk + incr) > mem_max_addr)) {
	errno = ENOMEM;
	fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory...\n");
	return (void *)-1;
    }
    mem_brk += incr;
    return (void *)old_brk;
}
/* $end memlib */

/* 
 * mem_deinit - free the storage used by the memory system model
 */
void mem_deinit(void)
{
}

/*
 * mem_reset_brk - reset the simulated brk pointer to make an empty heap
 */
void mem_reset_brk()
{
    mem_brk = (char *)mem_heap;
}

/*
 * mem_heap_lo - return address of the first heap byte
 */
void *mem_heap_lo()
{
    return (void *)mem_heap;
}

/* 
 * mem_heap_hi - return address of last heap byte
 */
void *mem_heap_hi()
{
    return (void *)(mem_brk - 1);
}

/*
 * mem_heapsize() - returns the heap size in bytes
 */
size_t mem_heapsize() 
{
    return (size_t)((void *)mem_brk - (void *)mem_heap);
}
