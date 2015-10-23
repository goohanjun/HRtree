/*
 * Node.cpp
 *
 *  Created on: Aug 22, 2015
 *      Author: hanjunkoo
 */
#include <iostream>
#include <algorithm>
#include <string.h>

#include "Node.h"
#include "hr_defs.h"
#include "hrext.h"

using namespace std;

Node::Node() {
	// TODO Auto-generated constructor stub
	 bp = {};
	 isRoot = false;
	 level = 1;
	 numEntry = 0;
};

Node::~Node() {
	// TODO Auto-generated destructor stub
};

//status == 0 -> BP updates only, status==1 -> Time is changed status ==2 -> key, vsplit, status ==3 -> SVO
int Node::_UpdateParent(Node* Path[], int trace[], int& depth, int& numTrace, int status, Node* child1, Node* child2,
		Node* Root[], int & numRoot,double tnow){

	Node * Sib1;Node * Sib2;Node * Sib3;Node * Sib4;
	int delta = 0;

	bool bpChanged= false;
	bool SVO = false;
	bool check=true;

	int klen = sizeof(double)*dim*2;

	for (int i =0;i<numEntry;i++){ //만약 Entry들이 다 같은 시간대에 만들어진 거라면. True
		if(entries[i].bp[4] != tnow)
			check= false;
		cout<< entries[i].bp[4]<<endl;
	}//다 시간이 같으면 check = true;

	int stat;//for updateParent


	if(entries[trace[depth]].bp[4] == tnow && status==1) //자기가 타고온 애랑 시간이 같으면? 이거 조건 추가해줘야함!
		status=0;
	//BP가 뭔가 바뀌어서 업데이트페런츠로 왔는데, 시간이 같아버리면 새로운 앨 인서트하기 뭐하니까 기존껄 업데이트해주는 식으로 가야지 ㅇㅇ//

	if(status==0){ // 추가 안하고 bp만 업데이트 해주면 됩니다.
		hr_rect r1 ( bp ,klen); //현재 BP
		hr_rect r2 ( entries[trace[depth]].child->bp ,klen);// 방금 날 호출한 child의 BP // Todo: 살아있는애들만 해야함
		double t_temp = entries[trace[depth]].bp[4];
		stat = 0;
		bpChanged= (r1.expand(&r2)==true) ? true:false;
		memcpy( bp, r1.coord, klen);
		memcpy( entries[trace[depth]].bp, entries[trace[depth]].child->bp ,klen);
		entries [trace[depth]].bp[4]  = t_temp;
		r1.dealloc();r2.dealloc();
	}
	else{ //status ==1 (스플릿없이 인서트) , 2 (키스플릿하고 인서트), 3(버전스플릿하고 인서트) 4 (스플릿 2번,SVO 하고 인서트)
		delta = (status==4) ? 2:1; //Split 이 났거나, BP가 변해서 새로운 엔트리를 추가해줘야하는 것.
		////////Insertion in _UpdateParent//////////////
		if(numEntry+delta <= numEnt_MAX){ // Fit in
			cout<<"_UpdateParent fit in"<<endl;

			stat=1;
			if(status==1){ //스플릿 없이 엔트리 추가하게되는 상황
				entries[numEntry].child = entries[trace[depth]].child; //새로 추가해줄 녀석
				memcpy(entries[numEntry].bp, entries[trace[depth]].child->bp ,klen);
				entries[trace[depth]].bp[5]=tnow; //기존꺼의 시간은 사정없이 닫아줍시다.
				entries[numEntry].bp[4]= tnow;//새로 만들어주는건 시간을 잘라서 넣어줍시다.
				entries[numEntry].pos=false; //중복해서 추가해주는 거는 false를 넣어줍시다
			}else{ //스플릿 때문에 엔트리를 추가하는 상황


				hr_rect r(bp, klen);
				hr_rect r1(child1->bp,klen);

				//child1 삽입
				entries[numEntry].child = child1;
				memcpy(entries[numEntry].bp, child1->bp,klen);
				entries[numEntry].pos= true;

				if(r.expand(&r1))
					bpChanged = true;

				//child2 삽입
				if( delta == 2 ){
					entries[numEntry+1].child = child2;
					memcpy(entries[numEntry+1].bp, child2->bp,klen);
					entries[numEntry+1].pos= true;
					hr_rect r2(child2->bp, klen);
					if(r.expand(&r2))
						bpChanged = true;
					r2.dealloc();
				}
				memcpy(bp, r.coord, klen); //기존의 BP 업데이트
				r.dealloc(); r1.dealloc();


			}
			numEntry+=delta;
		}
		else{ //OverFlow Occured!// Split 한 뒤에 중복Entry를 넣어주고 //  Sib1,2를 넣고 업데이트 패런츠를 부르자!!!!
			cout<<"_UpdateParent Overflow, depth = "<<depth<<" Delta = "<<delta<<" Status = "<<status<<endl;
			bpChanged = true;
			bool oneGoesRight= false, twoGoesRight=false;
			int rightEntries[numEnt_MAX]={},leftEntries[numEnt_MAX]={};
			double *rkey = new double[dim*2];double *lkey = new double[dim*2];
			int numRight=0,numLeft=0;
			int SStatus; //Split Status // 1= key, 2 =version , 3= key without key (SVO)

			double *tempKey = new double[dim*2];
			if (status==1){
				memcpy(tempKey,entries[trace[depth]].child->bp,klen);
			}

			//status==1 인 경우에, Version Split을 하면 안 넣어줘도 된다!
			if(check){ //keySplit
				SStatus= 1;stat=2;
				cout<<"Parent Key Split?"<<endl;
				if (status==1) //Bp가 바뀌고 시간이 달라서 위에 추가하러 온 것
					_Ksplit(delta, tempKey,tempKey,oneGoesRight,twoGoesRight,rightEntries, numRight, rkey, lkey);
				else
					_Ksplit(delta, child1->bp,child2->bp,oneGoesRight,twoGoesRight,rightEntries, numRight, rkey, lkey);
				_SplitNode(rkey,lkey, rightEntries, numRight, leftEntries ,numLeft,
						SStatus, Root, numRoot, Sib1, Sib2,Path[depth]); //Path[depth] ==자기 자신
			}
			else{ // Vsplit 처리하자!!
				cout<<"Parent Version Split?"<<endl;
				SStatus= 2;stat=3;
				if (status==1)
					_Vsplit(delta,tempKey,tempKey, rkey,lkey,rightEntries,numRight,leftEntries,numLeft);
				else
					_Vsplit(delta,child1->bp,child2->bp, rkey,lkey,rightEntries,numRight,leftEntries,numLeft);

				_SplitNode(rkey,lkey, rightEntries, numRight, leftEntries ,numLeft,
						SStatus, Root, numRoot, Sib1, Sib2,Path[depth]);

				if (!isRoot) //Vsplit 후에는 Delete Parent를 불러줘야함 ㅇㅇ
					Path[depth-1]->_deleteParent(trace,depth-1,tnow);

				if(Sib2->numEntry >= 0.85 * numEnt_MAX){
					SStatus = 3;SVO=true;stat=4;
					cout<<"Parent Version Split? SVO!!!!"<<endl;
					Sib2->_Ksplit(delta, child1->bp,child2->bp,oneGoesRight,twoGoesRight,rightEntries, numRight, rkey, lkey);
					Sib2->_SplitNode(rkey,lkey, rightEntries, numRight, leftEntries ,numLeft,
							SStatus, Root, numRoot, Sib3, Sib4,Sib2);
				} //SVO Split 끝! 넣어줘야지!?

				//SVO 상황에서만 넣어준다.status==1 인 경우에는 넣어주지 않아도 된다!.
				if (status != 1 && SVO) { //SVO 일땐 넣어주는게 특이하니까;
					//Split후에 원소 넣어주기
					if (oneGoesRight) {
						Sib4->entries[Sib4->numEntry].child = child1;
						memcpy(Sib4->entries[Sib4->numEntry].bp,child1->bp,klen);
						Sib4->numEntry++;
					} else {
						Sib3->entries[Sib3->numEntry].child = child1;
						memcpy(Sib3->entries[Sib3->numEntry].bp,child1->bp,klen);
						Sib3->numEntry++;
					}
					if (twoGoesRight && delta == 2) {
						Sib4->entries[Sib4->numEntry].child = child2;
						memcpy(Sib4->entries[Sib4->numEntry].bp,child2->bp,klen);
						Sib4->numEntry++;
					} else if (!twoGoesRight && delta == 2) {
						Sib3->entries[Sib3->numEntry].child = child2;
						memcpy(Sib3->entries[Sib3->numEntry].bp,child2->bp,klen);
						Sib3->numEntry++;
					}
				}
			} //나누는 것 끝!!!

			//현재 페런츠에서 SVO 없이 key나 버전만 일어났을때 넣어주는것
			if (status != 1 && !SVO) { //스플릿이 일어났을때 원소 넣어주는 것!
				//Split후에 원소 넣어주기

				if (stat == 3) //버전스플릿이면 무조건 오른쪽으로 가니까.
					oneGoesRight = true;

				if (oneGoesRight) {

					//Todo 구현되어야함

				}
				else{

				}
				if(delta==2 && twoGoesRight){

				}
				entries[numEntry].child = entries[trace[depth]].child;
				memcpy(entries[numEntry].bp, entries[trace[depth]].child->bp,klen);
				entries[numEntry].bp[4] = tnow; //새로 만들어주는건 시간을 잘라서 넣어줍시다.
				entries[trace[depth]].bp[5] = tnow; //기존꺼의 시간은 사정없이 닫아줍시다.

				entries[numEntry].pos = false; //중복해서 추가해주는 거는 false를 넣어줍시다

				entries[numEntry].child = entries[trace[depth]].child;
				memcpy(entries[numEntry].bp, entries[trace[depth]].child->bp,klen);
				entries[numEntry].bp[4] = tnow; //새로 만들어주는건 시간을 잘라서 넣어줍시다.
				entries[trace[depth]].bp[5] = tnow; //기존꺼의 시간은 사정없이 닫아줍시다.

				entries[numEntry].pos = false; //중복해서 추가해주는 거는 false를 넣어줍시다

			}
			//SVO 없이 스플릿 없이 인서트가 일어났을때 넣어주는것
			else if(status == 1 && !SVO ){ //스플릿 없이 Insertion 해서 넣어주는 추가적인 원소!
				if (oneGoesRight) {
					Sib2->entries[Sib2->numEntry].child = child1;

					Sib2->numEntry++;
				} else {
					Sib1->entries[Sib1->numEntry].child = child1;
					Sib1->numEntry++;
				}
				if (twoGoesRight && delta == 2) {
					Sib2->entries[Sib2->numEntry].child = child2;
					Sib2->numEntry++;
				} else if (!twoGoesRight && delta == 2) {
					Sib1->entries[Sib1->numEntry].child = child2;
					Sib1->numEntry++;
				}
			}
		}
	} //Over flow || Fit-in  각각에서 모든 status 상황에서 처리 완료

	if(!isRoot && bpChanged){
		depth--;
		if(!SVO)
			Path[depth]->_UpdateParent(Path,trace,depth, numTrace, stat, Sib2 ,Sib1,Root,numRoot,tnow);
		else
			Path[depth]->_UpdateParent(Path,trace,depth, numTrace, stat, Sib3 ,Sib4,Root,numRoot,tnow);
	}
	return 0;
};



int Node::_SplitNode(double *rkey,double *lkey,  int rightE[], int nR,  int leftE[] , int nL,
				int& Status, Node* Root[], int & numRoot,Node*& nNode1,Node*& nNode2,Node *self){
	int klen = sizeof(double)*dim*2;
	if( isRoot == true){//Root Node Split!
		if(Status ==1){//Root key Split
			cout<<"Root Key Split"<<endl;
			Node *right = new Node();Node *left = new Node();
			right->level=level; left->level=level;
			right->numEntry=0;left->numEntry=0;
			for(int i =0;i<numEntry;i++){
				bool temp_flag=false;
				for(int j =0;j<nR;j++){
					if( rightE[j] ==i )
						temp_flag = true;
				}
				if( temp_flag){
					memcpy(&right->entries[right->numEntry], &entries[i], sizeof(Entry));
					right->numEntry++;
				}
				else{
					memcpy(&left->entries[left->numEntry], &entries[i], sizeof(Entry));
					left->numEntry++;
				}
			}
			level++;// 자기 자신은 하나 올라가고
			numEntry = 2;
			entries[0].child=left;entries[1].child=right;
			memcpy(entries[0].bp, lkey,klen);
			memcpy(entries[1].bp, rkey,klen);
			memcpy(left->bp, lkey,klen);
			memcpy(right->bp, rkey,klen);
			nNode1 = left;	nNode2 = right; // For output!
		}
		else { //Root version Split 새로운 루트를 만들자!
			//New Root를 생성해줘야겠지?
			cout<<"Root Version Split"<<endl;
			Node *temp =new Node(); temp->copyNode(self);

			Root[numRoot]= new Node();
			Root[numRoot]->isRoot=true;Root[numRoot]->level=level;

			for ( int i = 0; i< nL;i++){ //옮겨담으면서 시간을 닫아주자!
				memcpy(&self->entries[i],&temp->entries[leftE[i]],sizeof(Entry));
				self->entries[i].bp[5]=rkey[4]; //시간을 닫아주는것 !!

			}
			for ( int i = 0; i< nR;i++){
				memcpy(&Root[numRoot]->entries[i],&temp->entries[rightE[i]],sizeof(Entry));
			}

			self->numEntry = nL;Root[numRoot]->numEntry = nR;
			memcpy(self->bp,lkey ,klen);memcpy(Root[numRoot]->bp,rkey ,klen);

			nNode1= self; nNode2= Root[numRoot]; //for Output
			numRoot++;
			delete temp;
		}///////Root version Split handling
	}//////////////Root Split handling Over

	else{ //Intermediate Node Split
		if(Status==1){ //1이면 키, 2면 버전
			cout<<"Intermediate Key Split"<<endl;
			Node *temp = new Node();
			Node *right = new Node();
			right->level= level;right->isRoot=false;

			temp->copyNode(self);

			int tempR = 0,tempL=0;bool temp_flag;

			for (int i =0; i< temp->numEntry;i++){ //옮겨닮자
				temp_flag=false;
				for(int j =0;j<nR;j++){
					if( rightE[j] ==i )
						temp_flag = true;
				}
				if( temp_flag ){
					memcpy(&right->entries[tempR], &temp->entries[i], sizeof(Entry));
					tempR++;
				}
				else{
					memcpy(&self->entries[tempL], &temp->entries[i], sizeof(Entry));
					tempL++;
				}
			}

			right->numEntry=tempR; self->numEntry= tempL;
			memcpy(self->bp, lkey, klen); memcpy(right->bp, rkey, klen);
			delete temp;
			nNode1=self; nNode2= right; //for Output
		}
		else{
			cout<<"Intermediate Version Split"<<endl;
			Node *temp =new Node(); temp->copyNode(self);
			Node *right = new Node();
			right->level = level;right->isRoot=false;
			//cout<<"LEFT E = ";
			for ( int i = 0; i< nL;i++){ //옮겨담으면서 시간을 닫아주자!
				memcpy(&self->entries[i],&temp->entries[leftE[i]],sizeof(Entry));
				self->entries[i].bp[5]=rkey[4]; //시간을 닫아주는것 !!
				//cout<<self->entries[i].data<<" ";
			}
			//cout<<endl<<"RIGHT E= ";
			for ( int i = 0; i< nR;i++){
				memcpy(&right->entries[i],&temp->entries[rightE[i]],sizeof(Entry));
				right->entries[i].bp[4]=rkey[4]; //시작시간은 요렇게!
				//cout<<right->entries[i].data<<" ";
			}
			//cout<<endl;
			memcpy(self->bp, lkey, klen);
			memcpy(right->bp, rkey, klen);
			self->bp[5] = rkey[4];//확인차

			self->numEntry=nL;right->numEntry=nR;
			nNode1=self;nNode2=right; //right이 최근꺼!
			delete temp;
		}
	}
	return 1;
};////SplitNode Over!


bool Node::_SplitKey(double *oldKey, double *newKey, double tnow){

	int klen = dim*sizeof(double)*2;
	double key[6] = {DBL_MAX,DBL_MAX, 0,0, DBL_MAX, 0};
	hr_rect r_old(key,klen); //죽은거
	hr_rect r_new(key,klen); //새거
	for (int i = 0; i <numEntry;i++){
		if(entries[i].bp[4] == tnow ){
			r_new.expand(entries[i].bp);
		}
		if ( entries[i].bp[4] < tnow ){
			r_old.expand(entries[i].bp);
		}
	}
	memcpy(oldKey, r_old.coord,klen);
	memcpy(newKey, r_new.coord,klen);
	r_old.dealloc();
	r_new.dealloc();
	return true;

}
//VersionSplit하고나서만 불린다.
int Node::_deleteParent(int trace[], int depth, double tnow){
	cout<<"Delete Parent"<<endl;
	if( entries[trace[depth]].bp[4]==tnow){
		if (trace[depth]==numEntry){
			cout<<"case 1"<<endl;
			numEntry--;
		}
		else{
			cout<<"case 2"<<endl;
			memcpy(&entries[trace[depth]],&entries[numEntry],sizeof(Entry));
			numEntry--;
		}
	}
	if( entries[trace[depth]].bp[4] < tnow){
		cout<<"case 3"<<endl;
		entries[trace[depth]].bp[5]= tnow;
	}
	return 1;
};



int Node::_findMinPen(double *key){

	int index = 0;
	double minPenalty = DBL_MAX;
	double tempPenalty=0.0;
	double minPenArea = DBL_MAX;
	int klen = sizeof(double)*6;//dirty hack

	hr_rect r(key, klen);
	for (int i =0;i < numEntry;i++){
		//check
		if( entries[i].bp[5] > key[4] ){ //살아있는 애들 중에 ㅇㅇ
			hr_rect r1( entries[i].bp , klen);
			hr_rect r2( entries[i].bp , klen);
			r2.expand(&r);
			tempPenalty = r2.span() - r1.span();
			if( tempPenalty <= minPenalty ){
				if(tempPenalty==minPenalty){
					if(minPenArea > r1.span()){
						minPenArea= r1.span();
						index = i;
					}
				}
				else{
					index = i;
					minPenalty= tempPenalty;
				}
			}
			r1.dealloc();r2.dealloc();
		}
	}
	r.dealloc();
	return index;
};

int Node::_findMinPenOver(double *key){
	int index = 0;
	double minPenalty = DBL_MAX;
	double tempPenalty;
	double minPenArea=DBL_MAX;

	int klen = sizeof(double)*6;

	hr_rect k(key, klen);

	for (int i =0;i < numEntry;i++){
		//check
		tempPenalty = 0.0;
		if( entries[i].bp[5] > key[4] ){ //살아있으면!
			hr_rect r1( entries[i].bp , klen); //원본
			hr_rect r( entries[i].bp , klen); //원본+키
			r.expand(&k);
			for(int j = 0; j<numEntry;j++){
				hr_rect r2( entries[j].bp , klen);
				if(i != j){
					tempPenalty += r.intersect(&r2)-r1.intersect(&r2);
				}
				r2.dealloc();
			}
			if( tempPenalty <= minPenalty ){
				if(tempPenalty==minPenalty){
					if (r.span()-r1.span() <= minPenArea){
						minPenArea = r.span()-r1.span();
						index = i;
					}
				}
				else{
					index = i;
					minPenalty=tempPenalty;
				}
			}
			//delete r1; delete r;
			r1.dealloc();r.dealloc();
		}
	}
	//~k();
	k.dealloc();
	return index;
};


int Node::_numAlive(double tnow){
	int num = 0;
	for(int i =0;i<numEntry;i++){
		if(tnow < entries[i].bp[5])
			num++;
	}
	return num;
};

double* Node::_keyAlive(double tnow){
	int klen = dim*2*sizeof(double);
	double* key =new double[dim*2];
	hr_rect r1(entries[0].bp,klen);
	for(int i=1;i<numEntry;i++){
		if(tnow < entries[i].bp[5]) //살아있음
			r1.expand(entries[i].bp);
	}
	return key;
}


int Node::_Vsplit(int numKey, double* key1 ,double *key2, double* rkey,double* lkey,
		int rightE[], int& nR,  int leftE[] , int& nL){
	nR = 0;
	nL = 0;
	int klen = sizeof(double)*dim*2;
	for(int i = 0; i< numEntry;i++){
		if( entries[i].bp[5] > key1[4] ){ // 살아있는 애들은 오른쪽
			rightE[nR]= i;
			nR++;
		}
		if( entries[i].bp[4] != key1[4] ){ //늙은애들은 왼쪽
			leftE[nL]= i;
			nL++;
		}
	}
	hr_rect right(entries[rightE[0]].bp,klen );
	hr_rect left(entries[leftE[0]].bp,klen );
	for(int i =1;i<nR;i++){
		hr_rect nright(entries[rightE[i]].bp,klen);
		right.expand(&nright);
		nright.dealloc();
	}
	for(int i =1;i<nL;i++){
		hr_rect nleft(entries[leftE[i]].bp,klen);
		left.expand(&nleft);
		nleft.dealloc();
	}

	if(numKey >0){ //1개가 들어오면 오른쪽에
		hr_rect nright(key1,klen);
		right.expand(&nright);
		nright.dealloc();
	}
	if(numKey ==2){ //2개가 들어와도 또 오른쪽에
		hr_rect nright(key2,klen);
		right.expand(&nright);
		nright.dealloc();
	}

	memcpy(lkey, left.coord, klen);
	memcpy(rkey, right.coord, klen);
	rkey[4] = key1[4]; //시작 시간 변경해주고
	lkey[5] = key1[4]; //원래 있던 애들은 닫아주고
	left.dealloc();right.dealloc();

	//cout <<"Vsplit is over   nl= "<<nL <<"  nR = " << nR<<endl;
	return 1;
};



struct xyz{
	double bp[6];
	int e;
};
bool cmp1_xyz(xyz a, xyz b){return a.bp[0]< b.bp[0]; };
bool cmp2_xyz(xyz a, xyz b){return a.bp[1]< b.bp[1]; };
bool cmp3_xyz(xyz a, xyz b){return a.bp[2]< b.bp[2]; };
bool cmp4_xyz(xyz a, xyz b){return a.bp[3]< b.bp[3]; };
bool cmp5_xyz(xyz a, xyz b){return a.e < b.e; };

int Node::_Ksplit(int numKey, double* key1 ,double *key2,bool& oneGoesRight,bool& twoGoesRight,int rightE[], int& nR, double* rkey,double* lkey){//R* tree의 Heuristic 을 가져다 쓴다!
	//나눴을때 많은 애가 오른쪽
	double temp=0.0;
	double temp_area=0.0;
	double min_over=DBL_MAX;
	double min_area=DBL_MAX;
	int klen = sizeof(double)*dim*2;
	double minSplitRatio = 0.4;
	int minSize = minSplitRatio*numEnt_MAX; //smallest number of entries in a node;

	int index = 0;
	int axis = 0;

	int size = numEntry+numKey;

	xyz *ent = new xyz[size];
	xyz *best = new xyz[size];

	for(int i =0;i<numEntry;i++){
		memcpy( ent[i].bp, entries[i].bp, klen);
		ent[i].e = i;
	}
	if(numKey > 0){
		memcpy((ent[numEntry].bp) , key1, klen);
		ent[numEntry].e = numEntry;
	}
	if(numKey==2){
		memcpy((ent[numEntry+1].bp) , key2, klen);
		ent[numEntry+1].e = numEntry+1;
	}

	int margin_x = 0, margin_y = 0;
	int area_x=0, area_y=0;
	int temp_x=0,temp_y=0;
	for (int i = 0; i<4;i++){
		if(i==0)
			sort(ent , ent + size, cmp1_xyz); //i==0  = > x_min 값으로 정렬한것
		if(i==1)
			sort(ent , ent + size, cmp3_xyz); //i==1  = > x_max 값으로 정렬한것
		if(i==2)
			sort(ent , ent + size, cmp2_xyz); //i==2  = > y_min 값으로 정렬한것
		if(i==3)
			sort(ent , ent + size, cmp4_xyz); //i==3  = > y_max 값으로 정렬한것
		//n-1 가지의 dist 에 대하여 sum of margin을 구한다.
		for (int j = minSize; j<size-minSize-1; j++){
			hr_rect r1( ent[0].bp, klen);
			hr_rect r2( ent[ size-1 ].bp, klen);
			for (int k = 0; k < j;   k++){
				r1.expand( ent[k].bp);
			}
			for (int k = j+1; k < size ;k++){
				r2.expand( ent[k].bp);
			}
			temp = temp + r1.margin()  + r2.margin();
			temp_x += (r1.span()+ r2.span());
			r1.dealloc();r2.dealloc();
		}
		if(i==0||i==1){
			margin_x += temp;
			area_x += temp_x;
		}
		else{
			margin_y += temp;
			area_y += temp_y;
		}
	}

	// 어떤 축으로 정렬해야 가장 적은 Margin 인지 골랐다!
	// 그렇게 다시 한번 소팅을 하고!
	if(margin_x < margin_y ){ // BP = Xmin Ymin Xmax Ymax순서로 들어온다.
		sort(ent , ent + size, cmp1_xyz); //i==0,2  = > Min값으로 정렬한것
		axis = 0; //X축으로 정렬!
	}
	else if( margin_x > margin_y){
		sort(ent , ent + size, cmp2_xyz); //i==1,3  = > Min값으로 정렬한것
		axis = 1; //Y축으로 정렬!
	}
	else {
		if(area_x > area_y){
			sort(ent , ent + size, cmp2_xyz); //i==0,2  = > Min값으로 정렬한것
			axis = 1; //Y축으로 정렬!
		}
		else{
			sort(ent , ent + size, cmp1_xyz); //i==0,2  = > Min값으로 정렬한것
			axis = 0; //X축으로 정렬!
		}
	}

	//Sorting Ends
	//Under the sorting, find the best cut that minimize Overlap among n-1 possibilities
	////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////여기까진 괜찮은듯 ㅇㅇ/////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////
	// 2* (n-2*k)개 중 베스트 컷을 찾자!
	int axis_rec = 0;
	for(int t= 0;t<2;t++){

		hr_rect r1( ent[0].bp , klen );
		hr_rect r2( ent[size-1].bp , klen );

		for (int k = 1; k < minSize;k++){
			r1.expand(ent[k].bp); //left
			r2.expand( ent[size-k-1].bp );//right
		} //양 끝 minSize개를 미리 넣어놓고, 그 사이에 있는걸 쪼개보자 ㅇㅇ

		for (int i =minSize-1;i<size-minSize-1;i++){//자르는 곳의 위치 0~i까지 왼쪽, 나머진 오른쪽에 넣을 것.
			temp = 0.0;
			temp_area= 0.0;

			hr_rect ll(r1.coord, klen); //left
			hr_rect rr(r2.coord, klen); //right

			for(int k=minSize; k<= i; k++){
				ll.expand(ent[k].bp);//0~ (i-1)까지 넣어보자 ㅇㅇ
			}
			for(int k=i+1; k<= size-minSize-1; k++){
				rr.expand(ent[k].bp);
			}

			temp = rr.overlap(&ll);
			if( temp  <= min_over ){
				//if overlap = 0, then try to find a smallest area sum cut
				if(temp== min_over){
					temp_area =  rr.span()+ ll.span();
					if(temp_area < min_area){
						min_area = temp_area;
						index = i;
						axis_rec = axis;
						memcpy(best, ent, sizeof(xyz)*(size));
						memcpy(lkey , ll.coord, klen);
						memcpy(rkey , rr.coord, klen);
					}
				}
				else{
					min_over = temp;
					index = i;
					axis_rec = axis;
					memcpy(best, ent, sizeof(xyz)*(size));
					memcpy(lkey , ll.coord, klen);
					memcpy(rkey , rr.coord, klen);
				}
			}
			rr.dealloc();ll.dealloc();
		}// 몇번째 index인지 찾고

		r1.dealloc();r2.dealloc();


		if(axis==0){
			axis +=2;
			sort(ent , ent + size, cmp3_xyz); //Max값으로 정렬
		}
		else{
			axis+=2;
			sort(ent , ent + size, cmp4_xyz); //Max값으로 정렬
		}
	}

	// index를 이용해 rightE를 채워넣는다.
	nR=0;
	int temp_nR = 0;
	for (int i = index+1; i < size ;i++){ // r1을 여기에 담는 것임!
		if(best[i].e < numEntry){
			rightE[nR] = best[i].e;
			nR++;
		}
		else if( best[i].e==numEntry ){
			oneGoesRight = true;
			temp_nR++;
		}
		else if( best[i].e==numEntry+1 ){
			twoGoesRight = true;
			temp_nR++;
		}
	}
	cout<<"Key Split result = size: "<<size<<", nR after KSplit  = "<<nR<<" temp_nR= "<<temp_nR<<endl;

	delete [] ent;
	//delete [] best;
	// 그 축으로 정렬한 뒤 나오는 n-3가지 split중 가장 적은 overlap이 발생하는 split을 찾는다.
	return 0;
};

void Node::copyNode(Node *node){
	int klen = sizeof(double)*dim*2;
	memcpy(bp, node->bp,klen);
	for(int i =0;i<node->numEntry;i++){
		memcpy(&entries[i],&node->entries[i],sizeof(Entry));
	}
	numEntry = node->numEntry;
	isRoot= node->isRoot;
	level = node->level;
}



Entry::Entry() {
	// TODO Auto-generated constructor stub
	 bp = {};
	 pos = true;
	 dlen = 0 ;
	 data= 0;
	 child = 0;
};

Entry::~Entry() {
	// TODO Auto-generated destructor stub
};
