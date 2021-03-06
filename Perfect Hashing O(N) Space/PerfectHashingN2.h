/** 
 * File: ChainHash.h
 * ---------------
 * Defines the interface for the hash set.
 *
 * This version of the hash set allow the client to add generic item
 * to a Tree and do other operation in O(1) in average.
 */
#ifndef _PERFECTHASHINGN2_
#define _PERFECTHASHINGN2_

/** 
 * Type: FreeFunction
 * ---------------
 * Defines the Function needed to free the  elements when needed
 *
 * FreeFunction is pointer to client-supplied function to free the items
 * When dispose the Hash set or when element need to be freed. the function will
 * recieve one element at time and it's job to free it. client will need to use it 
 * with all malloced variable, client will send NULL if it's in the stack
 */
typedef void (*FreeFunction) (void*);

/** 
 * Type: CompareFunction
 * ---------------
 * Defines the Function needed to compare two elements in the set
 *
 * Takes two pointers to the start of two element and should compare them and 
 * return the result
 * MUST return one IF P1 == P2, zero otherwise
 */
typedef int (*CompareFunction) (void* P1,void* P2);

/** 
 * Type: Hash
 * ---------------
 * Defines the Function needed to hash any element in the set
 *
 * Takes pointer to the key of the element and it's size and max hashed number
 * should return valid hash number
 */
typedef unsigned int (*Hash) (void* key, int keysize, long long max ,long long c1
	, long long c2);

/** 
 * Type: DataKey
 * ---------------
 * Defines the Struct needed for the DataKey
 *
 * data => pointing to the data
 * key => pointing to the key
 */
typedef struct DataKey{
	void* data;
	void* key;
}DataKey;

/** 
 * Type: PerfectHashSetN2
 * ---------------
 * Defines the Struct needed for the Hash set
 *
 * size => number of the element in the hash set
 * elemsize => the size in Byte for each element in the set
 * data => pointer to the array of linkedlist
 * datafreeFN => pointer to client supplied function used when freeing the data element
 * keyfreeFN => pointer to client supplied function used when freeing the key element
 * CmpFN => pointer to client supplied function used when comparing the key elements
 */
typedef struct PerfectHashSetN2{
	long long size;
	long long actualsize;
	long long cols;
	int elemsize;
	int keysize;
	long long c1;
	long long c2;
	DataKey** data;
	FreeFunction datafreeFN;
	FreeFunction keyfreeFN;
	CompareFunction CmpFN;
	Hash getHash;
}PerfectHashSetN2;

/** 
 * Function: InitializePerfectHashSetN2
 * ---------------
 * Initialize the given PerfectHashSetN2
 *
 * x => pointer to the HashSet
 * datafreeFN => pointer to client supplied function used when freeing the data element
 * keyfreeFN => pointer to client supplied function used when freeing the key element
 * CmpFN => pointer to client supplied function used when comparing the key elements
 */
void InitializePerfectHashSetN2(PerfectHashSetN2* x, int dataSize, int keySize, CompareFunction,
 FreeFunction datafreeFN, FreeFunction keyfreeFN, Hash hashFn);

/** 
 * Function: DisposePerfectHashSetN2
 * ---------------
 * Dispose the given PerfectHashSetN2
 *
 * x => pointer to the HashSet
 */
void DisposePerfectHashSetN2(PerfectHashSetN2* x, int full);

/** 
 * Function: PerfectHashSetN2AddKey
 * ---------------
 * Add element with key the given PerfectHashSetN2
 *
 * x => pointer to the HashSet
 * key => pointer to the key that related to the element
 * elementAddress => pointer to the element you want to store
 * return 1 if the element's key was new, 0 if the element's key was
 * already there and updated the data related to the key
 */
int PerfectHashSetN2AddKey(PerfectHashSetN2* x, void* key, void* elementAddress);

/** 
 * Function: PerfectHashSetN2Search
 * ---------------
 * search for element with key in the given PerfectHashSetN2
 *
 * x => pointer to the HashSet
 * key => pointer to the key you want to search for
 * targetAddress => pointer to the part of memory you want to write the
 * element data into if it found
 * return 1 if the key was found and written to tragetAddress, 0
 * otherwise
 */
int PerfectHashSetN2Search(PerfectHashSetN2* x, void* key, void* targetAddress);

/** 
 * Function: PerfectHashSetN2Delete
 * ---------------
 * search for element with key in the given PerfectHashSetN2
 *
 * x => pointer to the HashSet
 * key => pointer to the key you want to delete the element related with
 * return 1 if the key was found and deleted, 0 otherwise
 */
int PerfectHashSetN2Delete(PerfectHashSetN2* x, void* key);


long long PerfectHashSetN2GetSize(PerfectHashSetN2* x);

 #endif
