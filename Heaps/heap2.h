/** 
 * File: heaps1.h
 * ---------------
 * Defines the interface for the heaps.
 *
 * This version of the heap allows the user to add generic item related
 * with generic weight, it's implement both Max and Min heaps and allow
 * the user to build the heap from array and insert more element to it 
 * or just use insertion, the heap has no upper bound for it's size 
 * The client specifies the size (in bytes) of the elements that will be
 * stored in the heap when it is created.  Thereafter the client and 
 * the heap can refer to elements via (void *) ptrs. and the client must
 * iclude the Free function if the client malloc-ed the elements before the
 * insertion.
 */

#ifndef _heap2_
#define _heap2_

/** 
 * Type: FreeFunctionForData
 * ---------------
 * Defines the Function needed to free the data when needed
 *
 * FreeFunctionForData is pointer to client-supplied function to free the items
 * When dispose the Heap or when element need to be freed. the function will
 * recieve one element at time and it's job to free it. You will need use it 
 * with Malloced Variable or Struct and you will Not need this for simple types
 * like int, long long, float, doule ,char  
 */
typedef void (*FreeFunctionForData) (void*);

/** 
 * Type: FreeFunctionForWeight
 * ---------------
 * Defines the Function needed to free the Weight when needed
 *
 * FreeFunctionForWeight is pointer to client-supplied function to free the weights
 * When dispose the Heap or when weights need to be freed. the function will
 * recieve one element at time and it's job to free it. You will need use it 
 * with Malloced Variable or Struct and you will Not need this for simple types
 * like int, long long, float, doule ,char  
 */
typedef void (*FreeFunctionForWeight) (void*);

/** 
 * Type: CompareFunction
 * ---------------
 * Defines the function which will compare the weights of the elements.
 *
 * CompareFunction is pointer to client-supplied function to compare weights, This function
 * will recieve two pointers to the element the heap wants to compare and return the value
 * 0 or 1 , by changing how it do the comparison you can build MAX or MIN heaps
 */
typedef int (*CompareFunction) (void*,void*);

/** 
 * Type: heap
 * ---------------
 * Defines the struct needed for the heap
 *
 * array : points to the array of elements the client provide
 * weight : points to the array of weight corrosponding to the element in the
 * same index
 * logicalLen : stores how many element currently in the heap
 * actualLen : stores how many element we have to put the elemtns
 * // actualLen >= logicalLen //
 * elemsize : store the client-supplied size of each element in the array of data
 * weightsize : store the client-supplied size of each weight in the array of weights
 * FreeFNData : points to the function which will be called to free any data item
 * FreeFNWeight : points to the function which will be called to free any weight item
 * CmpFN : points to the function Which will be called to compare any two weight
 */
typedef struct {
	void  *array;
	void *weight;
	long long logicalLen;
	long long actualLen;
	int elemsize;
	int weightsize;
	FreeFunctionForData FreeFNData;
	FreeFunctionForWeight FreeFNWeight;
	CompareFunction CmpFN;
}heap;

/** 
 * Function: InitializeMaxHeap
 * ---------------
 * Initialize the given heap as a Maximum heap
 *
 * accepts pointer to the heap
 * elemsize which denote the number of Bytes of which each data element will occupy in the memory
 * weightsize which denote the number of Bytes of which each weight element will occupy in the memory
 * FreeFNData points to the function which will be called to free data items, If items don't
 * need to be freed you can supply NULL.
 * FreeFNWeight points to the function which will be called to free weight items, If items don't
 * need to be freed you can supply NULL.
 * CompareFunction points to the function which will be called to compare weight items, by implementing
 * this you choose to build Min/Max heap
 */
void InitializeHeap(heap *h, int elemsize, int weightsize, FreeFunctionForData f1,
 FreeFunctionForWeight f2, CompareFunction f3);

/** 
 * Function: DisposeHeap
 * ---------------
 * Destroy the given heap
 *
 * accepts pointer to the heap
 * Will call the FreeFunction if supplied on each element stored in the heap
 * and will Free the heap itself as well
 */
void DisposeHeap(heap *h);

/** 
 * Function: InsertKey
 * ---------------
 * Insert new Key with weight to the heap
 *
 * accepts pointer to the heap that will insert in it
 * accepts pointer to the address of the Key the client want to store it
 * accepts pointer to the weight corresponding to that key
 */
void InsertKey(heap *h, void* Key, void* weight);

/** 
 * Function: BuildFromArray
 * ---------------
 * Build heap from 2 given array, one for data and one for the weight
 *
 * accepts pointer to the heap that will insert in it
 * accepts pointer to the array of data
 * accepts pointer to the array of weight
 * accepts the length of the 2 arrays
 */
void BuildFromArray(heap* h,void* array, void* weight, int len);

/** 
 * Function: PeekFront
 * ---------------
 * write the Max or the Min element from the heap without removing it
 *
 * accepts pointer to the heap that will insert in it
 * accepts pointer to the location to write the Max/Min element in it
 */
void PeekFront(heap* h, void* target);

/** 
 * Function: ExtractFront
 * ---------------
 * Extract the Max or the Min element from the heap and remove it
 *
 * accepts pointer to the heap that will insert in it
 * accepts pointer to the location to write the Max/Min element in it
 */
void ExtractFront(heap* h, void* target);

/** 
 * Function: HeapSize
 * ---------------
 * Get the heap size
 *
 * accepts pointer to the heap that will insert in it
 * return how many elements in the heap
 */
long long HeapSize(heap* h);

#endif