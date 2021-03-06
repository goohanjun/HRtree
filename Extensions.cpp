/*
 * Extensions.cpp
 *
 *  Created on: Jan 16, 2016
 *      Author: hanjunkoo
 */

#include "Extensions.h"

Extensions::Extensions() {
	// TODO Auto-generated constructor stub

}

Extensions::~Extensions() {
	// TODO Auto-generated destructor stub
}


#include <cassert>

using namespace std;

int Extensions::Insert(HNode* root, double* key, int data, int dlen, RootTable* RT) {

	stack *Stack = new stack();
	HNode *leaf;
	HNode *nNode1, *nNode2;
	HNode *oNode1, *oNode2;
	int status = 0;
	double tnow = key[4]; //

	if (root->level != 1) { //Root가 Leaf노드가 아니고, Root노드 아래가 다 닫혀있다면, 새로운 루트를 생성해야한다! (Deletion때문에 발생 가능)
		if (root->_numAlive(key[4]) == 0)
			return 0;
	}
	hr_rect *rect = new hr_rect(root->bp, 0); // change bp of root with key
	rect->expand(key);

	leaf = _ChooseSubtree(root, Stack, key, data);

	// Insert object in a leaf node
	memcpy(&leaf->entries[leaf->numEntry].data, &data, dlen);
	memcpy(leaf->entries[leaf->numEntry].bp, key, klen);
	leaf->numEntry++;

	int tempStatus = 0;
	if (leaf->isRoot == true) {
		status = _TreatOverflow(leaf, Stack, nNode1, nNode2, oNode1, oNode2, tempStatus, RT, tnow);
		Stack->curr--;
		nNode1 = oNode1;
		nNode2 = oNode2;
	} else {
		for (int i = 0; i < Stack->depth + 1; i++) {
			HNode *outNode1, *outNode2;
			tempStatus = _TreatOverflow(Stack->trace[Stack->curr], Stack,
					nNode1, nNode2, outNode1, outNode2, status, RT, tnow);
			Stack->curr--;
			status = tempStatus;
			nNode1 = outNode1;
			nNode2 = outNode2;
		}
	}
	delete Stack;
	return 1;
}

int Extensions::Delete(HNode* root, double* key, int data, int dlen, RootTable* RT) {

	stack *Stack = new stack();
	HNode *leaf, *nNode1, *nNode2;
	int statusUF = 0;
	int statusOF = 0;
	double currentTime = key[4]; // || key[4];

	if (root->bp[5] != DBL_MAX) {
		return 0;
	}
	if (!_FindLeaf(root, leaf, Stack, key, data, currentTime,RT)) { //Find a leaf node to delete the object from
		return 0;
	}
	if (root->level == 1) // Don't need to go further
		return 1;

	Stack->depth = root->level;


	// Leaf Underflow
	// status == 1 , nNode1 need to be inserted in ParentNode. status == 2 , nNode1,2 need to be inserted in ParentNode
	if (leaf->_numAlive() < UF_Ratio * MaxEntry) {
		statusUF = _TreatUnderflow(Stack->trace[1], leaf, Stack, nNode1, nNode2,
				currentTime, true);
	}

	for (int i = 1; i < root->level; i++) {
		HNode *nNode3, *nNode4, *oNode1, *oNode2;
		// Fit in
		if (Stack->trace[i]->numEntry + statusUF + 1 < MaxEntry) {
			// Insert nNode1
			if (statusUF > 0) {
				_InsertEntry(Stack->trace[i], nNode1);
			}
			// Insert nNode2
			if (statusUF == 2) {
				_InsertEntry(Stack->trace[i], nNode2);
			}

			// Calculate new Key
			double newKey[dim * 2];
			if (!_calcAliveNewBP(Stack->trace[i], newKey)) {

			}
			memcpy(Stack->trace[i]->bp, newKey, klen - 2 * sizeof(double));

			//Break when reached to root
			if (Stack->trace[i]->isRoot == true)
				break;

			// Parent entry's bp Update
			HNode *Parent = Stack->trace[i + 1];
			int ParentToi = Stack->choice[Stack->trace[i]->level];

			memcpy(Parent->entries[ParentToi].bp, Stack->trace[i]->bp,
					klen - 2 * sizeof(double));

			statusOF = 0;
			statusUF = 0;

			// Fit in
			if ((Stack->trace[i]->_numAlive() + 1) < UF_Ratio * MaxEntry) { //Underflow

				statusUF = _TreatUnderflow(Stack->trace[i + 1], Stack->trace[i],
						Stack, nNode3, nNode4, currentTime, true);
				nNode1 = nNode3;
				nNode2 = nNode4;
			}
		} else {
			//Stack->trace[i] 에 nNode1, nNode2 를 넣어주고 oNode1, oNode2로 쪼개져서 나오는 함수.

			statusOF = _TreatOverflow2(Stack->trace[i], Stack, nNode1, nNode2,
					oNode1, oNode2, statusUF, RT, currentTime);
			nNode1 = oNode1;
			nNode2 = oNode2;

			//Break when reached to root
			if (Stack->trace[i]->isRoot == true)
				break;

			// Parent entry's bp Update
			HNode *Parent = Stack->trace[i + 1];
			int position = Stack->choice[Stack->trace[i]->level];

			if (statusOF == 2 || statusOF == 3) { //VersionSplit
			// Delete entry from parent

				int MaxNum = Stack->trace[i + 1]->numEntry;
				if (Parent->entries[position].bp[4] < currentTime) // logical delete
					Parent->entries[position].bp[5] = currentTime;
				else { // Physical delete
					if (position != (MaxNum - 1))
						memcpy(&(Parent->entries[position]),
								&(Parent->entries[MaxNum - 1]), sizeof(Entry));
					Parent->numEntry--;
				}
			} else if (statusOF == 1) { // KeySplit
				double newBP[dim * 2];
				int position = Stack->choice[Stack->trace[i]->level];
				_calcAliveNewBP(Stack->trace[i], newBP);
				memcpy((Parent->entries[position].bp), newBP,
						klen - 2 * sizeof(double));

			}

			// Update Parent's BP
			double newKey[dim * 2];
			_calcAliveNewBP(Parent, newKey);
			memcpy(Parent->bp, newKey, klen - 2 * sizeof(double));

			if (statusOF == 3) //Overflow SVO
				statusUF = 2;
			else if (statusOF == 2 || statusOF == 1) //Key Or Version Split
				statusUF = 1;
			else
				statusUF = 0;

			if (statusOF == 2
					&& (oNode1->_numAlive() + 1) < UF_Ratio * MaxEntry) { // VersionSplit
					//Stack->trace[i] 가 UF 되는데, nNode1, nNode2로 합쳐지고 (쪼개져서) 나오는 함수.
				statusUF = _TreatUnderflow(Stack->trace[i + 1], oNode1, Stack,
						nNode3, nNode4, currentTime, false);
				nNode1 = nNode3;
				nNode2 = nNode4;

			}
		}

	}

	HNode *oldRoot = RT->Root[RT->numRoot - 1];
	// Lower the root if there is only one alive child node of Root
	if (RT->Root[RT->numRoot - 1]->_numAlive() == 1) {
		if (oldRoot->numEntry != 1) {
			int index = 0;
			for (int i = 0; i < oldRoot->numEntry; i++) {
				if (oldRoot->entries[i].bp[5] == DBL_MAX) {
					RT->Root[RT->numRoot] = oldRoot->entries[i].child;
					RT->numRoot++;
					oldRoot->entries[i].child->isRoot = true;
					index = i;
				}
			}

			if (index != (oldRoot->numEntry - 1)) // deletion
				memcpy(&oldRoot->entries[index],
						&oldRoot->entries[oldRoot->numEntry - 1],
						sizeof(Entry));
			oldRoot->numEntry--;

			double newKey[dim * 2];
			_calcDeadNewBP(oldRoot, newKey);
			memcpy(oldRoot->bp, newKey, klen);
			oldRoot->bp[5] = currentTime; //Root closed!
		} else {
			RT->Root[RT->numRoot - 1] = oldRoot->entries[0].child;
			oldRoot->entries[0].child->isRoot = true;
		}
	}

	delete Stack;
	return 1;
}

int Extensions::Search(HNode* self, double* key, int data, int dlen, HNode* Root[],
		int & numRoot) {
	for (int i = 0; i < self->numEntry; i++) {
		if (self->level == 1) {
			return 1;
		}
		if (_Consistent(self->entries[i].child->bp, key))
			Search(self->entries[i].child, key, data, dlen, Root, numRoot);
	}
	return 1;
}

//Deletion
bool Extensions::_FindLeaf(HNode* Node, HNode *&leaf, stack *Stack, double* key, int numID,
		double currentTime, RootTable* RT) {
	// Reached to Leaf Node
	if (Node->level == 1) {
		int index = 0;
		int numAlive = 0;
		bool isExist = false;
		for (int i = 0; i < Node->numEntry; i++) {

			int numObject = Node->entries[i].data;
			int objectID = RT->itemVector[numObject/100].items[numObject%100].objectID;

			if (objectID == numID) {
				RT->itemVector[numObject/100].items[numObject%100].bp[5] = currentTime;
				index = i;
				isExist = true;
			} else if (Node->entries[i].bp[5] == DBL_MAX) // Alive Entry
				numAlive++;
		}
		if (isExist == false)
			return false;

		// Physical Deletion
		if (Node->entries[index].bp[4] == currentTime) {
			if (index != (Node->numEntry - 1))
				memcpy(&Node->entries[index],
						&Node->entries[Node->numEntry - 1], sizeof(Entry));
			Node->numEntry--;
		} else
			// Logical Deletion
			Node->entries[index].bp[5] = currentTime;

		if (Node->isRoot == true && Node->numEntry == 0) {

			//cout << "The Root is empty" << endl;
			Node->bp[0] = DBL_MAX;
			Node->bp[1] = DBL_MAX;
			Node->bp[2] = 0.0;
			Node->bp[3] = 0.0;
			Node->bp[4] = currentTime;
			Node->bp[5] = DBL_MAX;
			return true;
		}

		hr_rect newBP;
		bool flag = true;
		for (int i = 0; i < Node->numEntry; i++) {
			if (Node->entries[i].bp[5] == DBL_MAX) {
				if (flag) {
					newBP.copyRect(Node->entries[i].bp);
					flag = false;
				} else
					newBP.expandTime(Node->entries[i].bp);
			}
		}
		if (!newBP.isEqual(Node->bp)) {
			Stack->isChanged[0] = true;
			memcpy(Node->bp, newBP.coord, klen - 2 * sizeof(double));
		}
		if (Node->isRoot && Node->level == 1) { // When Root is a leaf node, Root can be closed.
			if (numAlive == 0)
				Node->bp[5] = currentTime; // Close the Root
		}
		leaf = Node;
		Stack->trace[0] = Node;
		Stack->choice[0] = index;

		newBP.dealloc();
		return true;
	}
	hr_rect keyRect;
	keyRect.copyRect(key);
	// Non-leaf Nodes
	for (int i = 0; i < Node->numEntry; i++) {
		if (Node->entries[i].bp[5] == DBL_MAX
				&& keyRect.isIncluded(Node->entries[i].bp)) { // Alive Entry which include key's BP
			if (_FindLeaf(Node->entries[i].child, leaf, Stack, key, numID,
					currentTime, RT)) {
				if (Node->entries[i].bp[4] < currentTime
						&& Stack->isChanged[Node->level - 2]) { // Insert new entry e' and close e

					// make e'
					memcpy(&Node->entries[Node->numEntry], &Node->entries[i],
							sizeof(Entry));
					memcpy(Node->entries[Node->numEntry].bp,
							Node->entries[i].child->bp,
							klen - 2 * sizeof(double));

					// close e
					Node->entries[i].bp[5] = currentTime;
					Node->entries[Node->numEntry].bp[4] = currentTime;
					Node->entries[Node->numEntry].bp[5] = DBL_MAX;

					// adjust Stack
					Stack->choice[Node->level - 1] = Node->numEntry;
					Node->numEntry++;

				} else if (Stack->isChanged[Node->level - 2]) { // Just update e's BP
					memcpy(Node->entries[i].bp, Node->entries[i].child->bp,
							klen - 2 * sizeof(double)); //except Time
					Stack->choice[Node->level - 1] = i;
				} else {
					Stack->choice[Node->level - 1] = i;
				}

				hr_rect newBP;
				bool flag = true;
				// Calculate newBP
				for (int j = 0; j < Node->numEntry; j++) {
					if (Node->entries[j].bp[5] == DBL_MAX) {
						if (flag) {
							newBP.copyRect(Node->entries[j].bp);
							flag = false;
						} else
							newBP.expandTime(Node->entries[j].bp);
					}
				}
				Stack->trace[Node->level - 1] = Node;
				if (!newBP.isEqual(Node->bp)) {
					Stack->isChanged[Node->level - 1] = true;
					memcpy(Node->bp, newBP.coord, klen - 2 * sizeof(double));
				}
				newBP.dealloc();
				keyRect.dealloc();
				return true;
			}
		}
	}
	keyRect.dealloc();
	return false;
}

int Extensions::_TreatUnderflow(HNode* Parent, HNode* self, stack *st, HNode*& Node1,
		HNode*& Node2, double currentTime, bool isFromParent) {

	int status = 0;
	int index = 0; // From parent to MergeNode
	int indexToSelf = st->choice[self->level]; // From parent to self
	double minAreaEnlarge = DBL_MAX;
	double tempArea;
	double deltaArea;
	int level = self->level;

	for (int i = 0; i < Parent->numEntry; i++) {
		if (i != indexToSelf && Parent->entries[i].bp[5] == DBL_MAX) { // See only alive entry except self
			hr_rect keyBP;
			keyBP.copyRect(Parent->entries[i].bp);
			tempArea = keyBP.span();
			keyBP.expand(self->bp);
			deltaArea = keyBP.span() - tempArea;

			if (deltaArea < minAreaEnlarge) {
				index = i;
				minAreaEnlarge = deltaArea;
			}
			keyBP.dealloc();
		}
	}

	// Find an alive node that give minimum area enlargement
	HNode *MergeNode = Parent->entries[index].child;

	pCursor *cursor = new pCursor();
	// Delete the Node & Make cursor
	if (isFromParent == true) {
		if (indexToSelf == Parent->numEntry - 1) {
			_deleteNode(Parent, self, cursor, indexToSelf, currentTime);
			_deleteNode(Parent, MergeNode, cursor, index, currentTime);
		} else {
			_deleteNode(Parent, MergeNode, cursor, index, currentTime);
			_deleteNode(Parent, self, cursor, indexToSelf, currentTime);
		}
	} else {
		_deleteNode(Parent, MergeNode, cursor, index, currentTime);
		cursor->InsertNode(self); // Self is from version Split.
		delete self;
	}

	if (cursor->size < MaxEntry * SVO_Ratio) { //Merge

		HNode *newNode1 = new HNode();
		hr_rect newBP;
		for (int i = 0; i < cursor->size; i++) {
			if (i == 0)
				newBP.copyRect(cursor->entries[i].bp);
			else
				newBP.expand(cursor->entries[i].bp);
			memcpy(&newNode1->entries[i], &cursor->entries[i], sizeof(Entry));
		}
		//Node setting
		newNode1->numEntry = cursor->size;
		newNode1->level = level;

		//BP setting
		memcpy(newNode1->bp, newBP.coord, klen - 2 * sizeof(double));
		newNode1->bp[4] = currentTime;
		newNode1->bp[5] = DBL_MAX;

		Node1 = newNode1;
		status = 1;
	} else { //KeySplit (Similar with SVO)

		HNode *rightNode = new HNode();
		HNode *leftNode = new HNode();
		double rkey[dim * 2], lkey[dim * 2];
		int numRight;
		int rightEntries[cursor->size];
		bool isRight = false;
		int rightSize = 0, leftSize = 0;

		_keySplit(cursor, rightEntries, numRight, rkey, lkey);

		for (int i = 0; i < cursor->size; i++) {
			isRight = false;
			for (int j = 0; j < numRight; j++) {
				if (rightEntries[j] == i)
					isRight = true;
			}
			if (isRight) {
				memcpy(&rightNode->entries[rightSize], &cursor->entries[i],
						sizeof(Entry));
				rightSize++;
			} else {
				memcpy(&leftNode->entries[leftSize], &cursor->entries[i],
						sizeof(Entry));
				leftSize++;
			}
		}

		rightNode->level = level;
		leftNode->level = level;

		memcpy(rightNode->bp, rkey, klen);
		memcpy(leftNode->bp, lkey, klen);

		rightNode->bp[4] = currentTime;
		leftNode->bp[4] = currentTime;
		rightNode->bp[5] = DBL_MAX;
		leftNode->bp[5] = DBL_MAX;

		rightNode->numEntry = rightSize;
		leftNode->numEntry = leftSize;

		Node1 = leftNode;
		Node2 = rightNode;

		status = 2;
	}
	delete cursor;
	return status;
}

int Extensions::_TreatOverflow2(HNode* self, stack* Stack, HNode* nNode1, HNode* nNode2,
		HNode*& outNode1, HNode*& outNode2, int status, RootTable* RT,
		double tnow) {

	HNode *newNode1, *newNode2;
	int numKey = 0;
	int ResultStatus = 0;

	//From Underflow
	numKey = status;

	bool isKeySplit = true;

	//Init the cursor.
	pCursor *cursor = new pCursor(); //

	cursor->InsertNode(self);
	if (numKey > 0)
		cursor->InsertEntry(nNode1); // put entry into cursor
	if (numKey == 2)
		cursor->InsertEntry(nNode2); // put entry into cursor
	int rightEntries[cursor->size];

	isKeySplit = _isKeySplit(cursor, tnow);

	if (isKeySplit) { //KeySplit
		//cerr << "\n\n\n Deletion KeySplit?!?!!?!\n\n\n\n" << endl;
		//assert(false);
		int numRight = 0;
		double rkey[dim * 2];
		double lkey[dim * 2];
		_keySplit(cursor, rightEntries, numRight, rkey, lkey);
		_keySplitNode(self, cursor, rightEntries, numRight, newNode1, rkey,
				lkey);
		ResultStatus = 1;
	} else { // VersionSplit
		if (cursor->numAliveEntry() < MaxEntry * SVO_Ratio) {
			_versionSplitNode(RT, cursor, self, newNode1, tnow);
			ResultStatus = 2;
		} else { // Strong Version Overflow. Do the KeySplit
			_SVOSplitNode(RT, cursor, self, newNode1, newNode2, tnow);
			ResultStatus = 3;
		}
	}
	delete cursor;

	outNode1 = newNode1;
	outNode2 = newNode2;

	//cout << "Extension::_TreatOverflow Over" << endl;
	return ResultStatus;
}

//Self will be closed.
bool Extensions::_deleteNode(HNode* Parent, HNode* self, pCursor *cursor, int indexToSelf,
		double currentTime) {
	// Make cursor
	for (int i = 0; i < self->numEntry; i++) {
		if (self->entries[i].bp[5] == DBL_MAX) {
			cursor->InsertEntry(&self->entries[i]);
			cursor->entries[cursor->size - 1].bp[4] = currentTime;
		}
	}
	//Self entry deletion
	int numPhysicalDeletion = 0;
	for (int i = self->numEntry - 1; i >= 0; i--) {
		if (self->entries[i].bp[4] < currentTime) { // Logical Deletion
			if (self->entries[i].bp[5] == DBL_MAX)
				self->entries[i].bp[5] = currentTime;
		} else { //Physical Deletion
			numPhysicalDeletion++;
			if (i != (self->numEntry - 1))
				memcpy(&self->entries[i],
						&self->entries[self->numEntry - numPhysicalDeletion],
						sizeof(Entry));
			//memcpy(&self->entries[self->numEntry-numPhysicalDeletion], &self->entries[i], sizeof(Entry));
		}
	}
	self->numEntry = self->numEntry - numPhysicalDeletion;

	// Calculate new BP of self
	hr_rect newBP;
	for (int i = 0; i < self->numEntry; i++) {
		if (i == 0)
			newBP.copyRect(self->entries[i].bp);
		else
			newBP.expandTime(self->entries[i].bp);
	}
	memcpy(self->bp, newBP.coord, klen); // Make newBP

	//Node Deletion
	if (self->bp[4] < currentTime) // Logically delete!
		self->bp[5] = currentTime;
	else { // Physically Delete the self node
		delete self;
	}

	//Parent Entry
	if (Parent->entries[indexToSelf].bp[4] < currentTime) { // Logically Delete!
		Parent->entries[indexToSelf].bp[5] = currentTime;
		return true;
	} else { // Physically Delete!
		if (indexToSelf != (Parent->numEntry - 1))
			memcpy(&Parent->entries[indexToSelf],
					&Parent->entries[Parent->numEntry - 1], sizeof(Entry));
		Parent->numEntry--;
		return false;
	}
}

//Insertion
HNode* Extensions::_ChooseSubtree(HNode* self, stack* st, double* key, int data) {

	int index;
	bool bpChanged;

	if (self->level == 1) {
		// Put leaf node in a stack
		hr_rect *rect1 = new hr_rect(self->bp, 0);
		rect1->expand(key);

		st->depth = st->curr;
		st->trace[st->curr] = self;
		st->isChanged[st->curr] = true;
		st->choice[st->curr] = 0;

		return self;
	} else if (self->level == 2)
		index = self->_findMinPenOver(key);
	else
		index = self->_findMinPen(key);

	hr_rect bpRect(self->bp, 0);
	bpRect.expand(key);

	hr_rect rect;
	rect.copyRect(self->entries[index].bp);
	bpChanged = rect.expand(key); // Don't change the bp. Just check whether there is a change or not
	rect.dealloc();

	if (self->entries[index].bp[4] < key[4] && bpChanged) { // Copy e to e' when MBR is changed
		//cout<<"Extension::_ChooseSubtree case 1"<<endl;

		memcpy(&self->entries[self->numEntry], &self->entries[index],
				sizeof(Entry));
		hr_rect *rect2 = new hr_rect(self->entries[self->numEntry].bp, 0);
		rect2->expand(key); // Expand the intermediate node's BPs with new key

		self->entries[self->numEntry].bp[4] = key[4]; // Change the start time of e'
		self->entries[index].bp[5] = key[4]; // Change the end time of e

		st->choice[st->curr] = self->numEntry; // New key will go into this place
		self->numEntry++;
		st->trace[st->curr] = self;
		st->isChanged[st->curr] = true;
		st->curr++;
	} else {
		//cout<<"Extension::_ChooseSubtree case 2"<<endl;

		hr_rect *rect2 = new hr_rect(self->entries[index].bp, 0);
		rect2->expand(key); // Expand the intermediate node's BPs with new key

		st->choice[st->curr] = index; // New key will go into this place
		st->trace[st->curr] = self;
		st->curr++;
	}

	return _ChooseSubtree(self->entries[index].child, st, key, data);
}

//Insertion
int Extensions::_TreatOverflow(HNode* self, stack* Stack, HNode* nNode1, HNode* nNode2,
		HNode*& outNode1, HNode*& outNode2, int status, RootTable* RT,
		double tnow) {

	bool isOverflow = false;
	HNode *newNode1, *newNode2;
	int numKey = 0;

	if (status == 1 || status == 2) // KeySplit || VersionSplit
		numKey = 1;
	else if (status == 3) // Strong Version Split
		numKey = 2;

	//KeySplit BP Update
	if (status == 1) {
		memcpy(self->entries[Stack->choice[Stack->curr]].bp,
				self->entries[Stack->choice[Stack->curr]].child->bp, klen);
	}
	//VersionSplit BP Update
	if (status == 2 || status == 3) { // Remove the entry which is not correct due to the split of child.
		_RemoveEntry(self, Stack, tnow);
	}

	if (self->numEntry + numKey + 1 > MaxEntry)
		isOverflow = true;

	if (!isOverflow) {
		status = 0;
		if (numKey == 1) { //Insert new entry from child's Split
			self->entries[self->numEntry].child = nNode1;
			memcpy(self->entries[self->numEntry].bp, nNode1->bp, klen);
			self->numEntry++;
		} else if (numKey == 2) { //Insert new entry from child's Split
			self->entries[self->numEntry].child = nNode1;
			self->entries[self->numEntry + 1].child = nNode2;
			memcpy(self->entries[self->numEntry].bp, nNode1->bp, klen);
			memcpy(self->entries[self->numEntry + 1].bp, nNode2->bp, klen);
			self->numEntry++;
			self->numEntry++;
		}
	} else { //Overflow
		bool isKeySplit = true;
		int numRight = 0;
		double rkey[dim * 2];
		double lkey[dim * 2];

		//Init the cursor.
		pCursor *cursor = new pCursor(); //
		cursor->InsertNode(self);
		if (numKey > 0)
			cursor->InsertEntry(nNode1); // put entry into cursor
		if (numKey == 2)
			cursor->InsertEntry(nNode2); // put entry into cursor
		int rightEntries[cursor->size];

		isKeySplit = _isKeySplit(cursor, tnow);

		if (isKeySplit) { //KeySplit
			status = 1;
			_keySplit(cursor, rightEntries, numRight, rkey, lkey);
			_keySplitNode(self, cursor, rightEntries, numRight, newNode1, rkey,
					lkey);
		} else { // VersionSplit
			if (cursor->numAliveEntry() < MaxEntry * SVO_Ratio) {
				_versionSplitNode(RT, cursor, self, newNode1, tnow);
				status = 2;
			} else { // Strong Version Overflow. Do the KeySplit
				_SVOSplitNode(RT, cursor, self, newNode1, newNode2, tnow);
				status = 3;
			}
		}
		delete cursor;
	}


	outNode1 = newNode1;
	outNode2 = newNode2;

	return status;
}

void Extensions::_RemoveEntry(HNode *self, stack *Stack, double tnow) {
	if (self->entries[Stack->choice[Stack->curr]].bp[4] == tnow) { //Need to delete the entry.
		if (Stack->choice[Stack->curr] == (self->numEntry - 1)) {
			self->numEntry--;
		} else {
			memcpy(&self->entries[Stack->choice[Stack->curr]],
					&self->entries[self->numEntry - 1], sizeof(Entry));
			self->numEntry--;
		}
	} else
		self->entries[Stack->choice[Stack->curr]].bp[5] = tnow;
}

void Extensions::_versionSplitNode(RootTable* RT, pCursor *cursor, HNode* self,
		HNode*& newNode, double currentTime) {

	int rightE[cursor->size];
	int leftE[cursor->size];
	int nR = 0, nL = 0;

	for (int i = 0; i < cursor->size; i++) {
		if (cursor->entries[i].bp[5] == DBL_MAX) { // 살아있는 애들은 오른쪽
			rightE[nR] = i;
			nR++;
		}
		if (cursor->entries[i].bp[4] < currentTime) { //늙은애들은 왼쪽
			leftE[nL] = i;
			nL++;
		}
	}

	hr_rect rightRect, leftRect;
	rightRect.copyRect(cursor->entries[rightE[0]].bp);
	leftRect.copyRect(cursor->entries[leftE[0]].bp);

	for (int i = 1; i < nR; i++) {
		rightRect.expand(cursor->entries[rightE[i]].bp);
	}
	for (int i = 1; i < nL; i++) {
		leftRect.expandTime(cursor->entries[leftE[i]].bp);
	}

	rightRect.coord[4] = currentTime; // 시작 시간 변경해주고
	leftRect.coord[5] = currentTime; // 원래 있던 애들은 닫아주고

	if (self->isRoot == true) {
		RT->Root[RT->numRoot] = new HNode();
		RT->Root[RT->numRoot]->isRoot = true;
		RT->Root[RT->numRoot]->level = self->level;

		for (int i = 0; i < nL; i++) { //옮겨담으면서 시간을 닫아주자!
			memcpy(&self->entries[i], &cursor->entries[leftE[i]],
					sizeof(Entry));
			if (self->entries[i].bp[5] == DBL_MAX)
				self->entries[i].bp[5] = currentTime;
		}
		for (int i = 0; i < nR; i++) {
			memcpy(&RT->Root[RT->numRoot]->entries[i],
					&cursor->entries[rightE[i]], sizeof(Entry));
			RT->Root[RT->numRoot]->entries[i].bp[4] = currentTime;
		}

		self->numEntry = nL;
		RT->Root[RT->numRoot]->numEntry = nR;

		memcpy(self->bp, leftRect.coord, klen);
		memcpy(RT->Root[RT->numRoot]->bp, rightRect.coord, klen);

		newNode = RT->Root[RT->numRoot]; //for Output
		RT->numRoot++;

	} else {
		HNode *right = new HNode();
		right->level = self->level;

		for (int i = 0; i < nL; i++) { //옮겨담으면서 시간을 닫아주자!
			memcpy(&self->entries[i], &cursor->entries[leftE[i]],
					sizeof(Entry));
			if (self->entries[i].bp[5] == DBL_MAX)
				self->entries[i].bp[5] = currentTime; //시간을 닫아주는것 !!
		}

		for (int i = 0; i < nR; i++) {
			memcpy(&right->entries[i], &cursor->entries[rightE[i]],
					sizeof(Entry));
			right->entries[i].bp[4] = currentTime; //시작시간은 요렇게!

		}

		memcpy(self->bp, leftRect.coord, klen);
		memcpy(right->bp, rightRect.coord, klen);
		self->numEntry = nL;
		right->numEntry = nR;

		newNode = right; //right이 최근꺼!
	}
}

void Extensions::_SVOSplitNode(RootTable* RT, pCursor *cursor, HNode* self,
		HNode*& newNode1, HNode*& newNode2, double currentTime) {

	pCursor *kcursor = new pCursor();
	hr_rect leftRect;
	int nL = 0;
	// Do the version Split part of SVO
	// make a closed node (self) and make a kcursor for further keySplit.
	for (int i = 0; i < cursor->size; i++) {
		if (cursor->entries[i].bp[5] == DBL_MAX) { // 살아있는 노드는 새로운 커서에 , KeySplit 용
			memcpy(&kcursor->entries[kcursor->size], &cursor->entries[i],
					sizeof(Entry));
			kcursor->entries[kcursor->size].bp[4] = currentTime;
			kcursor->size++;
		}
		if (cursor->entries[i].bp[4] < currentTime) { // 오래전 만들어진 노드는 왼쪽, Deletion 된 노드에 들어갈 Entry들
			if (nL == 0)
				leftRect.copyRect(cursor->entries[i].bp);
			else {
				leftRect.expandTime(cursor->entries[i].bp);
			}
			memcpy(&self->entries[nL], &cursor->entries[i], sizeof(Entry));
			if (self->entries[nL].bp[5] == DBL_MAX)
				self->entries[nL].bp[5] = currentTime;
			nL++;
		}
	}
	memcpy(self->bp, leftRect.coord, klen);
	// Close the node (self)
	self->bp[5] = currentTime;
	self->numEntry = nL;

	// Do the keySplit part of SVO
	int rightE[kcursor->size];
	double rkey[dim * 2];
	double lkey[dim * 2];
	int numRight = 0;

	_keySplit(kcursor, rightE, numRight, rkey, lkey);
	_keySplitSVO(RT, self, kcursor, rightE, numRight, newNode1, newNode2, rkey,
			lkey);

	delete kcursor;
}

bool Extensions::_isKeySplit(pCursor *cursor, double tnow) {
	bool flag = true;
	for (int i = 0; i < cursor->size; i++) {
		if (cursor->entries[i].bp[4] != tnow)
			flag = false;
	}
	return flag;
}

struct xyz {
	double bp[6];
	int e;
};

bool cmp1_xyz(xyz a, xyz b) {
	return a.bp[0] < b.bp[0];
}

bool cmp2_xyz(xyz a, xyz b) {
	return a.bp[1] < b.bp[1];
}

bool cmp3_xyz(xyz a, xyz b) {
	return a.bp[2] < b.bp[2];
}

bool cmp4_xyz(xyz a, xyz b) {
	return a.bp[3] < b.bp[3];
}

bool cmp5_xyz(xyz a, xyz b) {
	return a.e < b.e;
}

void Extensions::_keySplit(pCursor *Cursor, int rightE[], int& nR, double* rkey,
		double* lkey) {

	//나눴을때 많은 애가 오른쪽
	double temp = 0.0;
	double temp_area = 0.0;
	double min_over = DBL_MAX;
	double min_area = DBL_MAX;
	int minSize = minSplitRatio * MaxEntry; //smallest number of entries in a HNode;

	int index = 0;
	int axis = 0;
	int size = Cursor->size;

	xyz *ent = new xyz[size];
	xyz *best = new xyz[size];

	for (int i = 0; i < size; i++) {
		memcpy(ent[i].bp, Cursor->entries[i].bp, klen);
		ent[i].e = i;
	}

	int margin_x = 0, margin_y = 0;
	int area_x = 0, area_y = 0;
	int temp_x = 0;
	for (int i = 0; i < 4; i++) {
		if (i == 0)
			sort(ent, ent + size, cmp1_xyz); //i==0  = > x_min 값으로 정렬한것
		if (i == 1)
			sort(ent, ent + size, cmp3_xyz); //i==1  = > x_max 값으로 정렬한것
		if (i == 2)
			sort(ent, ent + size, cmp2_xyz); //i==2  = > y_min 값으로 정렬한것
		if (i == 3)
			sort(ent, ent + size, cmp4_xyz); //i==3  = > y_max 값으로 정렬한것
		//n-1 가지의 dist 에 대하여 sum of margin을 구한다.

		temp = 0.0;
		temp_x = 0.0;
		for (int j = minSize; j < size - minSize - 1; j++) {
			hr_rect temp1, temp2;
			temp1.copyRect(ent[0].bp);
			temp2.copyRect(ent[size - 1].bp);
			for (int k = 0; k < j; k++) {
				temp1.expand(ent[k].bp);
			}
			for (int k = j + 1; k < size; k++) {
				temp2.expand(ent[k].bp);
			}
			temp = temp + temp1.margin() + temp2.margin();
			temp_x += (temp1.span() + temp2.span());
			temp1.dealloc();
			temp2.dealloc();
		}

		if (i == 0 || i == 1) {
			margin_x += temp;
			area_x += temp_x;
		} else {
			margin_y += temp;
			area_y += temp_x;
		}
	}

	// 어떤 축으로 정렬해야 가장 적은 Margin 인지 골랐다!
	// 그렇게 다시 한번 소팅을 하고!
	if (margin_x < margin_y) { // BP = Xmin Ymin Xmax Ymax순서로 들어온다.
		sort(ent, ent + size, cmp1_xyz); //i==0,2  = > Min값으로 정렬한것
		axis = 0; //X축으로 정렬!
	} else if (margin_x > margin_y) {
		sort(ent, ent + size, cmp2_xyz); //i==1,3  = > Min값으로 정렬한것
		axis = 1; //Y축으로 정렬!
	} else {
		if (area_x > area_y) {
			sort(ent, ent + size, cmp2_xyz); //i==0,2  = > Min값으로 정렬한것
			axis = 1; //Y축으로 정렬!
		} else {
			sort(ent, ent + size, cmp1_xyz); //i==0,2  = > Min값으로 정렬한것
			axis = 0; //X축으로 정렬!
		}
	}
	//Sorting Ends
	//Under the sorting, find the best cut that minimize Overlap among n-1 possibilities
	////////////////////////////////////////////////////////////////////////////////////

	// 2* (n-2*k)개 중 베스트 컷을 찾자!
	int axis_rec = 0;
	for (int t = 0; t < 2; t++) {
		hr_rect r1, r2;
		r1.copyRect(ent[0].bp);
		r2.copyRect(ent[size - 1].bp);

		for (int k = 1; k < minSize; k++) {
			r1.expand(ent[k].bp); //left
			r2.expand(ent[size - k - 1].bp); //right
		} //양 끝 minSize개를 미리 넣어놓고, 그 사이에 있는걸 쪼개보자 ㅇㅇ

		for (int i = minSize - 1; i < size - minSize - 1; i++) { //자르는 곳의 위치 0~i까지 왼쪽, 나머진 오른쪽에 넣을 것.
			temp = 0.0;
			temp_area = 0.0;
			hr_rect ll, rr;
			ll.copyRect(r1.coord);
			rr.copyRect(r2.coord);

			for (int k = minSize; k <= i; k++) {
				ll.expand(ent[k].bp); //0~ (i-1)까지 넣어보자 ㅇㅇ
			}
			for (int k = i + 1; k <= size - minSize - 1; k++) {
				rr.expand(ent[k].bp);
			}

			temp = rr.overlap(&ll);
			if (temp <= min_over) {
				//if overlap = 0, then try to find a smallest area sum cut
				if (temp == min_over) {
					temp_area = rr.span() + ll.span();
					if (temp_area < min_area) {
						min_area = temp_area;
						index = i;
						axis_rec = axis;
						memcpy(best, ent, sizeof(xyz) * (size));
						memcpy(lkey, ll.coord, klen);
						memcpy(rkey, rr.coord, klen);
					}
				} else {
					min_over = temp;
					index = i;
					axis_rec = axis;
					memcpy(best, ent, sizeof(xyz) * (size));
					memcpy(lkey, ll.coord, klen);
					memcpy(rkey, rr.coord, klen);
				}
			}
			rr.dealloc();
			ll.dealloc();
		} // 몇번째 index인지 찾고

		r1.dealloc();
		r2.dealloc();

		if (axis == 0) {
			axis += 2;
			sort(ent, ent + size, cmp3_xyz); //Max값으로 정렬
		} else {
			axis += 2;
			sort(ent, ent + size, cmp4_xyz); //Max값으로 정렬
		}
	}
	// index를 이용해 rightE를 채워넣는다.
	nR = 0;
	for (int i = index + 1; i < size; i++) { // r1을 여기에 담는 것임!
		if (best[i].e < size) {
			rightE[nR] = best[i].e;
			nR++;
		}
	}

	delete[] ent;
	delete[] best;
	// 그 축으로 정렬한 뒤 나오는 n-3가지 split중 가장 적은 overlap이 발생하는 split을 찾는다.

}

//
void Extensions::_keySplitSVO(RootTable *RT, HNode* self, pCursor *cursor, int rightE[],
		int numRight, HNode*& newNode1, HNode*& newNode2, double *rkey,
		double *lkey) {
	if (self->isRoot == true) {
		HNode *right = new HNode();
		HNode *left = new HNode();
		right->level = self->level;
		left->level = self->level;
		right->numEntry = 0;
		left->numEntry = 0;
		for (int i = 0; i < cursor->size; i++) {
			bool temp_flag = false;
			for (int j = 0; j < numRight; j++) {
				if (rightE[j] == i)
					temp_flag = true;
			}
			if (temp_flag) {
				memcpy(&right->entries[right->numEntry], &cursor->entries[i],
						sizeof(Entry));
				right->numEntry++;
			} else {
				memcpy(&left->entries[left->numEntry], &cursor->entries[i],
						sizeof(Entry));
				left->numEntry++;
			}
		}

		memcpy(left->bp, lkey, klen);
		memcpy(right->bp, rkey, klen);

		//Make a new Root
		RT->Root[RT->numRoot] = new HNode();
		RT->Root[RT->numRoot]->isRoot = true;
		RT->Root[RT->numRoot]->level = self->level + 1;
		RT->Root[RT->numRoot]->numEntry = 2;
		RT->Root[RT->numRoot]->entries[0].child = left;
		RT->Root[RT->numRoot]->entries[1].child = right;

		memcpy(RT->Root[RT->numRoot]->entries[0].bp, lkey, klen);
		memcpy(RT->Root[RT->numRoot]->entries[1].bp, rkey, klen);

		hr_rect keyRect(lkey, 0);
		keyRect.expand(rkey);
		memcpy(RT->Root[RT->numRoot]->bp, keyRect.coord, klen);
		RT->numRoot++;
		newNode1 = left;
		newNode2 = right;

	} else {
		HNode *right = new HNode();
		HNode *left = new HNode();
		right->level = self->level;
		left->level = self->level;
		right->numEntry = 0;
		left->numEntry = 0;
		for (int i = 0; i < cursor->size; i++) {
			bool temp_flag = false;
			for (int j = 0; j < numRight; j++) {
				if (rightE[j] == i)
					temp_flag = true;
			}
			if (temp_flag) {
				memcpy(&right->entries[right->numEntry], &cursor->entries[i],
						sizeof(Entry));
				right->numEntry++;
			} else {
				memcpy(&left->entries[left->numEntry], &cursor->entries[i],
						sizeof(Entry));
				left->numEntry++;
			}
		}
		memcpy(left->bp, lkey, klen);
		memcpy(right->bp, rkey, klen);
		newNode1 = left;
		newNode2 = right;
	}
}

void Extensions::_keySplitNode(HNode* self, pCursor *cursor, int rightE[], int numRight,
		HNode*& newNode, double *rkey, double *lkey) {
	if (self->isRoot == true) {
		HNode *right = new HNode();
		HNode *left = new HNode();
		right->level = self->level;
		left->level = self->level;
		right->numEntry = 0;
		left->numEntry = 0;
		for (int i = 0; i < cursor->size; i++) {
			bool temp_flag = false;
			for (int j = 0; j < numRight; j++) {
				if (rightE[j] == i)
					temp_flag = true;
			}
			if (temp_flag) {
				memcpy(&right->entries[right->numEntry], &cursor->entries[i],
						sizeof(Entry));
				right->numEntry++;
			} else {
				memcpy(&left->entries[left->numEntry], &cursor->entries[i],
						sizeof(Entry));
				left->numEntry++;
			}
		}
		self->level++; // 자기 자신은 하나 올라가고
		self->numEntry = 2;
		self->entries[0].child = left;
		self->entries[1].child = right;
		memcpy(self->entries[0].bp, lkey, klen);
		memcpy(self->entries[1].bp, rkey, klen);
		memcpy(left->bp, lkey, klen);
		memcpy(right->bp, rkey, klen);

		hr_rect keyRect(lkey, 0);
		keyRect.expand(rkey);
		memcpy(self->bp, keyRect.coord, klen);
		newNode = right;
	} else {
		HNode *right = new HNode();
		right->level = self->level;

		int tempR = 0, tempL = 0;
		bool temp_flag;

		for (int i = 0; i < cursor->size; i++) { //옮겨닮자
			temp_flag = false;
			for (int j = 0; j < numRight; j++) {
				if (rightE[j] == i)
					temp_flag = true;
			}
			if (temp_flag) {
				memcpy(&right->entries[tempR], &cursor->entries[i],
						sizeof(Entry));
				tempR++;
			} else {
				memcpy(&self->entries[tempL], &cursor->entries[i],
						sizeof(Entry));
				tempL++;
			}
		}
		right->numEntry = tempR;
		self->numEntry = tempL;
		memcpy(self->bp, lkey, klen);
		memcpy(right->bp, rkey, klen);
		newNode = right; //for Output
	}
}

void Extensions::_printStack(stack* st) {
	cout << "curr : " << st->curr << " depth : " << st->depth << endl;
	if (st->depth > 0) {
		for (int i = 0; i < st->depth; i++) {
			cout << st->choice[i] << " -> ";
		}
		cout << endl;
	} else
		cout << "	stack is empty" << endl;
}

int Extensions::_Consistent(double *bp, double *key) {
	hr_rect r1(bp);
	hr_rect r2(key);
	if (r1.intersect(&r2)) {
		r1.dealloc();
		r2.dealloc();
		return 1;
	} else {
		r1.dealloc();
		r2.dealloc();
		return 0;
	}
}
bool Extensions::_calcAliveNewBP(HNode *nNode, double *newKey) {

	int numAlive = 0;
	int flag = true;
	hr_rect newBP;
	for (int i = 0; i < nNode->numEntry; i++) {
		if (flag == true && nNode->entries[i].bp[5] == DBL_MAX) {
			flag = false;
			newBP.copyRect(nNode->entries[i].bp);
			numAlive++;
		} else if (nNode->entries[i].bp[5] == DBL_MAX) {
			newBP.expandTime(nNode->entries[i].bp);
			numAlive++;
		}
	}

	if (numAlive == 0) {
		newBP.dealloc();
		return false;
	} else {
		memcpy(newKey, newBP.coord, klen);
		newBP.dealloc();
		return true;
	}
}
bool Extensions::_calcDeadNewBP(HNode *nNode, double *newKey) {

	int numAlive = 0;
	int flag = true;
	hr_rect newBP;
	for (int i = 0; i < nNode->numEntry; i++) {
		if (nNode->entries[i].bp[5] == DBL_MAX) {
			numAlive++;
		}
		if (flag == true && nNode->entries[i].bp[5] != DBL_MAX) {
			flag = false;
			newBP.copyRect(nNode->entries[i].bp);
		} else if (nNode->entries[i].bp[5] != DBL_MAX) {
			newBP.expandTime(nNode->entries[i].bp);
		}
	}

	if (numAlive == 0) {
		memcpy(newKey, newBP.coord, klen);
		newBP.dealloc();
		return true;
	} else {
		newBP.dealloc();
		return false;
	}
}
void Extensions::_InsertEntry(HNode *nNode, HNode *child) {
	int numEntry = nNode->numEntry;
	memcpy(nNode->entries[numEntry].bp, child->bp, klen);
	nNode->entries[numEntry].child = child;
	nNode->numEntry++;
}
/*=================================================
 *=======================Debug=====================
 *================================================= */

void Extensions::_printNode(HNode* HNode) {
	int lvl = HNode->level;

	if (HNode->isRoot)
		cout << "Root" << endl;
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
