#pragma once
#include "utilities.h"
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include "DataBlock.h"



class node {
public:
	unsigned int addr = 0;

	INFO * info = NULL;
	RTY2 * rty2 = NULL;
	LIG3 * lig3 = NULL;
	INFZ * infz = NULL;
	BBOX * bbox = NULL;
	CHID * chid = NULL;
	JNTV * jntv = NULL;
	MAT6 * mat6 = NULL;
	BON3 * bon3 = NULL;
	std::vector<TEXI> texi;
	std::vector<TEX2> tex2;
	std::vector<ITP> itp;
	VPAX * vpax = NULL;

	node() = default;
	node(unsigned int addr) :addr(addr) {}

	void output_data();

	std::string to_string();

	
};


class IT3File
{
public:
	IT3File(const std::vector<uint8_t> &file_content);
	~IT3File();
	std::vector<node> nodes;
	std::string to_string();
	void output_data();
};

