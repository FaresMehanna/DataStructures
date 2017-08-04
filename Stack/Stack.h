/** 
 * File: Stack.h
 * ---------------
 * Defines the interface for the Stack.
 *
 * This version of the Stack allow the client to push generic item
 * to a list in Front or pop them in constant time .
 * this DataStructure support Iterators
 */
#ifndef _stack_
#define _stack_


/** 
 * Type: FreeFunction
 * ---------------
 * Defines the Function needed to free the Stacks's element when needed
 *
 * FreeFunction is pointer to client-supplied function to free the items
 * When dispose the Stack or when element need to be freed. the function will
 * recieve one element at time and it's job to free it. client will need to use it 
 * with all malloced variable, client will send NULL if it's in the stack
 */
typedef void (*FreeFunction) (void*);


/** 
 * Type: Stacknode
 * ---------------
 * Defines the Struct needed for the Single node
 *
 * Each node will have two field, pointer to the data and pointer to the
 * next node 
 */
typedef struct Stacknode{
	void* element;
	struct Stacknode* next;
}Stacknode;

/** 
 * Type: Stack
 * ---------------
 * Defines the Struct needed for the Stack
 *
 * size => Number of the element in the stack
 * elemsize => the size in Byte for each element in the Stack
 * freeFN => pointer to client supplied function used when freeing the elements
 * head => pointer to dummy node at the head
 */
typedef struct Stack{
	long long size;
	int elemsize;
	FreeFunction freeFN;
	Stacknode* head;
}Stack;

/** 
 * Type: StackIterator
 * ---------------
 * Defines the Struct needed for the Iterator
 *
 * list => pointer to the Stack which the Iterator traverse
 * ndoe => pointer to the current node
 */
typedef struct StackIterator{
	Stack* list;
	Stacknode* node;
}StackIterator;

/** 
 * Function: InitializeStack
 * ---------------
 * Initialize the given Stack
 *
 * x => pointer to the Stack
 * elementSize => the number of bytes for each element in the stack
 * FreeFunction => the function which will be used to free items, NULL if items
 * is simple type as int,long,char,...
 */
void InitializeStack(Stack* x, int elementSize, FreeFunction);

/** 
 * Function: DisposeStack
 * ---------------
 * Dispose the given Stack
 *
 * x => pointer to the Stack
 */
void DisposeStack(Stack* x);

/** 
 * Function: StackPush
 * ---------------
 * Add element to the start of the Stack
 *
 * x => pointer to the Stack
 * elementAddress => pointer to the item data
 */
void StackPush(Stack* x,void* elementAddress);

/** 
 * Function: StackRemove
 * ---------------
 * Remove element From the start of the Stack
 *
 * x => pointer to the Stack
 * Returns 1 in case of success, 0 if the stack is empty
 */
int StackRemove(Stack* x);

/** 
 * Function: StackPeek
 * ---------------
 * write the first element from the Stack in targetAddress
 *
 * x => pointer to the Stack
 * targetAddress => pointer to the place in memory to write the data in it
 * Returns 1 in case of success, 0 if the stack is empty
 */
int StackPeek(Stack* x, void* targetAddress);

/** 
 * Function: StackPop
 * ---------------
 * write the first element from the Stack in targetAddress AND remove the element
 *
 * x => pointer to the Stack
 * targetAddress => pointer to the place in memory to write the data in it
 * Returns 1 in case of success, 0 if the stack is empty
 */
int StackPop(Stack* x, void* targetAddress);

/** 
 * Function: StackSize
 * ---------------
 * return number of elements of the Stack
 */
long long StackSize(Stack* x);

/** 
 * Function: StackGetIterator
 * ---------------
 * Return pointer to the Iterator
 * the Iterator initialy will be pointing to null Node
 *
 * x => pointer to the Stack
 * return pointer to the iterator
 */
void* StackGetIterator(Stack* x);

/** 
 * Function: StackIteratorGetCurrent
 * ---------------
 * write the current node's data to the target address
 *
 * Iterator => pointer to the Iterator
 * it will write the current node's data to the targetAddress
 */
void StackIteratorGetCurrent(void* Iterator, void* targetAddress);

/** 
 * Function: StackIteratorHasNext
 * ---------------
 * check if there is next node 
 *
 * Iterator => pointer to the Iterator
 * it will return 1 if there is node, 0 otherwise
 */
int StackIteratorHasNext(void* Iterator);

/** 
 * Function: StackIteratorGetNext
 * ---------------
 * write the current next node's data to the target address
 *
 * Iterator => pointer to the Iterator
 * it will write the next node's data to the targetAddress
 */
void StackIteratorGetNext(void* Iterator, void* targetAddress);

/** 
 * Function: StackIteratorGoNext
 * ---------------
 * advance the Iterator to the forward
 *
 * Iterator => pointer to the Iterator
 * it will make the iterator point to the next node
 */
void StackIteratorGoNext(void* Iterator);

/** 
 * Function: StackIteratorDispose
 * ---------------
 * Free the Iterator ONLY
 *
 * Iterator => pointer to the Iterator
 * it will use free() to free the pointer data
 */
void StackIteratorDispose(void* Iterator);
#endif