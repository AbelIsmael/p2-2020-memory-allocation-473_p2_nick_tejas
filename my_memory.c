// Include files
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#define  N_OBJS_PER_SLAB  64

// Functional prototypes
void setup( int malloc_type, int mem_size, void* start_of_memory );
void *my_malloc( int size );
void my_free( void *ptr );

//enum for the binary tree's node
enum status
{
    SPLIT = 0,      //has children
    FREE = 1,       //doesnt have anything allocated
    OCCUPIED = 2,   //the space has mem allocated
};

//node implementation for the binary tree
struct node
{
    int status;
    struct node *left;
    struct node *right;
};

int glob_malloc_type;
int glob_mem_size;
void * glob_start_of_memory;
////////////////////////////////////////////////////////////////////////////
//
// Function     : setup
// Description  : initialize the memory allocation system
//
// Inputs       : malloc_type - the type of memory allocation method to be used [0..3] where
//                (0) Buddy System
//                (1) Slab Allocation

void setup( int malloc_type, int mem_size, void* start_of_memory )
{
	glob_malloc_type = malloc_type;
	glob_mem_size = mem_size;
	glob_start_of_memory = start_of_memory;
}


void *buddy(int size)
{

}

void *slab(int size)
{

}

////////////////////////////////////////////////////////////////////////////
//
// Function     : my_malloc
// Description  : allocates memory segment using specified allocation algorithm
//
// Inputs       : size - size in bytes of the memory to be allocated
// Outputs      : -1 - if request cannot be made with the maximum mem_size requirement

void *my_malloc( int size )
{
	if( glob_malloc_type == 0) {buddy(size);}
	else {slab(size);}

	//local variables
	void *allocated_address;
    
    	allocated_address = sbrk(size);
    	printf("%d",allocated_address);
    	if(*((int*)allocated_address) == -1){printf("%s", "allocation error when sbrk was used");}
}

////////////////////////////////////////////////////////////////////////////
//
// Function     : my_free
// Description  : deallocated the memory segment being passed by the pointer
//
// Inputs       : ptr - pointer to the memory segment to be free'd
// Outputs      :

void my_free( void *ptr )
{
	//free(mylifeaway)
}
