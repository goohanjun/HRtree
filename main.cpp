/*
 * =============================================================================
 * HR+ Tree
 *
 * This is a implementation of Efficient Historical R-trees [SSDBM'01]
 *
 * HR-tree keeps an R-tree for each time stamp in history, but allow
 * consecutive trees to share branches when the underlying objects do not change
 *
 *
 * =============================================================================
 */

#include "Command.h"
#include "RootTable.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <set>

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::string;
using std::atoi;
using std::set;
using std::pair;

int main(){
	cout << "Starting HR+Tree.." << endl;

	RootTable *RT = new RootTable();
	RT->Root[0]->bp[5]=DBL_MAX; // First root is alive

	double key[6];  /// X_min, Y_min, X_max, Y_max, t_start, t_end
	int data = 0;

	//For parsing the data
	char input_string[100]; //Maximum length of a line is 100 characters
	int string_index = 0;
	int token_array[10];
	ifstream input_file("input_3D");
	string temp_string;
	string command_type;
	string delimeter = " ";
	size_t position = 0;

	// ===============Verification================
	set < int > *answers[100];
	set<int>::iterator it;
	int ElapsedTime=0;
	int numAns = 0;

	int numObject=0;
	bool isCorrect = true;
	// ===============Verification================

	//Parse each line.
	while (!input_file.eof()) {
		//Parse the data into <Key , Data>
		input_file.getline(input_string, 100);
		temp_string = input_string;
		position = temp_string.find(delimeter);
		command_type = temp_string.substr(0, position);
		temp_string.erase(0, position + delimeter.length());
		string_index = 0;

		while ((position = temp_string.find(delimeter)) != string::npos) {
			token_array[string_index] = atoi(temp_string.substr(0, position).c_str());
			temp_string.erase(0, position + delimeter.length());
			++string_index;
		}
		token_array[string_index] = atoi(temp_string.c_str());

		if (command_type == "Time"){
			key[4] = token_array[0];

			//For Validation
			ElapsedTime = token_array[0];
			answers[numAns] = new set<int>;
			numAns++;
			if (numAns != 1) { //Insert previous set to new set
				for (it = answers[numAns - 2]->begin();	it != answers[numAns - 2]->end(); ++it)
					answers[numAns - 1]->insert(*it);
			}
		}
		data = token_array[4];
		key[0] = token_array[0];
		key[1] = token_array[2];
		key[2] = token_array[1];
		key[3] = token_array[3];
		/*
		 * Insertion
		 * Key = [x_min y_min x_max y_max t_start t_end=*] (double array)
		 * Data = Object_ID (integer)
		 * RT->numRoot = 0 in the beginning
		 */
		if (command_type == "Insert") {
			key[5] = DBL_MAX;
			//Key has a form [x_min y_min x_max y_max t_start t_end]
			//cout<<"Data : "<<data <<" is inserting..."<<endl;
			if (!CommandInsert(RT->Root[RT->numRoot-1], key, data, sizeof(int), RT)) {
				if (RT->numRoot < MAX_ROOT) { //previous Root is deleted. Insert a new alive root
					cout<<"Main:: Insert a new alive root"<<endl;
					RT->Root[RT->numRoot] = new HNode();
					RT->Root[RT->numRoot]->isRoot = true;
					RT->Root[RT->numRoot]->bp[4] = key[4];
					RT->numRoot++;
					CommandInsert(RT->Root[RT->numRoot-1], key, data, sizeof(int), RT);
					//Insert the object into new Root
				}
			}

			if (isValidate) {
				//For validation
				answers[numAns-1]->insert(data);
				if (isVerifyAnswer) {
					for (int i = 0; i < numAns; i++) {
						cout << i << "th Answer set" << endl;
						for (it = answers[i]->begin(); it != answers[i]->end();
								++it)
							cout << *it << " ";
						cout << endl;
					}
					CommandView(RT);

				}
				isCorrect = CommandVerify(RT,answers,ElapsedTime);
			}
			//Debug
			//if(numObject%100 == 0)
				cout<<"Data : "<<data<<" is inserted"<<endl;
			numObject++;
		}

		/*
		 * Deletion
		 * Key = [x_min y_min x_max y_max t_start t_end = t_start]
		 * Data = Object_ID
		 */
		if (command_type == "Delete") {
			key[5] = key[4];
			CommandDelete(RT->Root[RT->numRoot-1], key, data, sizeof(int), RT);

			//For validation
			answers[numAns-1]->erase(data);
			isCorrect = CommandVerify(RT,answers,ElapsedTime);
			cout<<"Data : "<<data<<" is deleted"<<endl;
		}
		CommandView(RT);
		// Stop
		if (command_type == "Stop")
			break;
	}

	if( isCorrect )
		cout<<"Finished well."<<endl;
	else
		cout<<"Finished with Error"<<endl;
	return 0;
}
