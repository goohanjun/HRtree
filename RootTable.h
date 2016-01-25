/*
 * RootTable.h
 *
 *  Created on: Nov 6, 2015
 *      Author: hanjunkoo
 */

#ifndef SRC_ROOTTABLE_H_
#define SRC_ROOTTABLE_H_

#include "HNode.h"
#include "HDefs.h"
#include "Extensions.h"
#include <vector>

#include <set>

using namespace std;
class Extensions;

class RootTable {

	struct item{
		double bp[6];
		int objectID;
	};

	struct itemArray {
		item items[100];
	};

public:
	HNode* Root[MAX_ROOT];
	Extensions* ext;
	int numRoot;
	int numObject;
	vector<itemArray> itemVector;

	RootTable();
	virtual ~RootTable();

	// Basic Operations
	int CommandInsert(double* key, int data, int dlen);
	int CommandDelete(double* key, int data, int dlen);
	void CommandSearch(double *key,int data,set<int>* ans, int areaCondition, int timeCondition);

	//Verify the structure of the Tree.
	bool CommandVerifyTree(int currentTime);
	bool CommandVerify(set<int>* answers[],int ElapsedTime);

	//Print
	void CommandView();
	void CommandViewLast();
	void CommandDump(HNode* HNode);
	void CommandPrint(HNode* HNode);

private:
	//Search
	void _SearchAllObject(HNode *Node, set<int>* object, int currentTime);
	void _SearchObject(HNode *Node, double *key, set<int>* object, int currentTime);
	void _SearchOverlappedObject(HNode *Node, double *key, set<HNode*>* visitedNode, set<int>* object, int currentTime);
	void _SearchIncludedObject(HNode *Node, double *key, set<HNode*>* visitedNode, set<int>* object, int currentTime);

	//Verify
	bool VerifyRootNode(HNode* HNode, int currentTime);
	bool Verify(HNode* HNode, int currentTime);
};
#endif /* SRC_ROOTTABLE_H_ */
