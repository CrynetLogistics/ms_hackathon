#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "string.h"
#include "stdio.h"
#include "math.h"
#include "structures.h"
#include "stdlib.h"
#include <cmath>

using namespace std;

#undef main

int readRef(Graphs, Genders, int);

int main(int argc, char** argv) {

	Genders gender;
	switch (argv[2][0]) {
		case 'M' :
			gender = MALE;
			break;
		case 'F' :
			gender = FEMALE;
			break;
	}
	Graphs graphs;

	if (!strcmp("calories", argv[1])) graphs = CALORIES;
	else if (!strcmp("weight", argv[1])) graphs = WEIGHT;
	else if (!strcmp("height", argv[1])) graphs = HEIGHT;

	int age = atoi(argv[3]);

	cout << readRef(graphs, gender, age);

	return 0;
}

void convert(int in[], int size) {
	ofstream file;
	file.open("temp.txt");
	int labels[size / 2];
	int values[size / 2];
	for (int i = 0; i < size; i++) {
		if (i % 2 == 0) {
			labels[i/2] = in[i];
		} else {
			values[i/2] = in[i];
		}
	}
	for (int i = 0; i < size /  2; i++) {
		file << labels[i];
		file << ' ';
	}
	file << '\n';
	for (int i = 0; i < size / 2; i++) {
		file << values[i];
		file << ' ';
	}
	file.close();
}

int readRef(Graphs graphs, Genders genders, int age) {
	fstream file;
	switch (graphs) {
		case CALORIES :
			file.open("Calories.csv", ios::in);
			break;
		case WEIGHT :
			file.open("Weight.csv", ios::in);
			break;
		case HEIGHT :
			file.open("Height.csv", ios::in);
			break;
	}
	string buffer;
	string *in;

	if (file.bad() || !file.is_open()) {
		cout << "Error opening file" << endl;
		return -1;
	}

	file >> buffer; // (A/G)
	file >> buffer; // ,
	file >> buffer; // male
	file >> buffer; // ,
	file >> buffer; // female

	while (file >> buffer) {
		int label = atoi(buffer.c_str());
		int val = -1;
		switch (genders) {
			case FEMALE :
				file >> buffer; // .
				file >> buffer; // male_val
				file >> buffer; // ,
				file >> val;    // val
				if (label == age) return val;
				break;
			case MALE :
				file >> buffer; // ,
				file >> val;    // val
				if (label == age) return val;
				file >> buffer; // ,
				file >> buffer; // female_val
				break;
		}

	}
}

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
		//case CALORIES_USER :
		//	file.open("CaloriesUser.csv", ios::in);
		//	break;
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



int dA(Graphs graphs, Genders genders){
	vector<Pair*> data = parse(graphs, genders);
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
	int pass[data.size() * 2];
	for(int i = 0; i < data.size() * 2; i += 2){
		pass[i] = atoi(data[i/2]->x.c_str());
		pass[i+1] = data[i/2]->y;
		//cout<<p->x<<"  "<<p->y<<"  "<<1.8*a*log(0.3*b*i)-1.5*a+1400<<endl;
		//i++;
	}

	convert(pass, data.size()*2);
	return 0;
}