#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "SingleLinkedList.h"

//create and return Node in the heap
static Snode* CreateNode(SlinkedList* x, void* elementAddress, Snode* next){
	//malloc the node
	Snode* element = (Snode *) malloc(sizeof(Snode));
	assert(element != NULL);

	//malloc space foe the data in the node
	element->element = (void *) malloc(x->elemsize);
	assert(element->element != NULL);

	//write the data in the Node
	memcpy(element->element, elementAddress, x->elemsize);
	//set the node next
	element->next = next;
	//return the node
	return element;
}

//free the node and it's data
static void FreeNode(SlinkedList* x, Snode* node){
	//if the user supply function use it, else just free the node
	if(x->freeFN != NULL)
		x->freeFN(node->element);

	free(node->element);
	free(node);
}

//free the node and it's data
static void FreeNodeOnly(SlinkedList* x, Snode* node){
	//Don't use the user supplied function
	//if(x->freeFN != NULL)
	//	x->freeFN(node->element);

	free(node->element);
	free(node);
}

//free the node and it's data
static void OFreeNode(SlinkedList* x, Snode* node){
	free(node);
}

void InitializeSLinkedList(SlinkedList* x, int elementSize, FreeFunction freeFN){
	//initialize the linkedlist
	x->elemsize = elementSize;
	x->freeFN = freeFN;
	x->size = 1;

	//make sential node at the head
	Snode* first = (Snode *)malloc(sizeof(Snode));
	assert(first != NULL);

	first->next =  NULL;
	first->element =  NULL;

	//make the head and the end point to the sentail node
	x->head = first;
	x->end = first;
}

void DisposeSLinkedList(SlinkedList* x){
	Snode* It = x->head;

	//Free every Node
	for(int i=0;i<SLinkedListSize(x)+1;i++){
		Snode* nex = It->next;
		if(i == 0)
			OFreeNode(x,It);
		else
			FreeNode(x,It);
		It = nex;
	}
}

long long SLinkedListSize(SlinkedList* x){
	return x->size-1;
}

void SLinkedListAddFront(SlinkedList* x,void* elementAddress){
	Snode* firstNode = x->head;

	//create Node pointing to the first node
	Snode* element = CreateNode(x,elementAddress,firstNode->next);

	//if it's the first element, make the end point to it
	if(SLinkedListSize(x) == 0)
		x->end = element;

	//fix the head node and increase the size
	firstNode->next = element;
	x->size++;
}

void SLinkedListRemoveFront(SlinkedList* x){

	//if there is no data just return
	if(SLinkedListSize(x) == 0)
		return;

	//fix the end pointer
	if(SLinkedListSize(x) == 1)
		x->end = x->head;

	//else free the node and fix the pointers and decrease the size
	Snode* temp = x->head->next->next;
	FreeNode(x,x->head->next);
	x->head->next = temp;
	x->size--;
}

static inline void SLinkedListRemoveFrontOnly(SlinkedList* x){

	//if there is no data just return
	if(SLinkedListSize(x) == 0)
		return;

	//fix the end pointer
	if(SLinkedListSize(x) == 1)
		x->end = x->head;

	//else free the node and fix the pointers and decrease the size
	Snode* temp = x->head->next->next;
	FreeNodeOnly(x,x->head->next);
	x->head->next = temp;
	x->size--;
}

void SLinkedListPeekFront(SlinkedList* x, void* targetAddress){
	//if there is no data just return
	if(SLinkedListSize(x) == 0)
		return;
	
	//copy the data to pointer
	memcpy(targetAddress,x->head->next->element,x->elemsize);
}

void SLinkedListExtractFront(SlinkedList* x, void* targetAddress){
	//write the data then remove it
	SLinkedListPeekFront(x,targetAddress);
	SLinkedListRemoveFrontOnly(x);
}

void SLinkedListAddBack(SlinkedList* x, void* elementAddress){
	Snode* lastNode = x->end;

	//create new node pointing to the end
	Snode* element = CreateNode(x,elementAddress,NULL);

	//fix the end pointer and increase the size
	lastNode->next = element;
	x->end = element;
	x->size++;
}

void SLinkedListRemoveBack(SlinkedList* x){
	//if there is no nodes just return
	if(SLinkedListSize(x) == 0)
		return;

	//else get to the wanted node
	Snode* curr = x->head;
	while(curr->next != x->end){
		curr = curr->next;
	}

	//free it and fix the end pointer
	FreeNode(x,curr->next);
	curr->next = NULL;
	x->end = curr;
	x->size--;
}

static inline void SLinkedListRemoveBackOnly(SlinkedList* x){
	//if there is no nodes just return
	if(SLinkedListSize(x) == 0)
		return;

	//else get to the wanted node
	Snode* curr = x->head;
	while(curr->next != x->end){
		curr = curr->next;
	}

	//free it and fix the end pointer
	FreeNodeOnly(x,curr->next);
	curr->next = NULL;
	x->end = curr;
	x->size--;
}

void SLinkedListPeekBack(SlinkedList* x, void* targetAddress){
	//write the data to the address given
	memcpy(targetAddress,x->end->element,x->elemsize);
}

void SLinkedListExtractBack(SlinkedList* x, void* targetAddress){
	//write the data then rmeove the data
	SLinkedListPeekBack(x,targetAddress);
	SLinkedListRemoveBackOnly(x);
}

void SLinkedListGet(SlinkedList* x, int index, void* targetAddress){
	//if the index bigger than or equal the list, return
	if(index >= SLinkedListSize(x))
		return;
	//else go to the node wanted
	Snode* curr = x->head;
	for(int i=0;i<index;i++){
		curr = curr->next;
	}
	//write the data to targetAddress
	memcpy(targetAddress,curr->next->element,x->elemsize);
}

void SLinkedListRemove(SlinkedList* x, int index){
	//if the index bigger than or equal the list, return
	if(index >= SLinkedListSize(x))
		return;
	//fix the start and end pointers
	if(index == 0)
		return SLinkedListRemoveFront(x);
	else if(index == SLinkedListSize(x)-1)
		return SLinkedListRemoveBack(x);
	//else go to the node wanted
	Snode* curr = x->head;
	for(int i=0;i<index;i++){
		curr = curr->next;
	}
	//Free the node and fix the pointers and decrease the size
	FreeNode(x,curr->next);
	curr->next = curr->next->next;
	x->size--;
}

void SLinkedListInsert(SlinkedList* x, int index, void* elementAddress){
	//if the index bigger than the list, return
	if(index > SLinkedListSize(x))
		return;
	//if the index is zero, insert it in the front
	else if(index == 0)
		SLinkedListAddFront(x,elementAddress);
	//if the index is Listsize, insert it in the back
	else if(index == SLinkedListSize(x))
		SLinkedListAddBack(x,elementAddress);
	//if the index is in between, get the node and create the node and increase the size
	else{
		
		Snode* curr = x->head;
		for(int i=0;i<index;i++){
			curr = curr->next;
		}

		curr->next = CreateNode(x,elementAddress,curr->next);;
		x->size++;
	}
}

int SLinkedListSearch(SlinkedList* x, void* keyAddress, CompareFunction cmpFN){
	//search the linked list for key, using the CompareFunction provided by the user
	//return first index of the key if found else -1
	Snode* curr = x->head;
	for(int i=0;i<SLinkedListSize(x);i++){
		curr = curr->next;
		if(cmpFN(keyAddress,curr->element))
			return i;
	}
	return -1;
}

void* SLinkedListGetIterator(SlinkedList* x){
	//mallocate Iterator and initalize it
	SIterator* y = malloc(sizeof(SIterator));
	assert(y != NULL);

	y->node = x->head;
	y->list = x;
	return y;	//return pointer to the iterator
}

void SLinkedListIteratorGetCurrent(void* Iterator, void* targetAddress){
	//cast and get the current element's data
	SIterator* y = (SIterator *) Iterator;
	memcpy(targetAddress,y->node->element,y->list->elemsize);
}

int SLinkedListIteratorHasNext(void* Iterator){
	//cast and chech if it has next node
	SIterator* y = (SIterator *) Iterator;
	return  y->node->next != NULL;
}

void SLinkedListIteratorGetNext(void* Iterator, void* targetAddress){
	//cast and get the next element's data
	SIterator* y = (SIterator *) Iterator;
	memcpy(targetAddress,y->node->next->element,y->list->elemsize);
}

void SLinkedListIteratorGoNext(void* Iterator){
	//cast and move the pointer to the next node
	SIterator* y = (SIterator *) Iterator;
	y->node = y->node->next;
}

void SLinkedListIteratorDispose(void* Iterator){
	//free the iterator
	free(Iterator);
}