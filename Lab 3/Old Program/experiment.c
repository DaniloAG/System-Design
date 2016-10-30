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
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
             

int mm_init(void);
void *mm_malloc (size_t size);
void mm_free (void *ptr);
void mem_init(void);
void *mem_sbrk(int incr);

static char *mem_heap;     /* Points to first byte of heap */ 
static char *mem_brk;      /* Points to last byte of heap plus 1 */
static char *mem_max_addr; /* Max legal heap addr plus 1*/ 


/* ----------------------------------------------------------------------- */

void blocklistFunction();
void *allocateFunction(size_t size);
void freeFunction();
void printheapFunction();
void writeheapFunction();
int inputToCommandOne(char* first);
void inputToCommandTwice(char* first, char* second);
void inputToCommandThree(char* first, char* second, char* third);
void inputToCommandFourth(char* first, char* second, char* third, char* fourth);


static char *block_heap[400];
static int block_num = 0;

int main(){
    mem_init();
    // mm_init();
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
        if (block_heap[1]== NULL)
            printf("test");
    }
}

void blocklistFunction(){
    printf("blocklist!\n");
}

void *allocateFunction(size_t size){
    printf("allocate!\n");
    void *block_ptr;
    block_ptr = mm_malloc(size);

    printf("%d\n", ++block_num);
    return block_ptr;

}

void freeFunction(){
    printf("free!\n");
}

void printheapFunction(){
    printf("printheap!\n");
}

void writeheapFunction(){
    printf("writeheap!\n");
}

int inputToCommandOne(char* first){
    printf("%s\n", first);
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
    printf("%s %s\n", first, second);
    if (strcmp(first, "allocate") == 0){
        int number = atoi(second);
        block_heap[block_num]= allocateFunction(number);
        printf("%x %x",HDRP(block_heap[1]) ,FTRP(block_heap[1]));


    }
    else if (strcmp(first, "free") == 0){
        freeFunction();
    }
    else{
        printf("%s is not a valid command!\n", first);
    }
}

void inputToCommandThree(char* first, char* second, char* third){
    printf("%s %s %s\n", first, second, third);
    if (strcmp(first, "printheap") == 0){
        printheapFunction();
    }
    else{
        printf("%s is not a valid command!\n", first);
    }
}

void inputToCommandFourth(char* first, char* second, char* third, char* fourth){
    printf("%s %s %s %s\n", first, second, third, fourth);
    if (strcmp(first, "writeheap") == 0){
        writeheapFunction();
    }
    else{
        printf("%s is not a valid command!\n", first);
    }
}



/* ------------------------ FUNCTIONS FROM THE BOOK -------------------------- */

int mm_init(void) 
{
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
}

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

void mem_init()
{
    mem_heap = (char *)malloc(MAX_HEAP);
    mem_brk = (char *)mem_heap;               
    mem_max_addr = (char *)(mem_heap + MAX_HEAP); 
}

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
