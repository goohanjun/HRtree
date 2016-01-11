/*
 * Command.cpp
 *
 *  Created on: Aug 22, 2015
 *      Author: hanjunkoo
 */

#include "Command.h"
#include "Extension.h"
#include "RootTable.h"

#include <iostream>

using namespace std;

int CommandInsert(HNode* root, double* key, int data, int dlen, RootTable* RT) {
	if (!Insert(RT->Root[RT->numRoot-1], key, data, sizeof(int), RT)) {
		if (RT->numRoot < MAX_ROOT) { //previous Root is deleted. Insert a new alive root
			cout<<"Main:: Insert a new alive root"<<endl;
			RT->Root[RT->numRoot] = new HNode();
			RT->Root[RT->numRoot]->isRoot = true;
			RT->Root[RT->numRoot]->bp[4] = key[4];
			RT->numRoot++;
			Insert(RT->Root[RT->numRoot-1], key, data, sizeof(int), RT);
			//Insert the object into new Root
		}
	}
	return 1;

}

int CommandDelete(HNode* root, double* key, int data, int dlen, RootTable* RT) {
	if (Delete(root, key, data, dlen, RT)) {
		return 1;
	} else {
		cerr << "Deletion is Failed,  key[0] = " << key[0] << endl;
		return 0;
	}
}

void CommandTimeStamp(RootTable *RT, double *key,int data,set<int>* ans, int status) {
	// status == 0
	// Search all of overlapped region
	double queryTime = key[4]; //||key[5]
	//int numRootVisited=0;
	for (int i = 0;i<RT->numRoot;i++){
		if( RT->Root[i]->bp[4] <= queryTime &&  queryTime < RT->Root[i]->bp[5] ){
			_SearchObject(RT->Root[i], key, ans,queryTime);
			//numRootVisited++;
		}
	}

	//cout<<numRootVisited <<" many roots were visited"<<endl;
	// status == 1
	// Search only included region


}

void CommandTimeIntv(RootTable *RT, double *key,int data,set<int>* ans, int status) {
	// status == 0
	// Search all of overlapped region



	// status == 1
	// Search only included region


}

void _SearchObject(HNode *Node, double *key, set<int>* object, int Time) { // Timestamp Query to all region
	hr_rect keyBP;
	keyBP.copyRect(key);
	for (int i = 0; i < Node->numEntry; i++) {
		if ( (Node->entries[i].bp[4] <= Time)	&& (Time < Node->entries[i].bp[5])  &&	keyBP.isOverlap(Node->entries[i].bp)  ) {
			if (Node->level == 1){
				object->insert(Node->entries[i].data);
				//cout<< Node->entries[i].data <<" is Chosen"<<endl;
			}
			else
				_SearchObject(Node->entries[i].child, key, object, Time);
		}
	}
	keyBP.dealloc();
}

void _SearchObject(HNode *Node, set<int>* object, int currentTime) { // Timestamp Query to all region
	for (int i = 0; i < Node->numEntry; i++) {
		if ( Node->entries[i].bp[4] <= currentTime	&& currentTime < Node->entries[i].bp[5] ) {
			if (Node->level == 1) {
				object->insert(Node->entries[i].data);
			} else{
				_SearchObject(Node->entries[i].child, object, currentTime);
			}
		}
	}
}

//Debug
//Verification.
bool CommandVerify(RootTable *RT, set<int>* answers[],int ElapsedTime) {
	bool isCorrect = true;
	//Verify the trees after deletion.
	if (!CommandVerifyTree(RT, ElapsedTime)) {
		isCorrect = false;
		if (isVerifyAnswer)
			cout << "\n\n\nSomething is Wrong in Tree Structure\n\n\n";
	}
	if(!CommandVerifyAnswer(RT,answers,ElapsedTime)){
		isCorrect = false;
		if (isVerifyAnswer)
			cout << "\n\n\nSomething is Wrong in Object set\n\n\n";
	}
	return isCorrect;
}

bool CommandVerifyAnswer(RootTable *RT, set<int>* answers[], int ElapsedTime){
	bool flag = true;
	for (int i = 0; i < ElapsedTime + 1; i++) {

		//Debug
		if (isVerifyAnswer)
			cout<<"Time :" <<i<<" Answer Verfication"<<endl;

		set<int> *object = new set<int>;

		for (int j = 0; j < RT->numRoot; j++) {
			if (i >= RT->Root[j]->bp[4])
				_SearchObject(RT->Root[j], object,i);
		}
		flag = flag * _CompareSet(answers[i], object);

		delete object;
	}
	return flag;
}

bool _CompareSet(set<int>* answer, set<int>* object){
	bool isSame = true;
	set<int>::iterator it, it1, it2;
	if (answer->size() != object->size()){
		if (isVerifyAnswer) {
			cout << "Answer set Size = " << answer->size() << endl;
			for (it=answer->begin(); it!=answer->end(); ++it)
				cout<<*it<<" ";
			cout << "\nObject set Size = " << object->size() << endl;
			for (it = object->begin(); it != object->end(); ++it)
				cout << *it << " ";
			cout<<endl;
		}
		isSame = false;
	}
	for(it1=answer->begin() , it2 = object->begin(); it1 !=answer->end() , it2 != object->end() ; ++it1 , ++it2){
		if(*it1 != *it2)
			isSame = false;
	}
	return isSame;
}

bool CommandVerifyTree(RootTable *RT, int currentTime){
	//cout<<"Command Verify:: # of Root = "<<RT->numRoot<<endl;
	bool flag = true;
	for(int i = 0; i< RT->numRoot; i++){
		flag = flag*VerifyRootNode(RT->Root[i],currentTime);
	}
	return flag;
}

bool VerifyRootNode(HNode* HNode, int currentTime){
	bool flag = true;
	if (!Verify(HNode, currentTime)) {
		if (isVerifyTree)
			cout << "Verify:: Something is wrong in this tree" << endl;
		flag = false;
		//CommandPrint(HNode);
	}
	if (HNode->level > 1) {
		for (int i = 0; i < HNode->numEntry; i++) {
			flag = flag*VerifyRootNode(HNode->entries[i].child, currentTime);
		}
	}
	return flag;
}

bool Verify(HNode* HNode, int currentTime) {
	bool flag = true;
	if (!_isMinMaxCorrect(HNode)) {
		if (isVerifyTree)
			cout << "Verify:: _isMinMaxCorrect" << endl;
		flag = false;
	}
	if (!_isUnderflow(HNode)) {
		if (isVerifyTree)
			cout << "Verify:: _isUnderflow" << endl;
		flag = false;
	}
	if (HNode->bp[5] == DBL_MAX) { // If this Node is Alive
		if (!_isAliveBPCorrectNode(HNode)) {
			if (isVerifyTree)
				cout << "Verify:: _isAliveBPCorrectNode" << endl;
			flag = false;
		}
	} else if (!_isDeadBPCorrectNode(HNode, currentTime)) {
		if (isVerifyTree)
			cout << "Verify:: _isDeadBPCorrectNode" << endl;
		flag = false;
	}
	if (HNode->level > 1) {
		for (int i = 0; i < HNode->numEntry; i++) {
			if (HNode->entries[i].bp[5] == DBL_MAX) { // If this entry is Alive
				if (!_isAliveBPCorrectEntry(&HNode->entries[i], currentTime)) {
					if (isVerifyTree)
						cout << "Verify:: _isAliveBPCorrectEntry  i = " <<i <<endl;
					flag = false;
				}
			} else if (!_isDeadBPCorrectEntry(&HNode->entries[i],
					currentTime)) {
				flag = false;
				if (isVerifyTree) {
					cout << "Verify:: _isDeadBPCorrectEntry  i = " << i << endl;
					CommandPrint(HNode);
					cout << "////////////////" << endl;
					CommandPrint(HNode->entries[i].child);
				}
			}
		}
	}
	return flag;
}


//Print Roots
void CommandView(RootTable* RT) {
	for(int i = 0;i<RT->numRoot;i++){
		CommandDump(RT->Root[i]);
	}
}

void CommandDump(HNode* HNode) {
	CommandPrint(HNode);
	if (HNode->level > 1) {
		for (int i = 0; i < HNode->numEntry; i++) {
			CommandDump(HNode->entries[i].child);
		}
	}
}

void CommandPrint(HNode* HNode) {
	int lvl = HNode->level;

	if (HNode->isRoot)
		cout << "Root"<<endl;
	cout << "#" << HNode->NodeNumber << " level: " << lvl << " # slots: "
			<< HNode->numEntry << ", Avail: " << MaxEntry - HNode->numEntry
			<< endl;
	cout << "\tbp: <     ";

	for (int i = 0; i < dim * 2; i++) {
		if (HNode->bp[i] == DBL_MAX)
			cout << "* ";
		else
			cout << HNode->bp[i] << " ";
	}
	cout << ">\n";

	for (int i = 0; i < HNode->numEntry; i++) {
		cout << "\t[" << i << "] <";
		cout << " bp: ";
		for (int j = 0; j < dim * 2; j++) {
			if ((double) HNode->entries[i].bp[j] == DBL_MAX)
				cout << "* ";
			else
				cout << (double) HNode->entries[i].bp[j] << " ";
		}
		cout << " >";
		if (lvl == 1)
			cout << " data: " << HNode->entries[i].data << endl;
		else
			cout << " child: " << HNode->entries[i].child->numEntry << endl;
	}
	return;
}

