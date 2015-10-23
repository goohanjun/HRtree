/*
 * Command.cpp
 *
 *  Created on: Aug 22, 2015
 *      Author: hanjunkoo
 */

#include "Command.h"
#include "Extension.h"
#include <iostream>

using namespace std;


int CommandInsert(Node* root, double* key, int klen, int data, int dlen, Node* Root[], int & numRoot){
	//cout << "CommandInsert : root->Insert(k,kl,d,dl)" <<endl;
	if (Insert(root, key,klen,data,dlen, Root, numRoot)){
		return 1;
	}
	else{
		cerr << "Insertion is Failed,  key[0] = " <<key[0]<<endl;
		return 0;
	}
}

int CommandDelete(Node* root, double* key, int klen, int data, int dlen, Node* Root[], int & numRoot){
	if (Delete(root, key,klen,data,dlen, Root, numRoot)){
		return 1;
	}
	else{
		cerr << "Deletion is Failed,  key[0] = " <<key[0]<<endl;
		return 0;
	}
}
int CommandSearch(Node* root, double* key, int klen, int data, int dlen, Node* Root[], int & numRoot){
	if (Search(root, key,klen,data,dlen, Root, numRoot)){
		return 1;
	}
	else{
		cerr << "Searching is Failed,  key[0] = " <<key[0]<<endl;
		return 0;
	}
}
void CommandNewRoot(){


}
void CommandDump(Node* node){
	CommandPrint(node);
	if(node->level > 1){
		for(int i =0; i< node->numEntry;i++){
			CommandPrint(node->entries[i].child);
		}
	}
}
void CommandPrint(Node* node){
	int lvl = node -> level;
	if (node->isRoot)
		cout<<"Root     ";
	cout<<"level: "<<lvl<<" # slots: "<<node->numEntry<<", Avail: "<<numEnt_MAX-node->numEntry<<endl;

	cout<<"\tbp: <     ";
	for (int i =0;i<dim*2;i++){
		if(node->bp[i]==DBL_MAX)
			cout<<"* ";
		else
			cout<<node->bp[i]<<" ";
	}
	cout<<">\n";


	for(int i =0;i<node->numEntry;i++){
		cout<<"\t["<<i<<"] <";
		cout<<" bp: ";
		for(int j=0;j<dim*2;j++){
			if((double)node->entries[i].bp[j]==DBL_MAX)
				cout<<"* ";
			else
				cout<<(double)node->entries[i].bp[j]<<" ";
		}
		cout<<" >";
		if(lvl==1)
			cout<<" data: "<<node->entries[i].data<<endl;
		else
			cout<<" child: "<<node->entries[i].child->numEntry<<endl;
	}
}
