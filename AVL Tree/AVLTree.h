/** 
 * File: AVL.h
 * ---------------
 * Defines the interface for the AVL Tree.
 *
 * This version of the AVL Tree allow the client to add generic item
 * to a Tree and do other operation in O(lgn) .
 * this DataStructure Doesn't use Parent pointer in every node
 * this DataStructure Doesn't recursion functions
 * this DataStructure support 1-Iterators to traverse the Tree - In Order */
#ifndef _AVLTree_
#define _AVLTree_

/** 
 * Type: FreeFunction
 * ---------------
 * Defines the Function needed to free the Tree's elements when needed
 *
 * FreeFunction is pointer to client-supplied function to free the items
 * When dispose the Tree or when element need to be freed. the function will
 * recieve one element at time and it's job to free it. client will need to use it 
 * with all malloced variable, client will send NULL if it's in the stack
 */
typedef void (*FreeFunction) (void*);

/** 
 * Type: CompareFunction
 * ---------------
 * Defines the Function needed to compare two elements in the tree
 *
 * Takes two pointers to the start of two element and should compare them and 
 * return the result
 * MUST return one IF P1 == P2, Zero IF P1 < P2
 */
typedef int (*CompareFunction) (void* P1,void* P2);

/** 
 * Type: AVLnode
 * ---------------
 * Defines the Struct needed for the Single node
 *
 * Each node will have the data field and 3 pointer to the parent, 
 * right and left nodes
 */
typedef struct AVLnode{
	void* data;
	void* key;
	int height;
	struct AVLnode* right;
	struct AVLnode* left;
}AVLnode;

/** 
 * Type: AVLnodeparent
 * ---------------
 * Defines the Struct needed for the node and it's parent
 * I use this struct to pass Node's data between functions
 *
 * Each AVLnodeparent will have the pointer to the node , 
 * and pointer to it's parent
 */
typedef struct AVLnodeparent{
	struct AVLnode* node;
	struct AVLnode* parent;
}AVLnodeparent;

/** 
 * Type: AVLTree
 * ---------------
 * Defines the Struct needed for the AVL Trees
 *
 * size => number of the nodes in the tree
 * elemsize => the size in Byte for each element in the Tree
 * root => pointer to the root of the tree
 * datafreeFN => pointer to client supplied function used when freeing the data element
 * keyfreeFN => pointer to client supplied function used when freeing the key element
 * CmpFN => pointer to client supplied function used when comparing the key elements
 */
typedef struct AVLTree{
	long long size;
	int elemsize;
	int keysize;
	AVLnode* root;
	FreeFunction datafreeFN;
	FreeFunction keyfreeFN;
	CompareFunction CmpFN;
}AVLTree;

/** 
 * Type: AVLPushStack
 * Type: AVLPopStack
 * Type: AVLStackFreeFunction
 * ---------------
 * Defines the Function needed to make process on the Stack
 *
 * AVLPopStack : Take two pointers ,The first to the stack and the
 * second to the place in  memory to read the data from
 * AVLPushStack : Take two pointers ,The first to the stack and the
 * second to the place in  memory to write the data into
 * AVLStackFreeFunction : Take one pointer, to the element in the stack
 * we want to free
 */
typedef void (*AVLPushStack) (void*,void*);
typedef int (*AVLPopStack) (void*,void*);
typedef int (*AVLPeekStack) (void*,void*);
typedef void (*AVLStackFreeFunction) (void*);

/** 
 * Type: AVLStack
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
typedef struct AVLStack{
	int elemsize;
	int actuallength;
	int size;
	void* stack;
	AVLPushStack Push;
	AVLPopStack Pop;
	AVLPeekStack Peek;
	AVLStackFreeFunction Free;
}AVLStack;

/** 
 * Type: AVLIterator
 * ---------------
 * Defines the Struct needed for the Iterator
 *
 * tree => pointer to the tree which the Iterator traverse
 * node => pointer to the current node
 * stack => pointer to the stack of the Route 
 */
typedef struct AVLIterator{
	AVLTree* tree;
	AVLnode* node;
	AVLStack* stack;
}AVLIterator;

/** 
 * Function: InitializeAVLTree
 * ---------------
 * Initialize the given AVL Tree
 *
 * x => pointer to the AVL Tree
 * datafreeFN => pointer to client supplied function used when freeing the data element
 * keyfreeFN => pointer to client supplied function used when freeing the key element
 * CmpFN => pointer to client supplied function used when comparing the key elements
 */
void InitializeAVLTree(AVLTree* x, int dataSize, int keySize, CompareFunction,
 FreeFunction datafreeFN, FreeFunction keyfreeFN);

/** 
 * Function: DisposeAVLTree
 * ---------------
 * Dispose the given AVL Tree
 *
 * x => pointer to the AVL Tree
 */
void DisposeAVLTree(AVLTree* x);

/** 
 * Function: AVLTreeAddKey
 * ---------------
 * Add element with key the given AVL Tree
 *
 * x => pointer to the AVL Tree
 * key => pointer to the key that related to the element
 * elementAddress => pointer to the element you want to store
 * return 1 if the element's key was new, 0 if the element's key was
 * already there and the tree updated it's value
 */
int AVLTreeAddKey(AVLTree* x, void* key, void* elementAddress);

/** 
 * Function: AVLTreeSearch
 * ---------------
 * search for element with key in the given AVL Tree
 *
 * x => pointer to the AVL Tree
 * key => pointer to the key you want to search for
 * targetAddress => pointer to the part of memory you want to write the
 * element data into if it found
 * return 1 if the key was found and written to tragetAddress, 0
 * otherwise
 */
int AVLTreeSearch(AVLTree* x, void* key, void* targetAddress);

/** 
 * Function: AVLTreeDelete
 * ---------------
 * search for element with key in the given AVL Tree
 *
 * x => pointer to the AVL Tree
 * key => pointer to the key you want to delete the element related with
 * return 1 if the key was found and deleted, 0 otherwise
 */
int AVLTreeDelete(AVLTree* x, void* key);

/** 
 * Function: AVLTreeGetMax
 * ---------------
 * get the element with the maximum key in the given AVL Tree
 *
 * x => pointer to the AVL Tree
 * targetAddress => pointer to the part of memory you want to write the
 * element data into if it found
 * return 1 if the data was found and written, 0 if the tree is empty
 */
int AVLTreeGetMax(AVLTree* x, void* targetAddress);

/** 
 * Function: AVLTreeGetMin
 * ---------------
 * get the element with the minimum key in the given AVL Tree
 *
 * x => pointer to the AVL Tree
 * targetAddress => pointer to the part of memory you want to write the
 * element data into if it found
 * return 1 if the data was found and written, 0 if the tree is empty
 */
int AVLTreeGetMin(AVLTree* x, void* targetAddress);

/** 
 * Function: AVLTreeGetSuccessor
 * ---------------
 * get the successor element for the element's key in the given AVL Tree
 *
 * x => pointer to the AVL Tree
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
int AVLTreeGetSuccessor(AVLTree* x, void* key, void* targetAddress);

/** 
 * Function: AVLTreeGetPredecessor
 * ---------------
 * get the predecessor element for the element's key in the given AVL Tree
 *
 * x => pointer to the AVL Tree
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
int AVLTreeGetPredecessor(AVLTree* x, void* key, void* targetAddress);

/** 
 * Function: AVLGetIterator
 * ---------------
 * Return pointer to the Iterator - IN ORDER TRAVERSE
 * the Iterator initialy will be pointing to the first Node
 *
 * x => pointer to the AVLTree
 * return pointer to the iterator
 */
void* AVLGetIterator(AVLTree* x);

/** 
 * Function: AVLIteratorGetCurrent
 * ---------------
 * write the current node's data to the target address
 *
 * Iterator => pointer to the Iterator
 * it will write the current node's data to the targetAddress
 */
void AVLIteratorGetCurrent(void* Iterator, void* targetAddress);

/** 
 * Function: AVLIteratorHasNext
 * ---------------
 * check if there is next node 
 *
 * Iterator => pointer to the Iterator
 * it will return 1 if there is node, 0 otherwise
 */
int AVLIteratorHasNext(void* Iterator);

/** 
 * Function: AVLIteratorGoNext
 * ---------------
 * advance the Iterator to the forward
 *
 * Iterator => pointer to the Iterator
 * it will make the iterator point to the next node
 */
void AVLIteratorGoNext(void* Iterator);

/** 
 * Function: AVLIteratorDispose
 * ---------------
 * Free the Iterator ONLY
 *
 * Iterator => pointer to the Iterator
 * it will use free() to free the pointer data
 */
void AVLIteratorDispose(void* Iterator);

 #endif