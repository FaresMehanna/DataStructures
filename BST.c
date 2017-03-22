#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "BST.h"


static BSTnode* MakeNode(BSTTree* y, void* data, void* key, BSTnode* right, BSTnode* left){
	//create the node
	BSTnode* x = malloc(sizeof(BSTnode));
	//create the data and key space
	x->data = malloc(y->elemsize);
	x->key = malloc(y->keysize);
	//set the data and the key
	memcpy(x->data,data,y->elemsize);
	memcpy(x->key,key,y->keysize);
	//set pointers to the parent,right and left
	x->right = right;
	x->left = left;
	//return the node
	return x;
}

static BSTnodeparent* BSTNodeGetMax(BSTnode* x){

	//if the tree is empty
	if(x == NULL)
		return NULL;

	BSTnode* root = x;
	BSTnode* parent = NULL;

	while(1){

		//if the root is the wanted one
		if(root->right == NULL){
			
			BSTnodeparent* toReturn = malloc(sizeof(BSTnodeparent));
			toReturn->node = root;
			toReturn->parent = parent;

			return toReturn;
		}

		//move the next right element
		root = root->right;
	}	
}

static BSTnode* BSTNodeClone(BSTTree* y, BSTnode* x){
	return MakeNode(y,x->data,x->key,x->right,x->left);
}

static BSTnodeparent* BSTNodeGetMin(BSTnode* x){

	//if the tree is empty
	if(x == NULL)
		return NULL;

	BSTnode* root = x;
	BSTnode* parent = NULL;

	while(1){

		//if the root is the wanted one
		if(root->left == NULL){

			BSTnodeparent* toReturn = malloc(sizeof(BSTnodeparent));
			toReturn->node = root;
			toReturn->parent = parent;

			return toReturn;
		}

		//move the next right element
		parent = root;
		root = root->left;
	}	
}


//this will return the node if found or the closest Node
static int BSTNodeSearch(BSTTree* x, void* key, BSTStack* stck){

	if(x->root == NULL)
		return 0;

	BSTnode* root = x->root;

	while(1){

		//key found OR not found
		if(root == NULL)
			return 0;

		if(x->CmpFN(key,root->key) == 1){
			stck->Push(stck,&root);
			return 1;
		}

		//add to the stack
		stck->Push(stck,&root);

		//change the current root and loop again
		if(!x->CmpFN(key,root->key))
			root = root->left;
		else
			root = root->right;
	}
}

static void FreeNode(BSTTree* y, BSTnode* x){

	if(y->datafreeFN != NULL)
		y->datafreeFN(x->data);
	if(y->keyfreeFN != NULL)
		y->keyfreeFN(x->key);

	free(x->data);
	free(x->key);
	free(x);
}

static void ReplaceNode(BSTTree* z, BSTnode* x, BSTnode* xParent, BSTnode* y){

	if(x == NULL)
		return;

	if(xParent == NULL)
		z->root = y;
	else if(xParent->right == x)
		xParent->right = y;
	else if(xParent->left == x)
		xParent->left = y;

	FreeNode(z,x);
}

static void BSTGrowStack(BSTStack* x){

	x->actuallength = x->actuallength * 2;

	x->stack = realloc(x->stack, x->actuallength * x->elemsize);

	assert(x->stack != NULL);
}

static void BSTPush(void* y,void* elementAddress){

	BSTStack* x = (BSTStack*) y;

	if(x->size == x->actuallength){
		BSTGrowStack(x);
	}

	memcpy((char *) x->stack + x->size * x->elemsize,
	elementAddress, x->elemsize);
	x->size++;
}

static int BSTPop(void* y,void* targetAddress){

	BSTStack* x = (BSTStack*) y;

	if(x->size == 0)
		return 0;

	memcpy(targetAddress, 
		(char *) x->stack + (x->size-1) * x->elemsize, x->elemsize);

	x->size--;
	return 1;
}

static void BSTStackFree(void* x){
	free(((BSTStack*)x)->stack);
}

static void BSTInitializeStack(BSTStack* x,int elemsize){
	x->elemsize = elemsize;
	x->size = 0;
	x->actuallength = 4;
	x->stack = malloc(elemsize * 4);
	x->Push = BSTPush;
	x->Pop = BSTPop;
	x->Free = BSTStackFree;
}

void InitializeBSTTree(BSTTree* x, int dataSize, int keySize, CompareFunction CmpFN,
 FreeFunction datafreeFN, FreeFunction keyfreeFN){

	//initialize the data in the tree
	x->size = 0;
	x->elemsize = dataSize;
	x->keysize = keySize;
	x->root = NULL;
	x->datafreeFN = datafreeFN;
	x->keyfreeFN = keyfreeFN;
	x->CmpFN = CmpFN;

}

void DisposeBSTTree(BSTTree* x){
	
	BSTnode* currNode = x->root;

	BSTStack parents;
	BSTInitializeStack(&parents,sizeof(BSTnode *));

	while(currNode != NULL){

		if(currNode->left != NULL){
			parents.Push(&parents,&currNode);
			BSTnode* temp = currNode->left;
			currNode->left = NULL;
			currNode = temp;
			continue;
		}

		if(currNode->right != NULL){
			parents.Push(&parents,&currNode);
			BSTnode* temp = currNode->right;
			currNode->right = NULL;
			currNode = temp;
			continue;
		}

		FreeNode(x,currNode);

		BSTnode* temp;
		if(parents.Pop(&parents,&temp))
			currNode = temp;
		else
			currNode = NULL;
	}

	parents.Free(&parents);
}

int BSTTreeAddKey(BSTTree* x, void* key, void* elementAddress){

	x->size++;

	if(x->root == NULL){
		x->root = MakeNode(x,elementAddress,key,NULL,NULL);
		return 1;
	}

	BSTnode* root = x->root;
	while(1){

		//if the root is the wanted one
		if(x->CmpFN(key,root->key) == 1){
			memcpy(root->data, elementAddress, x->elemsize);
			return 0;
		}

		//change the current root and loop again
		if(!x->CmpFN(key,root->key)){
			if(root->left == NULL){
				root->left = MakeNode(x,elementAddress,key,NULL,NULL);
				return 1;
			}
			root = root->left;
		}else{
			if(root->right == NULL){
				root->right = MakeNode(x,elementAddress,key,NULL,NULL);
				return 1;
			}
			root = root->right;
		}
	}

}

int BSTTreeSearch(BSTTree* x, void* key, void* targetAddress){
	
	BSTnode* root = x->root;
	while(1){

		//if the tree is empty
		if(root == NULL)
			return 0;

		//if the root is the wanted one
		if(x->CmpFN(key,root->key) == 1){
			memcpy(targetAddress, root->data, x->elemsize);
			return 1;
		}

		//change the current root and loop again
		if(!x->CmpFN(key,root->key))
			root = root->left;
		else
			root = root->right;
	}
}

int BSTTreeDelete(BSTTree* x, void* key){
	
	if(x->root == NULL)
		return 0;

	BSTnode* parent = NULL;
	BSTnode* root = x->root;

	
	while(1){

		//we didn't find the key
		if(root == NULL)
			return 0;

		//if the root is the wanted one
		if(x->CmpFN(key,root->key) == 1){
			
			if(root->right == NULL)
				ReplaceNode(x,root,parent,root->left);
			else if(root->left == NULL)
				ReplaceNode(x,root,parent,root->right);
			else{
				BSTnodeparent* successor = BSTNodeGetMin(root->right);


				if(successor->node != root->right){

					BSTnode* temp = BSTNodeClone(x,successor->node);
					ReplaceNode(x,successor->node,successor->parent,successor->node->right);

					successor->node = temp;
					successor->node->right = root->right;
				}

				successor->node->left = root->left;
				ReplaceNode(x,root,parent,successor->node);
				free(successor);
			}
			x->size--;
			return 1;
		}

		//change the current root and loop again
		parent = root;

		if(!x->CmpFN(key,root->key))
			root = root->left;
		else
			root = root->right;
	}
}

int BSTTreeGetSuccessor(BSTTree* x, void* key, void* targetAddress){

	//if the tree is empty
	if(x->root == NULL)
		return 0;

	BSTStack Route;
	BSTInitializeStack(&Route,sizeof(BSTnode *));

	int toReturn  = BSTNodeSearch(x,key,&Route);

	BSTnode* node = NULL;
	Route.Pop(&Route,&node);

	//if the root is the wanted one
	if(node->right != NULL){

		BSTnodeparent* data = BSTNodeGetMin(node->right);
		BSTnode* successor = data->node;

		memcpy(targetAddress,successor->data,x->elemsize);
		free(data);

		Route.Free(&Route);
		return toReturn;
	}

	//if the key is bigger
	if(!toReturn)
		if(!x->CmpFN(key,node->key)){
			memcpy(targetAddress,node->data,x->elemsize);

			Route.Free(&Route);
			return toReturn;
		}

	BSTnode* parent = NULL;
	Route.Pop(&Route,&parent);

	while(parent != NULL && parent->right == node){
		node = parent;

		if(!Route.Pop(&Route,&parent))
			parent = NULL;
	}

	if(parent == NULL){
		BSTTreeGetMax(x,targetAddress);

		Route.Free(&Route);
		return toReturn;
	}

	memcpy(targetAddress,parent->data,x->elemsize);
	Route.Free(&Route);
	return toReturn;
}

int BSTTreeGetPredecessor(BSTTree* x, void* key, void* targetAddress){

	//if the tree is empty
	if(x->root == NULL)
		return 0;

	BSTStack Route;
	BSTInitializeStack(&Route,sizeof(BSTnode *));

	int toReturn  = BSTNodeSearch(x,key,&Route);

	BSTnode* node = NULL;
	Route.Pop(&Route,&node);

	//if the root is the wanted one
	if(node->left != NULL){

		BSTnodeparent* data = BSTNodeGetMax(node->left);
		BSTnode* successor = data->node;

		memcpy(targetAddress,successor->data,x->elemsize);
		free(data);

		Route.Free(&Route);
		return toReturn;
	}

	//if the key is bigger
	if(!toReturn)
		if(!x->CmpFN(node->key,key)){
			memcpy(targetAddress,node->data,x->elemsize);

			Route.Free(&Route);
			return toReturn;
		}

	BSTnode* parent = NULL;
	Route.Pop(&Route,&parent);

	while(parent != NULL && parent->left == node){
		node = parent;

		if(!Route.Pop(&Route,&parent))
			parent = NULL;
	}

	if(parent == NULL){
		BSTTreeGetMin(x,targetAddress);

		Route.Free(&Route);
		return toReturn;
	}

	memcpy(targetAddress,parent->data,x->elemsize);
	Route.Free(&Route);
	return toReturn;
}

int BSTTreeGetMax(BSTTree* x, void* targetAddress){

	//if the tree is empty
	if(x->root == NULL)
		return 0;

	BSTnode* root = x->root;
	while(1){

		//if the root is the wanted one
		if(root->right == NULL){
			memcpy(targetAddress, root->data, x->elemsize);
			return 1;
		}

		//move the next right element
		root = root->right;
	}	
}

int BSTTreeGetMin(BSTTree* x, void* targetAddress){

	//if the tree is empty
	if(x->root == NULL)
		return 0;

	BSTnode* root = x->root;
	while(1){

		//if the root is the wanted one
		if(root->left == NULL){
			memcpy(targetAddress, root->data, x->elemsize);
			return 1;
		}

		//move the next right element
		root = root->left;
	}	
}

void* BSTGetIterator(BSTTree* x){
	//mallocate Iterator and initalize it
	BSTIterator* y = malloc(sizeof(BSTIterator));
	y->node = x->root;
	y->tree = x;
	y->stack = malloc(sizeof(BSTStack));

	BSTInitializeStack((y->stack),sizeof(BSTnode *));

	while(y->node != NULL){
		if(y->node->left != NULL){
			y->stack->Push(y->stack,&(y->node));
			y->node = y->node->left;
		}else
			break;
	}

	return y;	//return pointer to the iterator
}

void BSTIteratorGetCurrent(void* Iterator, void* targetAddress){
	//cast and get the current element's data
	BSTIterator* y = (BSTIterator *) Iterator;
	memcpy(targetAddress,y->node->data,y->tree->elemsize);
}

int BSTIteratorHasNext(void* Iterator){
	
	//cast and check if it has next node
	BSTIterator* y = (BSTIterator *) Iterator;
	if(y->node->right != NULL || y->stack->size != 0)
		return 1;

	return 0;
}

void BSTIteratorGoNext(void* Iterator){
	//cast and move the pointer to the next node
	BSTIterator* y = (BSTIterator *) Iterator;
	if(y->node->right != NULL){
		y->node = y->node->right;

		while(y->node != NULL){
			if(y->node->left != NULL){
				y->stack->Push(y->stack,&(y->node));
				y->node = y->node->left;
			}else
				break;
		}

	}else if(y->stack->size != 0){
		y->stack->Pop(y->stack,&(y->node));
	}
}

void BSTIteratorDispose(void* Iterator){
	
	//free the iterator
	BSTIterator* y = (BSTIterator *) Iterator;

	y->stack->Free(y->stack);
	free(y->stack);
	free(y);
}