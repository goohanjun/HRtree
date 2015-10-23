/*
 * Extension.cpp
 *
 *  Created on: Sep 10, 2015
 *      Author: hanjunkoo
 */

#include "Extension.h"


int Insert(Node* root, double* key, int klen, int data, int dlen, Node* Root[], int & numRoot){
	//cout<<"Insertion Start!"<<endl;

	//Root가 Leaf노드가 아니고, Root노드 아래가 다 닫혀있다면, 새로운 루트를 생성해야한다! (Deletion때문에 발생 가능)
	bool flag_alive = false; //죽어있다고 가정.
	if (root->level !=1){
		if(root->_numAlive(key[4]) != 0){
			flag_alive = true;
		}
		if(flag_alive==false)
			return 0;
	}

	Node *Path[MAX_DEPTH_SIZE]={};
	int trace[MAX_DEPTH_SIZE]={};
	int numTrace=0;
	int status = 0;

	_Path(root, Path, trace, numTrace, key,klen); // Path와 Trace, numTrace 가 채워짐!

	Node *leaf = Path[numTrace]; //leaf 노드 설정
	Node *node1,*node2,*node3,*node4;
	int depth =numTrace;
	bool bpChanged= false;
	bool check = true;

	for(int i =0;i<leaf->numEntry;i++){//만약 Entry들이 다 같은 시간대에 만들어진 거라면. true
		if(leaf->entries[i].bp[4] < key[4]){
			check = false;
			break;
		}
	}//다 시간이 같으면 check = true;


	if(leaf->numEntry   <  numEnt_MAX){ // Overflow 가 나질 않아서 Insert Leaf 후 UpdateParent 부르고 끝남
		//cout<<"Insertion without Split"<<endl;

		memcpy( leaf->entries[leaf->numEntry].bp, key, klen);
		leaf->entries[leaf->numEntry].pos = true;
		leaf->entries[leaf->numEntry].data= data;
		leaf->entries[leaf->numEntry].dlen= dlen;
		leaf->numEntry++;

		hr_rect r1 ( leaf->bp ,klen);//현재 BP
		double temp_start = leaf->bp[4];


		if(r1.expand(key)){ //살아있는 애들의 BP만 저장되어있다고 생각하자!
			bpChanged= true;
			memcpy(leaf->bp, r1.coord, klen);
		}

		//Status 스플릿 없이 만약 시간이 바뀐데다가 비피까지 바뀌면, 새로운 애를 넣어주어야함
		if ( !check  &&  bpChanged ){ //이래야 새로운게 추가 될 일말의 가능성이라도 있음ㅇㅇㅇ
			status = 1;
		}
		r1.dealloc();

		if (!leaf->isRoot && bpChanged ){
			//Path[depth] = leaf node
			//Path[--depth] = leaf node's parent.
			depth--;
			Path[depth]->_UpdateParent(Path, trace, depth, numTrace, status, leaf, node2,Root, numRoot, key[4]);
		}
	}

	//Todo: 여기서부터 Bp가 살아있는 것들만 저장하는 애인걸 신경써주면 된다!

	else{ //Overflow!!
		int rightEntries[numEnt_MAX]={},leftEntries[numEnt_MAX]={};
		int numRight=0,numLeft=0, numKey=1;
		int SStatus; //Split Status // 1= key, 2 =version , 3= key without key (SVO)
		bool oneGoesRight=false,twoGoesRight=false;
		double *rkey = new double[dim*2];
		double *lkey = new double[dim*2];

		int depth = numTrace-1; //부모로 올라갈거라 미리 1을 빼준다

		if(check){ // key Split
			cout<<"KeySplit!"<<endl;
			SStatus = 1; // key Split for _SplitNode
			status = 2;//for UpdateParent
			leaf->_Ksplit(numKey, key,key,oneGoesRight,twoGoesRight,rightEntries, numRight, rkey, lkey);
			leaf->_SplitNode(rkey,lkey, rightEntries, numRight, leftEntries ,numLeft,
							SStatus, Root, numRoot, node1, node2,leaf);
			//오른쪽에 갈까 ? 왼쪽에 갈까?
			if(oneGoesRight){
				memcpy(node2->entries[node2->numEntry].bp,key, klen);
				node2->entries[node2->numEntry].data=data;node2->entries[node2->numEntry].dlen=dlen;
				node2->entries[node2->numEntry].pos=true;
				node2->numEntry++;
			}
			else{
				memcpy(node1->entries[node1->numEntry].bp,key, klen);
				node1->entries[node1->numEntry].data=data;node1->entries[node1->numEntry].dlen=dlen;
				node1->entries[node1->numEntry].pos=true;
				node1->numEntry++;
			}
			//cout<<"KeySplit Over"<<endl;
		}
		else{ //Version Split
			cout<<"VersionSplit!"<<endl;
			SStatus = 2; // version Split for _SplitNode
			status = 3;//for UpdateParent
			leaf->_Vsplit(numKey,key,key, rkey,lkey,rightEntries,numRight,leftEntries,numLeft);
			leaf->_SplitNode(rkey,lkey, rightEntries, numRight, leftEntries ,numLeft,
				SStatus, Root, numRoot, node1, node2,leaf);

			if(!leaf->isRoot){
				Path[depth]->_deleteParent(trace, depth, key[4]);//부모에게서, 중복된 필요없는 엔트리를 제거해준다.
			}
			if( node2->numEntry >= 0.85 * numEnt_MAX){//SVO
				cout<<"Insertion LEAF_SVO"<<endl;
				numKey = 1;numRight = 0;
				status = 4;//for UpdateParent
				SStatus = 1; // key Split for _SplitNode
				node2->_Ksplit(numKey,key,key,oneGoesRight,twoGoesRight,rightEntries, numRight,rkey,lkey);
				node2->_SplitNode(rkey,lkey, rightEntries, numRight, leftEntries ,numLeft,
								SStatus, Root, numRoot, node3, node4,node2);

				if(oneGoesRight){
					memcpy(node4->entries[node4->numEntry].bp, key, klen);
					node4->entries[node4->numEntry].data=data;node4->entries[node4->numEntry].dlen=dlen;
					node4->entries[node4->numEntry].pos=true;
					node4->numEntry++;
				}else{
					memcpy(node3->entries[node3->numEntry].bp, key, klen);
					node3->entries[node3->numEntry].data=data;node3->entries[node3->numEntry].dlen=dlen;
					node3->entries[node3->numEntry].pos=true;
					node3->numEntry++;
				}
			}
			else{
				//VersionSplit without SVO , 무조건 오른쪽에 넣어주면 된다.
				memcpy(node2->entries[node2->numEntry].bp,key, klen);
				node2->entries[node2->numEntry].data=data;node2->entries[node2->numEntry].dlen=dlen;
				node2->entries[node2->numEntry].pos=true;
				node2->numEntry++;
			}
		}

		delete [] rkey;
		delete [] lkey;
		if(!leaf->isRoot){
			if(status==4) // SVO
				Path[depth]->_UpdateParent(Path, trace, depth ,numTrace, status, node3, node4,Root, numRoot,key[4]); //leaf노드의 아빠
			else // key split || version Split
				Path[depth]->_UpdateParent(Path, trace, depth ,numTrace, status, node2, node1,Root, numRoot,key[4]); //leaf노드의 아빠
		}//node2 =새로 나온거, node1 원래 자신//먼저 넣어주어야 하는것 순서대로 넣자 노드2 -> 노드 1
	}
	cout<<"Insertion is Over"<<endl;
	return 1;
}

int Delete(Node* root, double* key, int klen, int data, int dlen, Node* Root[], int & numRoot){
	return 1;
}



int Search(Node* self, double* key, int klen, int data, int dlen, Node* Root[], int & numRoot){
	for(int i =0;i< self->numEntry;i++){
		if( self->level == 1){
			cout<<"Searched data = "<<self->entries[i].data<< endl;
			return 1;
		}
		if (_Consistent(self->entries[i].child->bp, key,klen))
			Search (self->entries[i].child, key,klen, data, dlen,Root,numRoot);
	}
	return 1;
}
int _Path(Node* self, Node* Path[], int trace[], int& numTrace, double* key, int klen){
	if (self->level==1){
		Path[numTrace]= self;
		trace[numTrace] = 0;
		return 0;
	}
	else{
		int index;
		if(self->level==2)
			index = self->_findMinPenOver(key);
		else
			index = self->_findMinPen(key);
		trace[numTrace]= index;
		Path[numTrace] = self; //자기 자신을 넣고, 몇번째 아이로 가는지도 넣고!
		numTrace++;
		_Path( self->entries[index].child , Path, trace, numTrace,key, klen);
	}
	return 0; //for removing warning
}






int _Consistent(double *bp,double *key, int klen){
	hr_rect r1(bp, klen);
	hr_rect r2(key, klen);
	if(r1.intersect(&r2)){
		r1.dealloc();r2.dealloc();
		return 1;
	}
	else{
		r1.dealloc();r2.dealloc();
		return 0;
	}
}
























