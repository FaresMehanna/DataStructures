#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Stack.h"

//create and return Node in the heap
static Stacknode* CreateNode(Stack* x, void* elementAddress, Stacknode* next){
	//malloc the node
	Stacknode* element = (Stacknode *) malloc(sizeof(Stacknode));
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
static void FreeNode(Stack* x, Stacknode* node){
	//if the user supply function use it, else just free the node
	if(x->freeFN != NULL)
		x->freeFN(node->element);

	free(node->element);
	free(node);
}

void InitializeStack(Stack* x, int elementSize, FreeFunction freeFN){
	//initialize the linkedlist
	x->elemsize = elementSize;
	x->freeFN = freeFN;
	x->size = 1;

	//make sential node at the head
	Stacknode* first = (Stacknode *)malloc(sizeof(Stacknode));
	assert(first != NULL);

	first->next =  NULL;
	first->element =  NULL;

	//make the head and the end point to the sentail node
	x->head = first;
}

void DisposeStack(Stack* x){
	Stacknode* It = x->head;

	//Free every Node
	for(int i=-1;i<StackSize(x);i++){
		Stacknode* nex = It->next;
		FreeNode(x,It);
		It = nex;
	}
}

long long StackSize(Stack* x){
	return x->size-1;
}

void StackPush(Stack* x,void* elementAddress){
	Stacknode* firstNode = x->head;
	//create Node pointing to the first node
	Stacknode* element = CreateNode(x,elementAddress,firstNode->next);

	//fix the head node and increase the size
	firstNode->next = element;
	x->size++;
}

int StackRemove(Stack* x){
	//if there is no data just return
	if(StackSize(x) == 0)
		return 0;

	//else free the node and fix the pointers and decrease the size
	Stacknode* temp = x->head->next->next;
	FreeNode(x,x->head->next);
	x->head->next = temp;
	x->size--;
	return 1;
}

int StackPeek(Stack* x, void* targetAddress){

	//if there is no data just return
	if(StackSize(x) == 0)
		return 0;

	//copy the data to pointer
	memcpy(targetAddress,x->head->next->element,x->elemsize);
	return 1;
}

int StackPop(Stack* x, void* targetAddress){
	//write the data then remove it
	StackPeek(x,targetAddress);
	return StackRemove(x);
}

void* StackGetIterator(Stack* x){
	//mallocate Iterator and initalize it
	StackIterator* y = malloc(sizeof(StackIterator));
	y->node = x->head;
	y->list = x;
	return y;	//return pointer to the iterator
}

void StackIteratorGetCurrent(void* Iterator, void* targetAddress){
	//cast and get the current element's data
	StackIterator* y = (StackIterator *) Iterator;
	memcpy(targetAddress,y->node->element,y->list->elemsize);
}

int StackIteratorHasNext(void* Iterator){
	//cast and chech if it has next node
	StackIterator* y = (StackIterator *) Iterator;
	return  y->node->next != NULL;
}

void StackIteratorGetNext(void* Iterator, void* targetAddress){
	//cast and get the next element's data
	StackIterator* y = (StackIterator *) Iterator;
	memcpy(targetAddress,y->node->next->element,y->list->elemsize);
}

void StackIteratorGoNext(void* Iterator){
	//cast and move the pointer to the next node
	StackIterator* y = (StackIterator *) Iterator;
	y->node = y->node->next;
}

void StackIteratorDispose(void* Iterator){
	//free the iterator
	free(Iterator);
}