#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "AVLTree.h"

static AVLnode* MakeNode(AVLTree* y, void* data, void* key, int height, AVLnode* right, AVLnode* left){
	//create the node
	AVLnode* x = malloc(sizeof(AVLnode));
	assert(x != NULL);

	//create the data and key space
	x->data = malloc(y->elemsize);
	assert(x->data != NULL);

	x->key = malloc(y->keysize);
	assert(x->key != NULL);

	x->height = height;
	//set the data and the key
	memcpy(x->data,data,y->elemsize);
	memcpy(x->key,key,y->keysize);
	//set pointers to the parent,right and left
	x->right = right;
	x->left = left;
	//return the node
	return x;
}

static AVLnodeparent* AVLNodeGetMax(AVLnode* x, AVLStack* y){

	//if the tree is empty
	if(x == NULL)
		return NULL;

	AVLnode* root = x;
	AVLnode* parent = NULL;

	while(1){
		if(y != NULL)
			y->Push(y,&root);

		//if the root is the wanted one
		if(root->right == NULL){
			
			AVLnodeparent* toReturn = malloc(sizeof(AVLnodeparent));
			assert(toReturn != NULL);

			toReturn->node = root;
			toReturn->parent = parent;

			return toReturn;
		}

		//move the next right element
		parent = root;
		root = root->right;
	}	
}

static inline AVLnode* AVLNodeClone(AVLTree* y, AVLnode* x){
	return MakeNode(y,x->data,x->key,x->height,x->right,x->left);
}

static AVLnodeparent* AVLNodeGetMin(AVLnode* x, AVLStack* y){

	//if the tree is empty
	if(x == NULL)
		return NULL;

	AVLnode* root = x;
	AVLnode* parent = NULL;

	while(1){

		if(y != NULL)
			y->Push(y,&root);

		//if the root is the wanted one
		if(root->left == NULL){

			AVLnodeparent* toReturn = malloc(sizeof(AVLnodeparent));
			assert(toReturn != NULL);

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
static int AVLNodeSearch(AVLTree* x, void* key, AVLStack* stck){

	if(x->root == NULL)
		return 0;

	AVLnode* root = x->root;

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

static void FreeNode(AVLTree* y, AVLnode* x){

	if(y->datafreeFN != NULL)
		y->datafreeFN(x->data);
	if(y->keyfreeFN != NULL)
		y->keyfreeFN(x->key);

	free(x->data);
	free(x->key);
	free(x);
}

static void ReplaceNode(AVLTree* z, AVLnode* x, AVLnode* xParent, AVLnode* y){

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

static inline void AVLGrowStack(AVLStack* x){

	x->actuallength = x->actuallength * 2;

	x->stack = realloc(x->stack, x->actuallength * x->elemsize);

	assert(x->stack != NULL);
}

static void AVLPush(void* y,void* elementAddress){

	AVLStack* x = (AVLStack*) y;

	if(x->size == x->actuallength){
		AVLGrowStack(x);
	}

	memcpy((char *) x->stack + x->size * x->elemsize,
	elementAddress, x->elemsize);
	x->size++;
}

static int AVLPop(void* y,void* targetAddress){

	AVLStack* x = (AVLStack*) y;

	if(x->size == 0)
		return 0;

	memcpy(targetAddress, 
		(char *) x->stack + (x->size-1) * x->elemsize, x->elemsize);

	x->size--;
	return 1;
}

static int AVLPeek(void* y,void* targetAddress){

	AVLStack* x = (AVLStack*) y;

	if(x->size == 0)
		return 0;

	memcpy(targetAddress, 
		(char *) x->stack + (x->size-1) * x->elemsize, x->elemsize);
	return 1;
}

static inline void AVLStackFree(void* x){
	free(((AVLStack*)x)->stack);
}

static inline int max(int f1 , int f2){
	return f1>f2?f1:f2;
}

static inline int height(AVLnode* p)
{
    return p?p->height:-1;
}

static inline int AVLBalanceFactor(AVLnode* currNode){
	return height(currNode->right) - height(currNode->left);
}

static void AVLLeftRotaion(AVLTree* tree, AVLnode* currNode, AVLnode* parent){
	
	if(parent == NULL)
		tree->root = currNode->right;
	else
		if(parent->right == currNode)
			parent->right = currNode->right;
		else
			parent->left = currNode->right;

	AVLnode* temp = currNode->right->left;
	currNode->right->left = currNode;
	currNode->right = temp;

}

static void AVLRightRotaion(AVLTree* tree, AVLnode* currNode, AVLnode* parent){
	
	if(parent == NULL)
		tree->root = currNode->left;
	else
		if(parent->right == currNode)
			parent->right = currNode->left;
		else
			parent->left = currNode->left;


	AVLnode* temp = currNode->left->right;
	currNode->left->right = currNode;
	currNode->left = temp;
}

static inline void AVLFixHeight(AVLnode* node){
	node->height = max(height(node->right),
	height(node->left)) + 1;
}

static void AVLFix(AVLTree* x, AVLStack* y){
	AVLnode* currNode = NULL;
	while(y->Pop(y,&currNode)){
		
		//fix height
		AVLFixHeight(currNode);

		//rotations
		//printf("AVL Balance Factor Before : %d\n", AVLBalanceFactor(currNode));
		if(AVLBalanceFactor(currNode) == 2){

			AVLnode* parent = NULL;
			y->Peek(y,&parent);
			
			AVLnode* fixHeight1 = currNode;
			AVLnode* fixHeight2 = NULL;
			AVLnode* fixHeight3 = NULL;

			if(AVLBalanceFactor(currNode->right) >= 0){
				fixHeight2 = currNode->right->right;
				fixHeight3 = currNode->right;
				AVLLeftRotaion(x,currNode,parent);
			}else{
				fixHeight2 = currNode->right;
				fixHeight3 = currNode->right->left;
				AVLRightRotaion(x,currNode->right,currNode);
				AVLLeftRotaion(x,currNode,parent);
			}
				
			AVLFixHeight(fixHeight2);
			AVLFixHeight(fixHeight1);
			AVLFixHeight(fixHeight3);

		}else if(AVLBalanceFactor(currNode) == -2){

			AVLnode* parent = NULL;
			y->Peek(y,&parent);

			AVLnode* fixHeight1 = currNode;
			AVLnode* fixHeight2 = NULL;
			AVLnode* fixHeight3 = NULL;

			if(AVLBalanceFactor(currNode->left) <= 0){
				fixHeight2 = currNode->left->left;
				fixHeight3 = currNode->left;
				AVLRightRotaion(x,currNode,parent);
			}else{
				fixHeight2 = currNode->left;
				fixHeight3 = currNode->left->right;
				AVLLeftRotaion(x,currNode->left,currNode);
				AVLRightRotaion(x,currNode,parent);
			}

			AVLFixHeight(fixHeight2);
			AVLFixHeight(fixHeight1);
			AVLFixHeight(fixHeight3);
		}
		//printf("AVL Balance Factor After : %d\n", AVLBalanceFactor(currNode));
	}
}


static void AVLInitializeStack(AVLStack* x,int elemsize){
	x->elemsize = elemsize;
	x->size = 0;
	x->actuallength = 4;
	x->stack = malloc(elemsize * 4);
	assert(x->stack != NULL);
	
	x->Push = AVLPush;
	x->Pop = AVLPop;
	x->Peek = AVLPeek;
	x->Free = AVLStackFree;
}

void InitializeAVLTree(AVLTree* x, int dataSize, int keySize, CompareFunction CmpFN,
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

void DisposeAVLTree(AVLTree* x){
	
	AVLnode* currNode = x->root;

	AVLStack parents;
	AVLInitializeStack(&parents,sizeof(AVLnode *));

	while(currNode != NULL){

		if(currNode->left != NULL){
			parents.Push(&parents,&currNode);
			AVLnode* temp = currNode->left;
			currNode->left = NULL;
			currNode = temp;
			continue;
		}

		if(currNode->right != NULL){
			parents.Push(&parents,&currNode);
			AVLnode* temp = currNode->right;
			currNode->right = NULL;
			currNode = temp;
			continue;
		}

		FreeNode(x,currNode);

		AVLnode* temp;
		if(parents.Pop(&parents,&temp))
			currNode = temp;
		else
			currNode = NULL;
	}

	parents.Free(&parents);
}

int AVLTreeAddKey(AVLTree* x, void* key, void* elementAddress){

	x->size++;

	if(x->root == NULL){
		x->root = MakeNode(x,elementAddress,key,0,NULL,NULL);
		return 1;
	}

	AVLnode* root = x->root;

	AVLStack Route;
	AVLInitializeStack(&Route,sizeof(AVLnode*));

	while(1){

		//if the root is the wanted one
		if(x->CmpFN(key,root->key) == 1){
			memcpy(root->data, elementAddress, x->elemsize);
			return 0;
		}

		Route.Push(&Route,&root);

		//change the current root and loop again
		if(!x->CmpFN(key,root->key)){
			if(root->left == NULL){				
				root->left = MakeNode(x,elementAddress,key,0,NULL,NULL);
				
				AVLFix(x,&Route);
				return 1;
			}
			root = root->left;
		}else{
			if(root->right == NULL){
				root->right = MakeNode(x,elementAddress,key,0,NULL,NULL);
				
				AVLFix(x,&Route);
				return 1;
			}
			root = root->right;
		}
	}

}

int AVLTreeSearch(AVLTree* x, void* key, void* targetAddress){
	
	AVLnode* root = x->root;
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

int AVLTreeDelete(AVLTree* x, void* key){
	
	if(x->root == NULL)
		return 0;

	AVLnode* parent = NULL;
	AVLnode* root = x->root;

	AVLStack Route;
	AVLInitializeStack(&Route,sizeof(AVLnode*));
	
	while(1){

		//we didn't find the key
		if(root == NULL)
			return 0;

		Route.Push(&Route,&root);

		//if the root is the wanted one
		if(x->CmpFN(key,root->key) == 1){
			Route.Pop(&Route,&root);

			if(root->right == NULL){

				if(root->left != NULL)
					Route.Push(&Route,&(root->left));

				ReplaceNode(x,root,parent,root->left);
				AVLFix(x,&Route);
			}else if(root->left == NULL){

				if(root->right != NULL)
					Route.Push(&Route,&(root->right));

				ReplaceNode(x,root,parent,root->right);
				AVLFix(x,&Route);
			}else{

				int currSize = Route.size;

				AVLnode* temp_3 = malloc(sizeof(AVLnode));
				assert(temp_3 != NULL);

				Route.Push(&Route,&temp_3);
				free(temp_3);

				AVLnodeparent* successor = AVLNodeGetMin(root->right,&Route);

				Route.Pop(&Route,&temp_3);
				memmove((char *)(Route.stack) + currSize * Route.elemsize,
						&temp_3, Route.elemsize);

				if(successor->node != root->right){

					AVLnode* temp_2;

					AVLnode* temp = AVLNodeClone(x,successor->node);

					memmove((char *)(Route.stack) + currSize * Route.elemsize,
						&temp, Route.elemsize);

					ReplaceNode(x,successor->node,successor->parent,successor->node->right);

					successor->node = temp;
					successor->node->right = root->right;
				}

				successor->node->left = root->left;
				ReplaceNode(x,root,parent,successor->node);
				free(successor);

				AVLFix(x,&Route);
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

int AVLTreeGetSuccessor(AVLTree* x, void* key, void* targetAddress){

	//if the tree is empty
	if(x->root == NULL)
		return 0;

	AVLStack Route;
	AVLInitializeStack(&Route,sizeof(AVLnode *));

	int toReturn  = AVLNodeSearch(x,key,&Route);

	AVLnode* node = NULL;
	Route.Pop(&Route,&node);

	//if the root is the wanted one
	if(node->right != NULL){

		AVLnodeparent* data = AVLNodeGetMin(node->right,NULL);
		AVLnode* successor = data->node;

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

	AVLnode* parent = NULL;
	Route.Pop(&Route,&parent);

	while(parent != NULL && parent->right == node){
		node = parent;

		if(!Route.Pop(&Route,&parent))
			parent = NULL;
	}

	if(parent == NULL){
		AVLTreeGetMax(x,targetAddress);

		Route.Free(&Route);
		return toReturn;
	}

	memcpy(targetAddress,parent->data,x->elemsize);
	Route.Free(&Route);
	return toReturn;
}

int AVLTreeGetPredecessor(AVLTree* x, void* key, void* targetAddress){

	//if the tree is empty
	if(x->root == NULL)
		return 0;

	AVLStack Route;
	AVLInitializeStack(&Route,sizeof(AVLnode *));

	int toReturn  = AVLNodeSearch(x,key,&Route);

	AVLnode* node = NULL;
	Route.Pop(&Route,&node);

	//if the root is the wanted one
	if(node->left != NULL){

		AVLnodeparent* data = AVLNodeGetMax(node->left,NULL);
		AVLnode* successor = data->node;

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

	AVLnode* parent = NULL;
	Route.Pop(&Route,&parent);

	while(parent != NULL && parent->left == node){
		node = parent;

		if(!Route.Pop(&Route,&parent))
			parent = NULL;
	}

	if(parent == NULL){
		AVLTreeGetMin(x,targetAddress);

		Route.Free(&Route);
		return toReturn;
	}

	memcpy(targetAddress,parent->data,x->elemsize);
	Route.Free(&Route);
	return toReturn;
}

int AVLTreeGetMax(AVLTree* x, void* targetAddress){

	//if the tree is empty
	if(x->root == NULL)
		return 0;

	AVLnode* root = x->root;
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

int AVLTreeGetMin(AVLTree* x, void* targetAddress){

	//if the tree is empty
	if(x->root == NULL)
		return 0;

	AVLnode* root = x->root;
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

void* AVLGetIterator(AVLTree* x){
	//mallocate Iterator and initalize it
	AVLIterator* y = malloc(sizeof(AVLIterator));
	assert(y != NULL);
	
	y->node = x->root;
	y->tree = x;
	y->stack = malloc(sizeof(AVLStack));
	assert(y->stack != NULL);

	AVLInitializeStack((y->stack),sizeof(AVLnode *));

	while(y->node != NULL){
		if(y->node->left != NULL){
			y->stack->Push(y->stack,&(y->node));
			y->node = y->node->left;
		}else
			break;
	}

	return y;	//return pointer to the iterator
}

void AVLIteratorGetCurrent(void* Iterator, void* targetAddress){
	//cast and get the current element's data
	AVLIterator* y = (AVLIterator *) Iterator;
	memcpy(targetAddress,y->node->data,y->tree->elemsize);
}

int AVLIteratorHasNext(void* Iterator){
	
	//cast and check if it has next node
	AVLIterator* y = (AVLIterator *) Iterator;
	if(y->node->right != NULL || y->stack->size != 0)
		return 1;

	return 0;
}

void AVLIteratorGoNext(void* Iterator){
	//cast and move the pointer to the next node
	AVLIterator* y = (AVLIterator *) Iterator;
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

void AVLIteratorDispose(void* Iterator){
	
	//free the iterator
	AVLIterator* y = (AVLIterator *) Iterator;

	y->stack->Free(y->stack);
	free(y->stack);
	free(y);
}