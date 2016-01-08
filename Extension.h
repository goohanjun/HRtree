/*
 * Extension.h
 *
 *  Created on: Sep 10, 2015
 *      Author: hanjunkoo
 */

#ifndef SRC_EXTENSION_H_
#define SRC_EXTENSION_H_

#include "HNode.h"
#include "hrext.h"
#include "RootTable.h"
#include "pCursor.h"

#include <algorithm>
#include <iostream>
#include <string.h>

using namespace std;

int Insert(HNode* root, double* key, int data, int dlen, RootTable* RT);
int Delete(HNode* root, double* key, int data, int dlen, RootTable* RT);
int Search(HNode* root, double* key, int data, int dlen, HNode* Root[], int & numRoot);

// Insertion
HNode* _ChooseSubtree(HNode* self, stack* st, double* key, int data);
int _TreatOverflow(HNode* self, stack* Stack, HNode*& nNode1, HNode*& nNode2, int& status, RootTable* RT, double tnow);

// Deletion
bool _FindLeaf(HNode* Node,HNode *&leaf,stack *Stack, double* key,int data);
int _TreatUnderflow();
int _Deletion_ApplyChanges();
int _Merge();

// General
bool _isKeySplit(pCursor *cursor, double currentTime);
void _RemoveEntry(HNode *self , stack *Stack, double tnow);

//key Split
void _keySplit(pCursor *Cursor, int rightE[], int& nR, double* rkey, double* lkey);
void _keySplitNode(HNode* self, pCursor *cursor, int rightE[], int numRight, HNode*& newNode, double *rkey, double *lkey);

//version Split
void _versionSplitNode(RootTable* RT, pCursor *cursor, HNode* self, HNode*& newNode,double currentTime);

//SVO
void _SVOSplitNode(RootTable* RT, pCursor *cursor, HNode* self, HNode*& newNode1,HNode*& newNode2,double currentTime);
void _keySplitSVO(RootTable* RT, HNode* self, pCursor *cursor, int rightE[], int numRight, HNode*& newNode1,HNode*& newNode2, double *rkey, double *lkey);

//Search
int _Consistent(double *bp, double *key);

//Debug
void _printStack(stack *st);
void _printNode(HNode* HNode);

//Deprecated
int Insert2(HNode* root, double* key,  int data, int dlen, HNode* Root[], int & numRoot);
int _Path(HNode* self, stack* st, double* key);
int _Insertion_ApplyChanges();

#endif /* SRC_EXTENSION_H_ */
