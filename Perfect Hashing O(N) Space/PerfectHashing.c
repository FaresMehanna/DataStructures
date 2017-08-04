#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "PerfectHashing.h"
#include "PerfectHashingN2.c"


static inline void collisionSolve(PerfectHashSet* x){


	int repeat = 1;

	while(repeat){

		//ending condition
		repeat = 0;

		//generate new constant numbers
		int c1 = rand();
		int c2 = rand();

		if(x->c1 == c1 && x->c2 == c2){
			c1 = (unsigned long long) c1 * c1 % c2;
			c2 = (unsigned long long) c2 * c2 % c1;
		}

		x->c1 = c1;
		x->c2 = c2;

		printf("%d\n", x->limit);
		printf("COLLISION SOLVER , SIZE %lld , ActualSize %lld , C1 %d , C2 %d\n\n", x->size,x->actualsize
			,c1,c2);

		//pointe to the old array
		PerfectHashSetN2** old = x->data;

		//create new array
		PerfectHashSetN2** new = malloc(x->actualsize * sizeof(PerfectHashSetN2*));
		assert(new != NULL);

		for(int i=0;i<x->actualsize;i++){
			(new)[i] = malloc(sizeof(PerfectHashSetN2));
			assert(new[i] != NULL);

			InitializePerfectHashSetN2((new)[i],x->elemsize,x->keysize,x->CmpFN,x->datafreeFN,x->keyfreeFN,x->getHash);
		}


		for(int i=0;i<x->actualsize;i++){
			
			//loop throught the small N2 one
			int currSize = old[i]->actualsize;
			for(int j=0;j<currSize;j++){

				if(old[i]->data[j] == NULL)
					continue;
			
				DataKey* curr = old[i]->data[j];

				unsigned int hash = x->getHash(curr->key,x->keysize,x->actualsize,x->c1,x->c2);
				
				if(PerfectHashSetN2GetSize(new[hash]) == x->limit-1){
					
					repeat = 1;
					
					for(int i=0;i<x->actualsize;i++){
						DisposePerfectHashSetN2((new)[i],0);
						free((new)[i]);
					}

					free(new);
					break;

				}

				//add the key
				PerfectHashSetN2AddKey(new[hash],curr->key,curr->data);
			}

			if(repeat)
				break;
		}

		if(!repeat){

			for(int i=0;i<x->actualsize;i++){
				DisposePerfectHashSetN2(old[i],0);
				free(old[i]);
			}

			free(x->data);
			x->data = new;
		}

	}

}

static inline void updateLimit(PerfectHashSet* x){
	x->limit = x->size/500 > x->limit ? x->size/500: x->limit;
}

static inline void GrowPerfectHashSet(PerfectHashSet* x){
	
	//if the sizes aren't the same return
	if(x->actualsize !=  x->size)
		return;

	printf("GROW HASH , SIZE %lld , ActualSize %lld , NewActualSize %lld, C1 %lld , C2 %lld\n",
		 x->size,x->actualsize,x->actualsize*2,x->c1,x->c2);

	long long cols = 0;
	for(int i=0;i<x->actualsize;i++){
		cols += x->data[i]->cols;
	}

	printf("GROW HASH , Cols : %lld\n\n",cols);
	//grow the array
	x->actualsize = 2 * x->actualsize;

	//pointe to the old array
	PerfectHashSetN2** old = x->data;
	int oldSize = x->actualsize/4;

	//create new array
	x->data = malloc(x->actualsize * sizeof(PerfectHashSetN2*));
	assert(x->data != NULL);
	PerfectHashSetN2** new = x->data;

	for(int i=0;i<x->actualsize;i++){
		(x->data)[i] = malloc(sizeof(PerfectHashSetN2));
		assert((x->data)[i] != NULL);
	}
	for(int i=0;i<x->actualsize;i++)
		InitializePerfectHashSetN2((x->data)[i],x->elemsize,x->keysize,x->CmpFN,x->datafreeFN,x->keyfreeFN,x->getHash);


	for(int i=0;i<oldSize;i++){
		
		//loop throught the small N2 one
		int currSize = old[i]->actualsize;
		for(int j=0;j<currSize;j++){

			if(old[i]->data[j] == NULL)
				continue;
		
			DataKey* curr = old[i]->data[j];

			unsigned int hash = x->getHash(curr->key,x->keysize,x->actualsize,x->c1,x->c2);
			
			while(PerfectHashSetN2GetSize(new[hash]) == x->limit){
				collisionSolve(x);
				hash = x->getHash(curr->key,x->keysize,x->actualsize,x->c1,x->c2);
			}

			//add the key
			PerfectHashSetN2AddKey(new[hash],curr->key,curr->data);

		}
		DisposePerfectHashSetN2(old[i],0);
		free(old[i]);
	}

	//free the old array
	free(old);
}

void InitializePerfectHashSet(PerfectHashSet* x, int dataSize, int keySize, CompareFunction CmpFN,
 FreeFunction datafreeFN, FreeFunction keyfreeFN, Hash hashFn){

	//initialize the data in the tree
	x->size = 0;
	x->actualsize = 8;
	x->elemsize = dataSize;
	x->keysize = keySize;
	x->limit = 5;

	//the array holding the pointers to the datakey pair
	x->data = malloc(x->actualsize * sizeof(PerfectHashSetN2*));
	assert(x->data != NULL);

	for(int i=0;i<x->actualsize;i++){
		(x->data)[i] = malloc(sizeof(PerfectHashSetN2));
		assert((x->data)[i] != NULL);

	}
	for(int i=0;i<x->actualsize;i++)
		InitializePerfectHashSetN2((x->data)[i],dataSize,keySize,CmpFN,datafreeFN,keyfreeFN,hashFn);

	x->datafreeFN = datafreeFN;
	x->keyfreeFN = keyfreeFN;
	x->CmpFN = CmpFN;
	x->getHash = getHash;

	//generate new constant numbers
	srand(time(NULL));
	x->c1 = rand();
	x->c2 = rand();

	if(hashFn != NULL)
		x->getHash = hashFn;
}

void DisposePerfectHashSet(PerfectHashSet* x){

	long long cols = 0;
	for(int i=0;i<x->actualsize;i++){
		cols += x->data[i]->cols;
	}
	printf("COLLISIONS : %lld\n", cols);

	//loop through the data array
	for(int i=0;i<x->actualsize;i++){
		DisposePerfectHashSetN2((x->data)[i],1);
		free((x->data)[i]);
	}
	
	//free the array of pointers
	free(x->data);
}

//will return 1 incase the add succ
//will return 0 incase the add failed and updated instead
int PerfectHashSetAddKey(PerfectHashSet* x, void* key, void* elementAddress){
	//grow the set if needed
	GrowPerfectHashSet(x);
	updateLimit(x);

	//get the hash value
	unsigned int hash = x->getHash(key,x->keysize,x->actualsize,x->c1,x->c2);

	//check for the size
	if(PerfectHashSetN2GetSize(x->data[hash]) == x->limit){
		collisionSolve(x);
		hash = x->getHash(key,x->keysize,x->actualsize,x->c1,x->c2);
	}

	//add the key
	if(PerfectHashSetN2AddKey((x->data)[hash],key,elementAddress))
		x->size++;
	return 1;
}


int PerfectHashSetSearch(PerfectHashSet* x, void* key, void* targetAddress){
	
	//get the hash value
	unsigned int hash = x->getHash(key,x->keysize,x->actualsize,x->c1,x->c2);
	//check for existing
	return PerfectHashSetN2Search((x->data)[hash], key, targetAddress);

}

int PerfectHashSetDelete(PerfectHashSet* x, void* key){
	
	updateLimit(x);

	//get the hash value
	unsigned int hash = x->getHash(key,x->keysize,x->actualsize,x->c1,x->c2);
	
	x->size--;
	return PerfectHashSetN2Delete((x->data)[hash], key);
}
