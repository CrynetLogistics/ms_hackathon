#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "stdio.h"
#include "math.h"
#include "structures.h"
#include <cmath>

using namespace std;

#undef main
vector<Pair*> parse(Graphs graph, Genders gen) {
	fstream file;
	switch (graph) {
		case HEIGHT :
			file.open("Height.csv", ios::in);
			break;
		case WEIGHT :
			file.open("Weight.csv", ios::in);
			break;
		case CALORIES :
			file.open("Calories.csv", ios::in);
			break;
		case CALORIES_USER :
			file.open("CaloriesUser.csv", ios::in);
			break;
		default :
			cout << "Unknown graph" << endl;
			return vector<Pair*> ();
	}

	string buffer;
	string * in;
	vector<Pair*> data;
	int cols = 3;

	if (file.bad() || !file.is_open()) {
		cout << "Error opening file" << endl;
		return vector<Pair*> ();
	}

	file >> buffer; // (A/G)
	file >> buffer; // ,
	file >> buffer; // male
	file >> buffer; // ,
	file >> buffer; // female

	while (file >> buffer) {
		string label = buffer;
		int val = -1;
		switch (gen) {
			case FEMALE :
				file >> buffer; // .
				file >> buffer; // male_val
				file >> buffer; // ,
				file >> val;    // val
				break;
			case MALE :
				file >> buffer; // ,
				file >> val;    // val
				file >> buffer; // ,
				file >> buffer; // female_val
				break;
		}
		Pair * temp =  new Pair;
		temp->x = label;
		temp->y = val;
		data.push_back(temp);
	}
	file.close();
	return data;

}



int main(){
	vector<Pair*> data = parse(CALORIES, MALE);
	float numeratorSum = 0;
	float denominatorSum = 0;
	int idx = 7;
	for(Pair* p : data){
		numeratorSum += p->y * log(idx);
		denominatorSum += log(idx)*log(idx);
		idx++;
	}

	float a = numeratorSum/denominatorSum;

	idx = 7;
	numeratorSum = 0;
	denominatorSum = 0;
	for(Pair* p : data){
		numeratorSum += exp(2*p->y/a);
		denominatorSum += idx*exp(p->y/a);
		idx++;
	}

	float b = numeratorSum/denominatorSum;

	int i=7;
	for(Pair* p : data){
		cout<<p->x<<"  "<<p->y<<"  "<<1.8*a*log(0.3*b*i)-1.5*a+1400<<endl;
		i++;
	}

	int dummy;
	cin>>dummy;
	return 0;
}