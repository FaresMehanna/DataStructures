#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "ChainHash.h"

static inline unsigned int getHash(void* key, int keysize, long long max){

	unsigned int hash = 2166136261;

	for(int i=0;i<keysize;i++){
		hash = hash ^ (*(char *)key + i);
 		hash = hash * 16777619;
 	}

	return hash % max;
}

static inline DataKey* createDataKey(HashSetChain* x ,void* key ,void* elementAddress){
	//create the pair
	DataKey* y = malloc(sizeof(DataKey));
	y->data = malloc(x->elemsize);
	y->key = malloc(x->keysize);
	//set the data
	memcpy (y->data,elementAddress,x->elemsize);
	memcpy (y->key,key,x->keysize);
	//return
	return y;
}

static inline void userDeleteDataKeyPair(HashSetChain* x ,DataKey* y){

	if(x->datafreeFN != NULL)
		x->datafreeFN(y->data);
	if(x->keyfreeFN != NULL)
		x->keyfreeFN(y->key);

}

static inline void totalyDeleteLinkedList(HashSetChain* x ,SlinkedList* l){

	DataKey* y;
	void* iterator = SLinkedListGetIterator(l);
		
	while(SLinkedListIteratorHasNext(iterator)){
		//get the item
		SLinkedListIteratorGetNext(iterator,&y);
		SLinkedListIteratorGoNext(iterator);
		//delete the item
		userDeleteDataKeyPair(x,y);
	}

	//free the iterator
	SLinkedListIteratorDispose(iterator);

	//free the linkedlist
	DisposeSLinkedList(l);
}

static inline int getIndexFromLinkedList(HashSetChain* x,SlinkedList* SL, void* key, int* targetAddressForIndex){

	//create datakey pair
	DataKey* y;

	//get the index from the list
	void* iterator = SLinkedListGetIterator(SL);
	int index = 0;
	int found = 0;
	while(SLinkedListIteratorHasNext(iterator)){
		
		//get the item
		SLinkedListIteratorGetNext(iterator,&y);
		SLinkedListIteratorGoNext(iterator);

		//is it the one we wanted ?
		if(x->CmpFN(y->key,key)){
			found = 1;
			break;
		}
		index++;
	}

	//free the iterator
	SLinkedListIteratorDispose(iterator);

	if(found)
		*targetAddressForIndex = index;

	return found;
}

void freeKeySLinkedList(void* f1){
	DataKey* x1 = *(DataKey**) f1;
	free(x1->key);
	free(x1->data);
	free(x1);
}

static inline void GrowHashSetChain(HashSetChain* x){
	//if the sizes aren't the same return
	if(x->actualsize != x->size)
		return;
	//else double the size
	x->actualsize = x->actualsize*2;
	//pointer to the old array of pointers
	SlinkedList** old = x->data;
	//make the new array of pointers
	x->data = malloc(sizeof(SlinkedList*) * x->actualsize);
	//create the lists for the new array
	for(int i=0;i<x->actualsize;i++){
		(x->data)[i] = malloc(sizeof(SlinkedList));
		InitializeSLinkedList((x->data)[i],sizeof(DataKey*),freeKeySLinkedList);
	}

	//delete the old array and create new one
	DataKey* y;
	for(int i=0;i<x->actualsize/2;i++){
		//current linkedList
		SlinkedList* curr = old[i];
		//Iterator to it
		void* iterator = SLinkedListGetIterator(curr);
		//iterate through the linkedlist
		while(SLinkedListIteratorHasNext(iterator)){
			
			//get the item
			SLinkedListIteratorGetNext(iterator,&y);
			SLinkedListIteratorGoNext(iterator);
			//new hash
			unsigned int hash = x->getHash(y->key,x->keysize, x->actualsize);
			//copy it's data without user-deleting-it!!
			DataKey* new = createDataKey(x,y->key,y->data);
			//add to the new list
			SLinkedListAddFront((x->data)[hash], &new);
		}

		//free the iterator
		SLinkedListIteratorDispose(iterator);
		//free the linkedlist
		DisposeSLinkedList(curr);
		//free the linkedlist itself
		free(curr);
	}

	//free the old array
	free(old);
}

void InitializeHashSetChain(HashSetChain* x, int dataSize, int keySize, CompareFunction CmpFN,
 FreeFunction datafreeFN, FreeFunction keyfreeFN, Hash hashFn){

	//initialize the data in the tree
	x->size = 0;
	x->actualsize = 8;
	x->elemsize = dataSize;
	x->keysize = keySize;
	//the array holding the pointers to the linkedlists
	x->data = malloc(sizeof(SlinkedList*) * 8);
	//malloc the LinkedLists
	for(int i=0;i<8;i++){
		(x->data)[i] = malloc(sizeof(SlinkedList));
		InitializeSLinkedList((x->data)[i],sizeof(DataKey*),freeKeySLinkedList);
	}
	x->datafreeFN = datafreeFN;
	x->keyfreeFN = keyfreeFN;
	x->CmpFN = CmpFN;
	x->getHash = getHash;

	if(hashFn != NULL)
		x->getHash = hashFn;
}

void DisposeHashSetChain(HashSetChain* x){
	//the array of linkedlists
	SlinkedList** temp = x->data;

	for(int i=0;i<x->actualsize;i++){
		//the current linkedlist
		SlinkedList* curr = temp[i];

		//delete the data
		totalyDeleteLinkedList(x,curr);

		//free the linkedlist
		free((x->data)[i]);
	}
	
	//free the array of pointers
	free(temp);
}

//will return 1 incase the add succ
//will return 0 incase the add failed and updated instead
int HashSetChainAddKey(HashSetChain* x, void* key, void* elementAddress){
	//grow the set if needed
	GrowHashSetChain(x);

	//get the hash value
	unsigned int hash = x->getHash(key,x->keysize,x->actualsize);

	//create datakey pair
	DataKey* y = createDataKey(x,key,elementAddress);

	int index = 0;
	if(getIndexFromLinkedList(x,(x->data)[hash],key,&index)){
		//get the current key
		DataKey* temp;
		SLinkedListGet((x->data)[hash], index, &temp);
		//delete it by the user defined functions
		userDeleteDataKeyPair(x,temp);
		//remove it from the list
		SLinkedListRemove((x->data)[hash], index);
		//add the new one
		SLinkedListAddFront((x->data)[hash], &y);
		return 0;

	}else{
		SLinkedListAddFront((x->data)[hash], &y);
		//increase the size
		x->size++;
		return 1;
	}
}


int HashSetChainSearch(HashSetChain* x, void* key, void* targetAddress){
	
	//get the hash value
	unsigned int hash = x->getHash(key,x->keysize,x->actualsize);

	//create datakey pair
	DataKey* y;
	
	int index;
	if(getIndexFromLinkedList(x,(x->data)[hash],key,&index)){
		SLinkedListGet((x->data)[hash], index, &y);
		memcpy(targetAddress,y->data,x->elemsize);
		return 1;
	}

	return 0;
}

int HashSetChainDelete(HashSetChain* x, void* key){

	//get the hash value
	unsigned int hash = x->getHash(key,x->keysize,x->actualsize);

	//create datakey pair
	DataKey* y;

	//get the index from the list
	int index;
	if(getIndexFromLinkedList(x,(x->data)[hash],key,&index)){
		//get the pair
		SLinkedListGet((x->data)[hash], index, &y);
		//user delete it
		userDeleteDataKeyPair(x,y);
		//linkedlist delete it
		SLinkedListRemove((x->data)[hash], index);
		return 1;
	}

	return 0;
}

int HashSetChainSize(HashSetChain* x){
	return x->size;
}