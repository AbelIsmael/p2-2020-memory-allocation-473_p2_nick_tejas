// Include files
#include <stdio.h>
#include <stdlib.h>

#include <math.h> // addded for log function

#define  N_OBJS_PER_SLAB  64

// Functional prototypes
void setup( int malloc_type, int mem_size, void* start_of_memory );
void *my_malloc( int size );
void my_free( void *ptr );

int glob_malloc_type;
int glob_mem_size;
void * glob_start_of_memory;

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
	int offset;
    struct node *left;
    struct node *right;
};

struct node *buddy_tree;


/* newNode() allocates a new node with the given data and NULL left and  
right pointers. */
struct node* newNode(int memSize, int status, int offset, struct node *parent)//change parameters later 
{ 
	// Allocate memory for new node  
	struct node* node = (struct node*)malloc(sizeof(struct node)); 
  
	if(parent == NULL)
	{
		node->mem_size = glob_mem_size;
		node->status = FREE;
		node->offset = offset;

	}
	else
	{
		// Assign data to this node 
		node->mem_size = parent->mem_size/2; //more stuff here too
		node->status = status;
		node->offset = offset;
	}
  
	// Initialize left and right children as NULL 
	node->left = NULL; 
	node->right = NULL; 
	return(node); 
} 

////////////////////////////////////////////////////////////////////////////
//
// Function     : split
// Description  : splits the parent node's mem size into 2 child nodes
//
// Inputs       : malloc_type - the type of memory allocation method to be used [0..3] where
//                

int split(struct node *parent)	//only happens if the previous status was free
{
	if(parent->status != FREE)
	{
		return -1;
	}
	else if(parent->mem_size == 1024)
	{
		return 0;
	}
	else
	{
		parent->status = SPLIT;
		parent->left = newNode(parent->mem_size, FREE, parent->offset, parent);
		parent->right = newNode(parent->mem_size, FREE, (parent->offset)+(parent->mem_size/2), parent);
	}
}

////////////////////////////////////////////////////////////////////////////
//
// Function     : combine
// Description  : splits the parent node's mem size into 2 child nodes
//
// Inputs       : node type - parent node which has the SPLIT status
//                
// Outputs		: 0 if the combine was successfull
//				: 1 if the parent node does not have 2 free children 
//				: -1 if combine was called on a parent without a SPLIT status
int combine(struct node *parent)	//only happens if the previous status was free
{
	if(parent->status != SPLIT)
	{
		return -1;
	}
	else
	{
		if(parent->left->status == FREE && parent->right->status == FREE)
		{
			free(parent->left);
			free(parent->right);
			parent->status = FREE;
			return 0;
		}
		else
		{
			return 1;
		}
	}
}

/* Given a binary tree, print its nodes in inorder*/
void allocating_space(struct node* parent, int space, int *offset) 
{ 
	if (parent == NULL)			//we didnt find an appropriate space, offset is -1
	{
		*offset = -1;
		return;
	}
	else if (space == parent->mem_size) 		//space found, the offset is given
	{
        *offset = parent->offset;
		return; 
	}
	else
	{
		/* first recur on left child */
    	allocating_space(parent->left, space, offset); 
		if (*offset > 0)							//indicates that we have found an offset so no furter recursive calls are needed
		{
			/* now recur on right child */
    		allocating_space(parent->right, space, offset); 
		}
		return;
    	
	}   
} 
 
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
	buddy_tree = newNode(glob_mem_size, FREE, 0, NULL);
}


void *buddy(int size)
{
//	int *error =-1
	if (size > glob_mem_size) {return (void*)-1 ;}//returns negative 1
	
	// do log calculation to find what power of 2 its closest two
	//int power = (log(size))/ (log(2));
	//printf("POWER IS %d \n", power);
	int alocation_size = glob_mem_size;
	int space_found = 1;
	struct node *temp = buddy_tree;	//potential bug!!!!!!!
	while (size+4 < alocation_size)
	{
		alocation_size = alocation_size >> 1;	//finds the power of 2 that is smaller that size
	}

	alocation_size = alocation_size <<1;		//this is to find the smallest size that is of power of 2 and is still enough to hold all bytes
	
	// if the head of the tree is too big, split it into two
	// repeat until it has a space thats the right size for it
	while(space_found != 0)
	{
		//split until we have the right memblock
		if(temp->status == SPLIT)
		{
			
		}
		else if (temp->status == FREE && temp->mem_size != alocation_size)
		{
			split(temp);				
			buddy_tree = temp; 		//potential bug
			temp = temp->left;
		}


	}
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
