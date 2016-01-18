/*
 * Extensions.h
 *
 *  Created on: Jan 16, 2016
 *      Author: hanjunkoo
 */

#ifndef SRC_EXTENSIONS_H_
#define SRC_EXTENSIONS_H_

#include "HNode.h"
#include "hrext.h"
#include "RootTable.h"
#include "pCursor.h"

#include <algorithm>
#include <iostream>
#include <string.h>

using namespace std;

class RootTable;
class pCursor;

class Extensions {
public:
	Extensions();
	virtual ~Extensions();

	int Insert(HNode* root, double* key, int data, int dlen, RootTable* RT);
	int Delete(HNode* root, double* key, int data, int dlen, RootTable* RT);
	int Search(HNode* root, double* key, int data, int dlen, HNode* Root[],	int & numRoot);
private:

	// Insertion
	HNode* _ChooseSubtree(HNode* self, stack* st, double* key, int data);
	int _TreatOverflow(HNode* self, stack* Stack, HNode* nNode1, HNode* nNode2,	HNode*& outNode1, HNode*& outNode2, int status, RootTable* RT, double tnow);

	// General
	bool _isKeySplit(pCursor *cursor, double tnow);
	void _RemoveEntry(HNode *self, stack *Stack, double tnow);
	bool _calcAliveNewBP(HNode *nNode, double *newKey);
	bool _calcDeadNewBP(HNode *nNode, double *newKey);
	void _InsertEntry(HNode *nNode, HNode *child);

	//key Split
	void _keySplit(pCursor *Cursor, int rightE[], int& nR, double* rkey, double* lkey);
	void _keySplitNode(HNode* self, pCursor *cursor, int rightE[], int numRight, HNode*& newNode, double *rkey, double *lkey);

	//version Split
	void _versionSplitNode(RootTable* RT, pCursor *cursor, HNode* self, HNode*& newNode, double currentTime);

	//SVO
	void _SVOSplitNode(RootTable* RT, pCursor *cursor, HNode* self, HNode*& newNode1, HNode*& newNode2, double currentTime);
	void _keySplitSVO(RootTable* RT, HNode* self, pCursor *cursor, int rightE[], int numRight, HNode*& newNode1, HNode*& newNode2, double *rkey,
			double *lkey);

	// Deletion
	bool _FindLeaf(HNode* Node, HNode *&leaf, stack *Stack, double* key, int data,	double currentTime);
	int _TreatOverflow2(HNode* self, stack* Stack, HNode* nNode1, HNode* nNode2,HNode*& outNode1, HNode*& outNode2, int status, RootTable* RT,
			double tnow);
	int _TreatUnderflow(HNode* Parent, HNode* self, stack *st, HNode*& Node1, HNode*& Node2, double currentTime, bool isFromParent);
	bool _deleteNode(HNode* Parent, HNode* self, pCursor *cursor, int indexToSelf,	double currentTime);

	//Search
	int _Consistent(double *bp, double *key);

	//Debug
	void _printStack(stack *st);
	void _printNode(HNode* HNode);

};

#endif /* SRC_EXTENSIONS_H_ */
