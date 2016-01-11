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

int CommandInsert(HNode* root, double* key, int data, int dlen, RootTable* RootTable);
int CommandDelete(HNode* root, double* key, int data, int dlen, RootTable* RootTable);
int CommandSearch(HNode* root, double* key, int data, int dlen, RootTable* RootTable);
void CommandTimeStamp(RootTable *RT, double *key,int data,set<int>* ans, int status);
void CommandTimeIntv(RootTable *RT, double *key,int data,set<int>* ans, int status);
/*
 * Verification
 */

//Verify the structure of the Tree.
bool CommandVerifyTree(RootTable *RT, int currentTime);
bool VerifyRootNode(HNode* HNode, int currentTime);
bool Verify(HNode* HNode, int currentTime);

//Verify the answer set with object id.
bool CommandVerifyAnswer(RootTable *RT, set<int>* answers[],int ElapsedTime);
void _SearchObject(HNode *Node, set<int>* object, int currentTime);
void _SearchObject(HNode *Node, double *key, set<int>* object, int currentTime);
bool _CompareSet(set<int>* answer, set<int>* object);


bool CommandVerify(RootTable *RT, set<int>* answers[],int ElapsedTime);
void CommandDump(HNode* HNode);
void CommandView(RootTable *RT);
void CommandPrint(HNode* HNode);
#endif /* SRC_COMMAND_H_ */
