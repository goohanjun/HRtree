/*
 * RootTable.h
 *
 *  Created on: Nov 6, 2015
 *      Author: hanjunkoo
 */

#include "RootTable.h"

#include <iostream>

using namespace std;

RootTable::RootTable(){
	Root[0] = new HNode();
	Root[0]->isRoot = true;
	numRoot = 1;
	numObject = 0;
	ext = new Extensions();
}

RootTable::~RootTable(){
	for (int i = 0; i<numRoot;i++)
		delete Root[i];
}

int RootTable::CommandInsert(double* key, int data, int dlen) {
	if (!ext->Insert(Root[numRoot-1], key, data, sizeof(int), this)) {
		if (numRoot < MAX_ROOT) { //previous Root is deleted. Insert a new alive root
			cout<<"Main:: Insert a new alive root"<<endl;
			Root[numRoot] = new HNode();
			Root[numRoot]->isRoot = true;
			Root[numRoot]->bp[4] = key[4];
			numRoot++;
			//Insert the object into new Root
			ext->Insert(Root[numRoot-1], key, data, sizeof(int), this);
		}
	}
	numObject++;
	return 1;
}

int RootTable::CommandDelete(double* key, int data, int dlen) {
	if (ext->Delete(Root[numRoot-1], key, data, dlen, this)) {
		return 1;
	} else {
		cout << "Deletion is Failed, id = " << data << endl;
		return 0;
	}
}

void RootTable::CommandSearch(double *key,int data, set<int>* ans, int areaCondition, int timeCondition) {
	double queryStartTime = key[4];
	double queryEndTime = key[5];
	// Search all of overlapped region
	for (int i = 0; i < numRoot; i++) {
		if ((queryStartTime <= Root[i]->bp[4] && Root[i]->bp[4] < queryEndTime ) ||
			(queryStartTime <= Root[i]->bp[5] && Root[i]->bp[5] < queryEndTime )) {
			if (areaCondition == 0)
				_SearchOverlappedObject(Root[i], key, ans, timeCondition);
			else //areaCondition == 1
				_SearchIncludedObject(Root[i], key, ans, timeCondition);
		}
		else if(Root[i]->bp[4] <= queryStartTime && queryEndTime <= Root[i]->bp[5]){
			if (areaCondition == 0)
				_SearchOverlappedObject(Root[i], key, ans, timeCondition);
			else //areaCondition == 1
				_SearchIncludedObject(Root[i], key, ans, timeCondition);
		}
	}
}

//Search overlapped Object in key region
void RootTable::_SearchOverlappedObject(HNode *Node, double *key, set<int>* object, int timeCondition) {
	hr_rect keyBP;
	keyBP.copyRect(key);
	for (int i = 0; i < Node->numEntry; i++) {
		if(timeCondition == 0 ){ //Overlap
			if ( keyBP.isOverlap(Node->entries[i].bp) && keyBP.isTimeOverlap(Node->entries[i].bp)) {
				if (Node->level == 1){
					object->insert(Node->entries[i].data);
				}
				else
					_SearchOverlappedObject(Node->entries[i].child, key, object, timeCondition);
			}
		}
		else{ //Include
			if ( keyBP.isOverlap(Node->entries[i].bp) && keyBP.isTimeIncluded(Node->entries[i].bp)) {
				if (Node->level == 1){
					object->insert(Node->entries[i].data);
				}
				else
					_SearchOverlappedObject(Node->entries[i].child, key, object, timeCondition);
			}
		}
	}
	keyBP.dealloc();
}

//Search included Object in key region
void RootTable::_SearchIncludedObject(HNode *Node, double *key, set<int>* object, int timeCondition) {
	int Time = key[4];
	hr_rect keyBP;
	keyBP.copyRect(key);
	for (int i = 0; i < Node->numEntry; i++) {
		if ( keyBP.isIncluded(Node->entries[i].bp)  ) {
			if (Node->level == 1){
				object->insert(Node->entries[i].data);
			}
			else
				_SearchIncludedObject(Node->entries[i].child, key, object, Time);
		}
	}
	keyBP.dealloc();
}

// Timestamp Query to all region
void RootTable::_SearchAllObject(HNode *Node, set<int>* object, int currentTime) {
	for (int i = 0; i < Node->numEntry; i++) {
		if ( Node->entries[i].bp[4] <= currentTime	&& currentTime < Node->entries[i].bp[5] ) {
			if (Node->level == 1) {
				object->insert(Node->entries[i].data);
			} else{
				_SearchAllObject(Node->entries[i].child, object, currentTime);
			}
		}
	}
}

// Timestamp Query to all region
void RootTable::_SearchObject(HNode *Node, double *key, set<int>* object, int Time) {
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

//Debug
//Verification.
bool RootTable::CommandVerify(set<int>* answers[],int ElapsedTime) {
	bool isCorrect = true;
	//Verify the trees after deletion.
	if (!CommandVerifyTree(ElapsedTime)) {
		isCorrect = false;
		if (isVerifyAnswer)
			cout << "\n\n\nSomething is Wrong in Tree Structure\n\n\n";
	}
	return isCorrect;
}

bool RootTable::CommandVerifyTree(int currentTime){
	bool flag = true;
	for(int i = 0; i< numRoot; i++){
		if (Root[i]->numEntry > 0) {
			if (VerifyRootNode(Root[i], currentTime))
				flag = flag * true;
			else {
				flag = false;
				cout << i + 1 << "th Root has Error /" << numRoot << endl;
			}
		}
	}
	return flag;
}

bool RootTable::VerifyRootNode(HNode* HNode, int currentTime){
	bool flag = true;
	if (!Verify(HNode, currentTime)) {
		if (isVerifyTree)
			cout << "Verify:: Something is wrong in this tree" << endl;
		flag = false;
	}
	if (HNode->level > 1) {
		for (int i = 0; i < HNode->numEntry; i++) {
			flag = flag*VerifyRootNode(HNode->entries[i].child, currentTime);
		}
	}
	return flag;
}

bool RootTable::Verify(HNode* HNode, int currentTime) {
	bool flag = true;
	if (!HNode->_isMinMaxCorrect()) {
		if (isVerifyTree)
			cout << "Verify:: _isMinMaxCorrect" << endl;
		flag = false;
	}
	if (!HNode->_isUnderflow()) {
		if (isVerifyTree)
			cout << "Verify:: _isUnderflow" << endl;
		flag = false;
	}
	if (HNode->bp[5] == DBL_MAX) { // If this Node is Alive
		if (!HNode->_isAliveBPCorrectNode()) {
			if (isVerifyTree){
				cout << "Verify:: _isAliveBPCorrectNode  lvl = " << HNode->level<<endl;
				CommandPrint(HNode);
			}
			flag = false;
		}
	} else if (!HNode->_isDeadBPCorrectNode(currentTime)) {
		if (isVerifyTree){
			cout << "Verify:: _isDeadBPCorrectNode   lvl = " << HNode->level<<endl;
			CommandPrint(HNode);
		}
		flag = false;
	}
	if (HNode->level > 1) {
		for (int i = 0; i < HNode->numEntry; i++) {
			if (HNode->entries[i].bp[5] == DBL_MAX) { // If this entry is Alive
				if (!HNode->_isAliveBPCorrectEntry(&HNode->entries[i], currentTime)) {
					if (isVerifyTree){
						cout << "Verify:: _isAliveBPCorrectEntry  i = " <<i<<" level = "<<HNode->level <<endl;
						CommandPrint(HNode);
						cout << "////////////////" << endl;
						CommandPrint(HNode->entries[i].child);
					}
					flag = false;
				}
			} else if (!HNode->_isDeadBPCorrectEntry(&HNode->entries[i],
					currentTime)) {
				flag = false;
				if (isVerifyTree) {
					cout << "Verify:: _isDeadBPCorrectEntry  i = " << i<<" level = "<<HNode->level <<endl;
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
void RootTable::CommandView() {
	for(int i = 0;i<numRoot;i++){
		CommandDump(Root[i]);
	}
}

void RootTable::CommandViewLast(){
	CommandDump(Root[numRoot-1]);
}

void RootTable::CommandDump(HNode* HNode) {
	CommandPrint(HNode);
	if (HNode->level > 1) {
		for (int i = 0; i < HNode->numEntry; i++) {
			CommandDump(HNode->entries[i].child);
		}
	}
}

void RootTable::CommandPrint(HNode* HNode) {
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
