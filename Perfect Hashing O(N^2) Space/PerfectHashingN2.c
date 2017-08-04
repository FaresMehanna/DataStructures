#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "PerfectHashingN2.h"

static inline unsigned int getHash(void* key, int keysize, long long max, long long a, long long b){

	unsigned int hash = 2166136261;

	for(int i=0;i<keysize;i++){
		hash = hash ^ (*(char *)key + i);
 		hash = hash * 16777619;
 	}

	return ((((((hash % max) * a) % max ) + b) % max) + a) % max;
}

static inline void collisionSolveN2(PerfectHashSetN2* x){

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

		printf("N2 COLLISION SOLVER , SIZE %lld , ActualSize %lld , C1 %d , C2 %d\n", x->size,x->actualsize
			,c1,c2);


		//pointer to the array
		DataKey** arr = x->data;

		//create new array with the same size
		DataKey** new = calloc(x->actualsize,sizeof(DataKey*));
		assert(new != NULL);

		//rehash everything
		for(int i=0;i<x->actualsize;i++){
			
			if(arr[i] == NULL)
				continue;

			unsigned int hash = x->getHash(arr[i]->key,x->keysize,x->actualsize,c1,c2);
			//printf("%d - %lld\n",hash,(long long)new[hash]);
			//collision
			if(new[hash] != NULL){
				//printf("N2 SHIT\n");
				//redo everything
				repeat = 1;
				//free the data
				free(new);
				//break;
				break;
			}

			//else set the data
			new[hash] = arr[i];
		}

		if(!repeat){
			free(x->data);
			x->data = new;
		}

	}

}

static inline DataKey* createDataKey(PerfectHashSetN2* x ,void* key ,void* elementAddress){
	//create the pair
	DataKey* y = malloc(sizeof(DataKey));
	assert(y != NULL);

	y->data = malloc(x->elemsize);
	assert(y->data != NULL);

	y->key = malloc(x->keysize);
	assert(y->key != NULL);

	//set the data
	memcpy (y->data,elementAddress,x->elemsize);
	memcpy (y->key,key,x->keysize);
	//return
	return y;
}

static inline void userDeleteDataKeyPair(PerfectHashSetN2* x ,DataKey* y){

	if(x->datafreeFN != NULL)
		x->datafreeFN(y->data);
	if(x->keyfreeFN != NULL)
		x->keyfreeFN(y->key);

}

static inline void DeleteKeyPair(PerfectHashSetN2* x ,DataKey* y){
	free(y->data);
	free(y->key);
	free(y);
}


static inline void FullDeleteKeyPair(PerfectHashSetN2* x ,DataKey* y){
	userDeleteDataKeyPair(x,y);
	DeleteKeyPair(x,y);
}



static inline void GrowPerfectHashSetN2(PerfectHashSetN2* x){
	
	//if the sizes aren't the same return
	if(x->actualsize >= (x->size+1)*(x->size+1))
		return;

	printf("N2 GROW HASH , SIZE %lld , ActualSize %lld , NewActualSize %lld, C1 %lld , C2 %lld\n",
		 x->size,x->actualsize,x->actualsize*4,x->c1,x->c2);

	//grow the array
	x->actualsize =4 * x->actualsize;

	//pointe to the old array
	DataKey** old = x->data;
	int oldSize = x->actualsize/4;

	//create new array
	x->data = calloc(x->actualsize,sizeof(DataKey*));
	DataKey** new = x->data;
	assert(x->data != NULL);

	for(int i=0;i<oldSize;i++){
		
		if(old[i] == NULL)
			continue;

		unsigned int hash = x->getHash(old[i]->key,x->keysize,x->actualsize,x->c1,x->c2);

		while(new[hash] != NULL){
			//printf("COLLISON\n");
			//there is collision
			collisionSolveN2(x);
			hash = x->getHash(old[i]->key,x->keysize,x->actualsize,x->c1,x->c2);
		}

		new[hash] = old[i];
	}

	//free the old array
	free(old);
}

void InitializePerfectHashSetN2(PerfectHashSetN2* x, int dataSize, int keySize, CompareFunction CmpFN,
 FreeFunction datafreeFN, FreeFunction keyfreeFN, Hash hashFn){

	//initialize the data in the tree
	x->size = 0;
	x->actualsize = 8;
	x->elemsize = dataSize;
	x->keysize = keySize;
	
	//the array holding the pointers to the datakey pair
	x->data = calloc(x->actualsize,sizeof(DataKey*));
	assert(x->data != NULL);

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

void DisposePerfectHashSetN2(PerfectHashSetN2* x, int full){

	//loop through the data array
	for(int i=0;i<x->actualsize;i++){
		if((x->data)[i] != NULL){
			if(full){
				FullDeleteKeyPair(x,(x->data)[i]);
			}else{
				DeleteKeyPair(x,(x->data)[i]);
			}
		}
	}
	
	//free the array of pointers
	free(x->data);
}

//will return 1 incase the add succ
//will return 0 incase the add failed and updated instead
int PerfectHashSetN2AddKey(PerfectHashSetN2* x, void* key, void* elementAddress){
	//grow the set if needed
	GrowPerfectHashSetN2(x);

	//get the hash value
	unsigned int hash = x->getHash(key,x->keysize,x->actualsize,x->c1,x->c2);
	//printf("DATA %u\n", hash);
	//create datakey pair
	DataKey* y = createDataKey(x,key,elementAddress);

	//check for existing
	if((x->data)[hash] != NULL){
		//check for dublicated
		if(x->CmpFN(key,(x->data)[hash]->key)){
			FullDeleteKeyPair(x,(x->data)[hash]);
			(x->data)[hash] = y;
			return 0;
		}else{
			while((x->data)[hash] != NULL){
				//there is collision
				//printf("N2 Add Key Collision + %d + %lld\n", hash,(long long)(x->data)[hash]);
				collisionSolveN2(x);
				hash = x->getHash(key,x->keysize,x->actualsize,x->c1,x->c2);
			}
		}
	}

	(x->data)[hash] = y;
	x->size++;
	return 1;
}


int PerfectHashSetN2Search(PerfectHashSetN2* x, void* key, void* targetAddress){
	
	//get the hash value
	unsigned int hash = x->getHash(key,x->keysize,x->actualsize,x->c1,x->c2);
	//check for existing
	if((x->data)[hash] != NULL){
		//check for valid key
		if(x->CmpFN(key,(x->data)[hash]->key)){
			memcpy(targetAddress,(x->data)[hash]->data,x->elemsize);
			return 1;
		}
	}

	return 0;
}

int PerfectHashSetN2Delete(PerfectHashSetN2* x, void* key){

	//get the hash value
	unsigned int hash = x->getHash(key,x->keysize,x->actualsize,x->c1,x->c2);

	//check for existing
	if((x->data)[hash] != NULL){
		//check for valid key
		if(x->CmpFN(key,(x->data)[hash]->key)){
			FullDeleteKeyPair(x,(x->data)[hash]);
			(x->data)[hash] = NULL;
			x->size--;
			return 1;
		}
	}

	return 0;
}

long long PerfectHashSetN2GetSize(PerfectHashSetN2* x){
	return x->size;
}