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

enum slab_status
{
	PARTIAL = 0,
	EMPTY = 1,
	FULL = 2,
};

//struct for slab 
struct slab
{
	int type;								//type means what mem size will be held in this slab for example: 600kb, 500kb
	int offset;								//tracks the slab's offset
	int status;								//tells us if the slab is filled, free, or partial
	int max_object; 						//how many types this slab can hold
	int used;								//the number of object it is currently using
	int size;								//total size of the slab
	int slab_pointer[N_OBJS_PER_SLAB]; 		//the binary array that tells us which object is containted where
	struct slab *next;
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
struct slab *slab_descripter;			//potential bug!!!


////////////////////////////////////////////////////////////////////////////
//
// Function     : split
// Description  : splits the parent node's mem size into 2 child nodes
//
// Inputs       : malloc_type - the type of memory allocation method to be used [0..3] where
//                

struct slab* newSlab(int typeSize)
{
	// Allocate memory for new node  
	struct slab* slab = (struct slab*)malloc(sizeof(struct slab)); 
  
	slab->type = typeSize;
	slab->status = FREE;
	slab->max_object = N_OBJS_PER_SLAB;
	slab->used = 0;
	slab->size = (typeSize+4)*N_OBJS_PER_SLAB + 4;
	slab->offset += 8;
	for(int i = 0; i < N_OBJS_PER_SLAB; i += 1)
	{
		slab->slab_pointer[i] = 0;
	}
	
	slab->next = NULL;
	return(slab); 
}

////////////////////////////////////////////////////////////////////////////
//
// Function     : newNode
// Description  :  allocates a new node with the given data and NULL left and right pointers.
//
// Inputs       : mem
//                

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
			
			parent->left=NULL;
			parent->right=NULL;
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
////////////////////////////////////////////////////////////////////////////
//
// Function     : setup
// Description  : initialize the memory allocation system
//
// Inputs       : malloc_type - the type of memory allocation method to be used [0..3] where
//                (0) Buddy System
//                (1) Slab Allocation

void allocating_split(struct node* parent, int space, int *offset) 
{ 
	if (parent == NULL)			//we didnt find an appropriate space, offset is -1
	{
		*offset = -1;
		return;
	}
	else if (space == parent->mem_size && parent->status == FREE ) 		//space found, the offset is given
	{
       	*offset = parent->offset;
		parent->status = OCCUPIED;
		return; 
	}
	else if (parent->status == FREE && parent->mem_size > 1024)			//ensures we dont split under 1Kb 
	{
		split(parent);

		/* first recur on left child */
    	allocating_split(parent->left,space, offset); 
		//if (*offset > 0)							//indicates that we have found an offset so no furter recursive calls are needed
		//{
			/* now recur on right child */
		//	printf("SHOULD NOT HAVE GOTTEN IN HERE");
    		//allocating_split(parent->right,space, offset); 
		//}
		return;
	}
	else if(parent->status == OCCUPIED)
	{
		return;
	}
	else
	{
		/* first recur on left child */
    	allocating_split(parent->left, space, offset); 
		if (*offset <0)							//indicates that we have found an offset so no furter recursive calls are needed
		{
			/* now recur on right child */
    		allocating_split(parent->right, space, offset); 
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

void allocation_search(struct node* parent, int space, int *offset) 
{ 
	if (parent == NULL)			//we didnt find an appropriate space, offset is -1
	{
		*offset = -1;
		return;
	}
	else if (space == parent->mem_size && parent->status == FREE ) 		//space found, the offset is given
	{
       	*offset = parent->offset;
		parent->status = OCCUPIED;
		return; 
	}
	else if(parent->status == OCCUPIED)
	{
		return;
	}
	else
	{
		/* first recur on left child */
    		allocation_search(parent->left, space, offset); 
		if (*offset < 0)							//indicates that we have found an offset so no furter recursive calls are needed
		{
			/* now recur on right child */
    		allocation_search(parent->right, space, offset); 
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
	//local variable
	//int power = (log(size))/ (log(2));
	//printf("POWER IS %d \n", power);
	int allocation_size = glob_mem_size;
	int offset =-1;

	while (size+4 < allocation_size && allocation_size > 512 )
	{
		allocation_size = allocation_size >> 1;	//finds the power of 2 that is smaller that size
	}

	allocation_size = allocation_size <<1;		//this is to find the smallest size that is of power of 2 and is still enough to hold all bytes
	
	// if the head of the tree is too big, split it into two
	// repeat until it has a space thats the right size for it
	
	allocation_search(buddy_tree, allocation_size, &offset);			//this call attempts to allocate a block without spliting exisitng ones

	if(offset == -1)
	{
		allocating_split(buddy_tree, allocation_size, &offset);			//this call attempts to allocate a block by spliting free blocks
	}
	
	if (offset == -1){ return (void*)-1;}
	//printf("OFFSET = %d \n",offset);	
	return (void*)(offset+ glob_start_of_memory +4);
}

void *slab_it(int size)
{
	//local var
	struct slab *temp = slab_descripter;
	void *allocated;
	int updated = 0;
	int i;
	int space_used = 0;
	//first check if we have a slab, then check if its free 
	//if its full call buddy and send the slab
	//second if we dont have that type or if we have one and is full, then make a new one
	
	if(size * N_OBJS_PER_SLAB > glob_mem_size)
	{
		return (void*)(-1);
	}

	//makes sure we are not trying to allocate more space than we can when the new size request comes
	while (temp != NULL)
	{
		space_used += temp->size;
		if(space_used + (size * N_OBJS_PER_SLAB)  > glob_mem_size)
		{
			return (void*)(-1);
		}
		temp = temp->next;
	}
	
	temp = slab_descripter;

	if(slab_descripter == NULL)
	{
		slab_descripter = newSlab(size);
		slab_descripter->used = 1;
		allocated = buddy(slab_descripter->size);
		if(allocated ==(void*) -1){
			slab_descripter = NULL;
			return allocated;
		}
		else
		{
			allocated += 4;
		}
		
		slab_descripter->slab_pointer[0]=1;
		slab_descripter->offset = (int)(allocated - glob_start_of_memory);
		return allocated;
	}
	else
	{
		while(temp != NULL)
		{
			if(temp->type == size && temp->status != FULL)
			{
				//allocate if we can or make a new one 
				for(i = 0; i <N_OBJS_PER_SLAB; i++)
				{
					if(temp->slab_pointer[i] == 0)
					{
						temp->slab_pointer[i] = 1;
						updated = 1;
						temp->used += 1;
						temp->status = PARTIAL;
						//printf("ALLOCATED INTO EXISTING SLAB\n");
						allocated= (void*)(temp->offset + glob_start_of_memory+(size*i) + (i*4));
						break;
					}

					temp->status = FULL;	//this only happens when break is not called in the if statement, this means that everything in this slab is 1 aka occupied
				}
				break;	
			}
			temp = temp->next;
		}

		//we went through the table and didnt find the appropriate size
		if(updated == 0 && i >= 0)
		{
			temp = slab_descripter;
			while(temp->next != NULL)
			{
				temp = temp->next;
			}
			temp->next = newSlab(size);
			temp->next->slab_pointer[0]=1;
			temp->next->used = 1;
			allocated = buddy(temp->next->size);

			if (allocated == (void*)-1)
			{
				temp->next=NULL;
				return allocated;
			}
			else
			{
				allocated += 4;
				temp->next->offset = ((int)(allocated - glob_start_of_memory));// + 4;
			}
		}
	}
	return allocated;
	
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
	if( glob_malloc_type == 0) {return buddy(size);}
	else {slab_it(size);}

	//local variables
	//void *allocated_address;
        //printf("%d",allocated_address);
 //   	allocated_address = sbrk(size);
    	//printf("%d",allocated_address);
    	//if(*((int*)allocated_address) == -1){printf("%s", "allocation error when sbrk was used");}
}

void dfs_free(struct node* parent, int pointer, int* freed)
{
	//printf("MADE INTO DFSFREE pointer == %d\n",pointer);
	if(parent == NULL){return;} //if its nothing return

	else if( parent->offset == pointer && parent->status == OCCUPIED) //if the nodes offset is equal to the pointer free the node and update freed
	{
		parent->status=FREE;
		*freed = 1;
		return;
	}
	else if(parent->status == SPLIT) //if the node is split keep exploring
	{
		dfs_free(parent->left,pointer,freed);
		if(*freed == 1) 
		{
			combine(parent);
			return;
		} // if a node was found start backing up;
		
		dfs_free(parent->right,pointer,freed);
		if(*freed == 1)
		{
		
			combine(parent);
			return;
		}
	}
	return;
}
////////////////////////////////////////////////////////////////////////////
//
// Function     : buddy_free
// Description  : deallocated the memory segment being passed by the pointer using buddy system
//
// Inputs       : ptr - pointer to the memory segment to be free'd
// Outputs

void buddy_free(void *ptr)
{
	int pointer = (int)(ptr-glob_start_of_memory);
	pointer = pointer -4;
	int freed = 0;
	dfs_free(buddy_tree,pointer,&freed);	
}

////////////////////////////////////////////////////////////////////////////
//
// Function     : buddy_free
// Description  : deallocated the memory segment being passed by the pointer using buddy system
//
// Inputs       : ptr - pointer to the memory segment to be free'd
// Outputs

void slab_free(void *ptr)
{
	int pointer = (int)(ptr-glob_start_of_memory);
	//pointer = pointer -4;
	struct slab *temp = slab_descripter;
	struct slab *temp2 = slab_descripter;
	int i = -1;
	while(temp!=NULL)
	{
		if(pointer >= temp->offset && pointer < temp->offset + temp->size)
		{
			//printf("SLAB FOUND\n");
			for(i = 0; i < N_OBJS_PER_SLAB; i++)
			{
				if(pointer == temp->offset+((temp->type+4) *i))
				{
					temp->used -= 1;
					temp->slab_pointer[i]=0;
					if(temp->status == FULL)
					{
						temp->status = PARTIAL;
					}
					break;
				}
			}
			
			if(temp->used == 0)
			{
				
				buddy_free((void*)(temp->offset  + (glob_start_of_memory)-4));
				if(temp->next ==NULL) //if only one slab, empty table;
				{
					temp=NULL;
				}
				else
				{
				
					if(temp2->type = temp->type) //if the first slab needs to be removed
					{
						slab_descripter= slab_descripter->next;
					}
					else  //otherwise find the slab and remove it
					{
						while(temp2->type != temp->type)
						{			
							temp2= temp2->next;
						} 
						temp2->next = temp->next;
					}
				}
			}
			break;
		}
		temp=temp->next;
	}
		
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
	if(glob_malloc_type ==0) {buddy_free(ptr);}
	else {slab_free(ptr);}

}
