#pragma once

typedef struct vertex{
	float x;
	float y;
	float z;
} vertex_t;

typedef struct colour{
	float r;
	float g;
	float b;
} colour_t;

enum Graphs {
	HEIGHT,
	WEIGHT,
	CALORIES
};

enum Genders {
	MALE,
	FEMALE
};

struct Pair {
	string x;
	int y;
};