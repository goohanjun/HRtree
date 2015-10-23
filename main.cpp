#include <iostream>
#include "Command.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::string;
using std::atoi;
using namespace std;



int main(){
	int numRoot = 0;
	Node* Root[MAX_ROOT];


	cout << "HR+Tree Start" <<endl;

	cout << "Maximum Entry number = "<< numEnt_MAX<<endl;
	Root[0] = new Node(); // Initialize the first root node
	Root[0]->isRoot=true;

	double key[6];  /// X_min, Y_min, X_max, Y_max, t_start, t_end
	int data = 0;

	/*
	for (double i =0.0; i<;i++){
		double key[6] = {i,i+1, i+2,i+3,0.0,DBL_MAX};  /// X_min, Y_min, X_max, Y_max, t_start, t_end
		int data = i;

		if( CommandInsert(Root[numRoot], key, sizeof(double)*6 , data , sizeof(int), Root, numRoot ) ){
			//cout<< i << "th Insertion Over"<<endl
		}
		else{ //Root Split is Occured
			if(numRoot < MAX_ROOT){
				numRoot++;
				Root[numRoot] = new Node();
				Root[numRoot]->isRoot=true;
				CommandInsert(Root[numRoot], key, sizeof(double)*6 , data , sizeof(int), Root, numRoot );
			}
		}
	}
	*/
	char input_string[100];
	//HrRootTable* root_table_pointer = new HrRootTable;
	ifstream input_file("input_1.txt");

	int string_index = 0;
	int token_array[10];

	string temp_string;
	string command_type;
	string delimeter = " ";
	size_t position = 0;

	//HrEntry* entry_for_command = new HrEntry();
	while (!input_file.eof()) {
		input_file.getline(input_string, 100);
		temp_string = input_string;
		position = temp_string.find(delimeter);
		command_type = temp_string.substr(0, position);
		temp_string.erase(0, position + delimeter.length());
		string_index = 0;
		while ((position = temp_string.find(delimeter)) != string::npos) {
			token_array[string_index] = atoi(
					temp_string.substr(0, position).c_str());
			temp_string.erase(0, position + delimeter.length());
			++string_index;
		}
		token_array[string_index] = atoi(temp_string.c_str());
		if (command_type == "Time")
			key[4] = token_array[0];

		if (command_type == "Insert") {
			key[0] = token_array[0];
			key[1] = token_array[2];
			key[2] = token_array[1];
			key[3] = token_array[3];
			key[5] = DBL_MAX;
			data = token_array[4];

			if (CommandInsert(Root[numRoot], key, sizeof(double) * 6, data,sizeof(int), Root, numRoot)) {
			} else { //Root Split is Occured
				if (numRoot < MAX_ROOT) {
					numRoot++;
					Root[numRoot] = new Node();
					Root[numRoot]->isRoot = true;
					CommandInsert(Root[numRoot], key, sizeof(double) * 6, data,
							sizeof(int), Root, numRoot);
				}
			}
		}
		if (command_type == "Delete") {
			/*
			key[0] = token_array[0];
			key[1] = token_array[2];
			key[2] = token_array[1];
			key[3] = token_array[3];
			key[5] = DBL_MAX;
			*/
		}
		if (command_type == "Stop")
			break;
	}



	//cout << "2. Search " <<endl;
	// (Insert, Delete) is in current Node

	// Find time Index first to search
	//print the data in a Tree
	for(int i = 0; i< numRoot+1 ;i++){
		cout<<"\n///////////////////////////////////////////"<<endl;
		cout <<"\n\t"<< i << " th Root is printed\n"<<endl;
		CommandDump( Root[i] );
	}
	cout<<"\n//////////////////    FIN.    ///////////////"<<endl;

	return 0;
}
