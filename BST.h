/** 
 * File: BST.h
 * ---------------
 * Defines the interface for the BST Tree.
 *
 * This version of the BST Tree allow the client to add generic item
 * to a Tree and do other operation in O(h) .
 * this DataStructure support 1-Iterators to traverse the Tree
 */
#ifndef _BSTTree_
#define _BSTTree_

/** 
 * Type: FreeFunction
 * ---------------
 * Defines the Function needed to free the Tree's elements when needed
 *
 * FreeFunction is pointer to client-supplied function to free the items
 * When dispose the Heap or when element need to be freed. the function will
 * recieve one element at time and it's job to free it. client will need to use it 
 * with all malloced variable, client will send NULL if it's in the stack
 */
typedef void (*FreeFunction) (void*);

/** 
 * Type: CompareFunction
 * ---------------
 * Defines the Function needed to compare two elements in the tree
 *
 * Takes two pointers to the start of two element and shoue compare then and 
 * return the result
 * MUST return one IF P1 == P2, Zero IF P1 < P2
 */
typedef int (*CompareFunction) (void* P1,void* P2);

/** 
 * Type: BSTnode
 * ---------------
 * Defines the Struct needed for the Single node
 *
 * Each node will have the data field and 3 pointer to the parent, 
 * right and left nodes
 */
typedef struct BSTnode{
	void* data;
	void* key;
	struct BSTnode* right;
	struct BSTnode* left;
}BSTnode;

/** 
 * Type: BSTnodeparent
 * ---------------
 * Defines the Struct needed for the node and it's parent
 * I use this struct to pass Node's data between functions
 *
 * Each BSTnodeparent will have the pointer to the node , 
 * and pointer to it's parent
 */
typedef struct BSTnodeparent{
	struct BSTnode* node;
	struct BSTnode* parent;
}BSTnodeparent;

/** 
 * Type: BSTTree
 * ---------------
 * Defines the Struct needed for the BST Trees
 *
 * size => number of the nodes in the tree
 * elemsize => the size in Byte for each element in the Tree
 * root => pointer to the root of the tree
 * datafreeFN => pointer to client supplied function used when freeing the data element
 * keyfreeFN => pointer to client supplied function used when freeing the key element
 * CmpFN => pointer to client supplied function used when comparing the key elements
 */
typedef struct BSTTree{
	long long size;
	int elemsize;
	int keysize;
	BSTnode* root;
	FreeFunction datafreeFN;
	FreeFunction keyfreeFN;
	CompareFunction CmpFN;
}BSTTree;

/** 
 * Type: BSTPushStack
 * Type: BSTPopStack
 * Type: BSTStackFreeFunction
 * ---------------
 * Defines the Function needed to make process on the Stack
 *
 * BSTPopStack : Take two pointers ,The first to the stack and the
 * second to the place in  memory to read the data from
 * BSTPushStack : Take two pointers ,The first to the stack and the
 * second to the place in  memory to write the data into
 * BSTStackFreeFunction : Take one pointer, to the element in the stack
 * we want to free
 */
typedef void (*BSTPushStack) (void*,void*);
typedef int (*BSTPopStack) (void*,void*);
typedef void (*BSTStackFreeFunction) (void*);

/** 
 * Type: BSTStack
 * ---------------
 * Defines the Struct needed for the Stack that holds the Route
 * taken from the top to the buttom
 *
 * elemsize => Number of Bytes taken by every element in the stack
 * actuallength => the actual length of the stack in the memory
 * size => Number of elements in the stack
 * Push => pointer to function to push the element
 * Pop => pointer to function to Pop the element
 * Free => pointer to function to Free the element
 */
typedef struct BSTStack{
	int elemsize;
	int actuallength;
	int size;
	void* stack;
	BSTPushStack Push;
	BSTPopStack Pop;
	BSTStackFreeFunction Free;
}BSTStack;

/** 
 * Type: BSTIterator
 * ---------------
 * Defines the Struct needed for the Iterator
 *
 * tree => pointer to the tree which the Iterator traverse
 * node => pointer to the current node
 * stack => pointer to the stack of the Route 
 */
typedef struct BSTIterator{
	BSTTree* tree;
	BSTnode* node;
	BSTStack* stack;
}BSTIterator;

/** 
 * Function: InitializeBSTTree
 * ---------------
 * Initialize the given BST Tree
 *
 * x => pointer to the BST Tree
 * datafreeFN => pointer to client supplied function used when freeing the data element
 * keyfreeFN => pointer to client supplied function used when freeing the key element
 * CmpFN => pointer to client supplied function used when comparing the key elements
 */
void InitializeBSTTree(BSTTree* x, int dataSize, int keySize, CompareFunction,
 FreeFunction datafreeFN, FreeFunction keyfreeFN);

/** 
 * Function: DisposeBSTTree
 * ---------------
 * Dispose the given BST Tree
 *
 * x => pointer to the BST Tree
 */
void DisposeBSTTree(BSTTree* x);

/** 
 * Function: BSTTreeAddKey
 * ---------------
 * Add element with key the given BST Tree
 *
 * x => pointer to the BST Tree
 * key => pointer to the key that related to the element
 * elementAddress => pointer to the element you want to store
 * return 1 if the element's key was new, 0 if the element's key was
 * already there and the tree updated it's value
 */
int BSTTreeAddKey(BSTTree* x, void* key, void* elementAddress);

/** 
 * Function: BSTTreeSearch
 * ---------------
 * search for element with key in the given BST Tree
 *
 * x => pointer to the BST Tree
 * key => pointer to the key you want to search for
 * targetAddress => pointer to the part of memory you want to write the
 * element data into if it found
 * return 1 if the key was found and written to tragetAddress, 0
 * otherwise
 */
int BSTTreeSearch(BSTTree* x, void* key, void* targetAddress);

/** 
 * Function: BSTTreeDelete
 * ---------------
 * search for element with key in the given BST Tree
 *
 * x => pointer to the BST Tree
 * key => pointer to the key you want to delete the element related with
 * return 1 if the key was found and deleted, 0 otherwise
 */
int BSTTreeDelete(BSTTree* x, void* key);

/** 
 * Function: BSTTreeGetMax
 * ---------------
 * get the element with the maximum key in the given BST Tree
 *
 * x => pointer to the BST Tree
 * targetAddress => pointer to the part of memory you want to write the
 * element data into if it found
 * return 1 if the data was found and written, 0 if the tree is empty
 */
int BSTTreeGetMax(BSTTree* x, void* targetAddress);

/** 
 * Function: BSTTreeGetMin
 * ---------------
 * get the element with the minimum key in the given BST Tree
 *
 * x => pointer to the BST Tree
 * targetAddress => pointer to the part of memory you want to write the
 * element data into if it found
 * return 1 if the data was found and written, 0 if the tree is empty
 */
int BSTTreeGetMin(BSTTree* x, void* targetAddress);

/** 
 * Function: BSTTreeGetSuccessor
 * ---------------
 * get the successor element for the element's key in the given BST Tree
 *
 * x => pointer to the BST Tree
 * key => pointer to the key you want to get it's successor
 * targetAddress => pointer to the part of memory you want to write the
 * element data into if it found
 * return 1 if the element with that key was already there,
 * 0 if the element's key wasn't there
 *
 * NOTE : IN both cases this function will return the Successor
 * NOTE : IF the key was larger than any key, this function will return
 * MAX element
 */
int BSTTreeGetSuccessor(BSTTree* x, void* key, void* targetAddress);

/** 
 * Function: BSTTreeGetPredecessor
 * ---------------
 * get the predecessor element for the element's key in the given BST Tree
 *
 * x => pointer to the BST Tree
 * key => pointer to the key you want to get it's predecessor
 * targetAddress => pointer to the part of memory you want to write the
 * element data into if it found
 * return 1 if the element with that key was already there,
 * 0 if the element's key wasn't there
 *
 * NOTE : IN both cases this function will return the predecessor
 * NOTE : IF the key was smaller than any key, this function will return
 * MIN element
 */
int BSTTreeGetPredecessor(BSTTree* x, void* key, void* targetAddress);

/** 
 * Function: BSTGetIterator
 * ---------------
 * Return pointer to the Iterator - IN ORDER TRAVERSE
 * the Iterator initialy will be pointing to the first Node
 *
 * x => pointer to the BSTTree
 * return pointer to the iterator
 */
void* BSTGetIterator(BSTTree* x);

/** 
 * Function: BSTIteratorGetCurrent
 * ---------------
 * write the current node's data to the target address
 *
 * Iterator => pointer to the Iterator
 * it will write the current node's data to the targetAddress
 */
void BSTIteratorGetCurrent(void* Iterator, void* targetAddress);

/** 
 * Function: BSTIteratorHasNext
 * ---------------
 * check if there is next node 
 *
 * Iterator => pointer to the Iterator
 * it will return 1 if there is node, 0 otherwise
 */
int BSTIteratorHasNext(void* Iterator);

/** 
 * Function: BSTIteratorGoNext
 * ---------------
 * advance the Iterator to the forward
 *
 * Iterator => pointer to the Iterator
 * it will make the iterator point to the next node
 */
void BSTIteratorGoNext(void* Iterator);

/** 
 * Function: BSTIteratorDispose
 * ---------------
 * Free the Iterator ONLY
 *
 * Iterator => pointer to the Iterator
 * it will use free() to free the pointer data
 */
void BSTIteratorDispose(void* Iterator);

 #endif