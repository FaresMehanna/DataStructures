#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "heap2.h"


static void GrowHeap(heap *h){

	//double the logical length and the array
	h->actualLen = h->actualLen * 2;
	h->array = realloc(h->array,h->actualLen*h->elemsize); 
	h->weight = realloc(h->weight,h->actualLen*h->weightsize); 

	//incase of error stop here and print error
	assert(h->array != NULL);
	assert(h->weight != NULL);
}

static void swap(void* f1 , void* f2 , int elemsize){

	//make temp with the same size of every element
	void* temp = malloc(elemsize);

	//swap the elements
	memcpy(temp,f1,elemsize);
	memcpy(f1,f2,elemsize);
	memcpy(f2,temp,elemsize);

	//free temp
	free(temp);
}

//return the parent of a node
static void* GetParent(heap* h, int i){
	return (char *) h->array + (i/2) * h->elemsize;
}

//get weight of node
static void* WeightNode(heap* h, int i){
	return (char *)h->weight + i * h->weightsize;
}

//return a node
static void* Node(heap* h, int i){
	return (char *) h->array + i * h->elemsize;
}

//get parent weight
static void* ParentWeight(heap* h, int i){
	return WeightNode(h,i/2);
}

static void FixBottomUp(heap* h, int i){
	//fix the tree from the given root
	while(i != 1 && h->CmpFN(WeightNode(h,i), ParentWeight(h,i))){
		swap(ParentWeight(h,i),WeightNode(h,i),h->weightsize);
		swap(GetParent(h,i),Node(h,i),h->elemsize);
		i = i/2;
	}
}


static void InsertWithNoOrder(heap *h, void* elemAddr, void* weight){
	//if the heap is full expand it
	if(h->logicalLen == h->actualLen)
		GrowHeap(h);

	//get the target pointer
	void* targetData = (char *) h->array + h->elemsize * h->logicalLen;
	void* targetWeight = (char *) h->weight + h->weightsize * h->logicalLen;

	//copy the data to the wanted address
	memcpy(targetData,elemAddr,h->elemsize);
	memcpy(targetWeight,weight,h->weightsize);

	//increase the Length by one
	h->logicalLen++;
}

static void Heapify(heap* h, int i){

	int left = 2*i;
	int right = 2*i +1;
	int largest = i;

	if(left < h->logicalLen && h->CmpFN(WeightNode(h,left), WeightNode(h,i)))
		largest = left;
	
	if(right < h->logicalLen && h->CmpFN(WeightNode(h,right), WeightNode(h,largest)))
		largest = right;

	if(largest != i){
		swap(Node(h,i),Node(h,largest),h->elemsize);
		swap(WeightNode(h,i),WeightNode(h,largest),h->weightsize);
		Heapify(h,largest);
	}
}

void InitializeHeap(heap *h, int elemsize, int weightsize,
 FreeFunctionForData freefndata, FreeFunctionForWeight freefnweight, CompareFunction CmpFN){
	//malloc 4 element of each array
	h->array = malloc(elemsize * 4);
	h->weight = malloc(weightsize * 4);

	//provide the rest of the data
	h->elemsize = elemsize;
	h->weightsize = weightsize;
	h->logicalLen = 1;
	h-> actualLen = 4;
	h->FreeFNData = freefndata;	
	h->FreeFNWeight = freefnweight;
	h->CmpFN = CmpFN;
}

void DisposeHeap(heap *h){
	//if there is function to free the element use it 
	if(h->FreeFNData != NULL)
		for(int i=1;i<h->logicalLen;i++)
			h->FreeFNData((char *)h->array + i * h->elemsize);

	if(h->FreeFNWeight != NULL)
		for(int i=1;i<h->logicalLen;i++)
			h->FreeFNWeight((char *)h->weight + i * h->weightsize);

	//free the arrays malloced
	free(h->array);
	free(h->weight);
	h->array = NULL;
	h->weight = NULL;
}

void InsertKey(heap *h, void* elemAddr, void* weight){
	//if the heap is full expand it
	if(h->logicalLen == h->actualLen)
		GrowHeap(h);

	//get the target pointer
	void* targetData = (char *) h->array + h->elemsize * h->logicalLen;
	void* targetWeight = (char *) h->weight + h->weightsize * h->logicalLen;

	//copy the data to the wanted address
	memcpy(targetData,elemAddr,h->elemsize);
	memcpy(targetWeight,weight,h->weightsize);

	//fix the tree from the bottom to the up
	FixBottomUp(h,h->logicalLen);

	//increase the Length by one
	h->logicalLen++;

}

void BuildFromArray(heap* h,void* array, void* weight, int len){
	
	//insert all the element with no Order
	for(int i=0;i<len;i++)
		InsertWithNoOrder(h,(char *)array + i * h->elemsize ,(char *)weight + i * h->weightsize);
	
	//Fix the order from the bottom up
	for(int i=len/2;i!=0;i--)
		Heapify(h,i);
}

void ExtractFront(heap* h, void* target){

	//check if there is no more element in the heap
	assert(h->logicalLen != 1);
	//decrease the heap Logical length by one
	h->logicalLen--;
	//output the result to the target pointer
	memcpy(target,(char *)h->array + h->elemsize,h->elemsize);
	//put the last element as the first element
	memmove((char *)h->array + h->elemsize,(char *)h->array + h->elemsize * h->logicalLen,h->elemsize);
	memmove((char *)h->weight + h->weightsize,(char *)h->weight + h->weightsize * h->logicalLen,h->elemsize);
	//fix the tree
	Heapify(h,1);
}

void PeekFront(heap* h, void* target){

	//check if there is no more element in the heap
	assert(h->logicalLen != 1);
	//output the result to the target pointer
	memcpy(target,(char *)h->array + h->elemsize,h->elemsize);
}

long long HeapSize(heap* h){
	//return the heap size
	return h->logicalLen-1;
}