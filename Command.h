/*
 * Command.h
 *
 *  Created on: Aug 22, 2015
 *      Author: hanjunkoo
 */

#ifndef SRC_COMMAND_H_
#define SRC_COMMAND_H_

#include "HNode.h"
#include "RootTable.h"
#include "Verify.h"
#include <set>
using namespace std;

int CommandInsert(double* key, int data, int dlen, RootTable* RootTable);
int CommandDelete(double* key, int data, int dlen, RootTable* RootTable);
void CommandSearch(RootTable *RT, double *key,int data,set<int>* ans, int areaCondition, int timeCondition);

//Search
void _SearchAllObject(HNode *Node, set<int>* object, int currentTime);
void _SearchObject(HNode *Node, double *key, set<int>* object, int currentTime);
void _SearchOverlappedObject(HNode *Node, double *key, set<int>* object, int currentTime);
void _SearchIncludedObject(HNode *Node, double *key, set<int>* object, int currentTime);

//Verification
//Verify the structure of the Tree.
bool CommandVerifyTree(RootTable *RT, int currentTime);
bool VerifyRootNode(HNode* HNode, int currentTime);
bool Verify(HNode* HNode, int currentTime);

//Print
bool CommandVerify(RootTable *RT, set<int>* answers[],int ElapsedTime);
void CommandDump(HNode* HNode);
void CommandView(RootTable *RT);
void CommandPrint(HNode* HNode);

//Deprecated
bool _CompareSet(set<int>* answer, set<int>* object);
bool CommandVerifyAnswer(RootTable *RT, set<int>* answers[],int ElapsedTime);
#endif /* SRC_COMMAND_H_ */
