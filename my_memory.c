// Include files
#include <stdio.h>
#include <stdlib.h>

#include <math.h> // addded for log function

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
	int mem_size;
    int status;
	int *stack_pointer;
    struct node *left;
    struct node *right;
};

/* newNode() allocates a new node with the given data and NULL left and  
right pointers. */
struct node* newNode(int memSize, int status, int *stackPointer, struct node *parent)//change parameters later 
{ 
	// Allocate memory for new node  
	struct node* node = (struct node*)malloc(sizeof(struct node)); 
  
	// Assign data to this node 
	node->mem_size = parent->mem_size/2; //more stuff here too
	node->status = status;
	node->stack_pointer = stackPointer;
  
// Initialize left and right children as NULL 
node->left = NULL; 
node->right = NULL; 
return(node); 
} 

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
//	int *error =-1
	if (size > glob_mem_size) {return NULL;}//returns negative 1
	
	// do log calculation to find what power of 2 its closest two
	int power = (log(size))/ (log(2));
	printf("POWER IS %d \n", power);
	// if the head of the tree is too big, split it into two
	// repeat until it has a space thats the right size for it
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
        printf("%d",allocated_address);
 //   	allocated_address = sbrk(size);
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
