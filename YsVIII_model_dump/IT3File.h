#pragma once
#include "utilities.h"
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include "DataBlock.h"



class item {
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
	TEXI * texi = NULL;
	TEX2 * tex2 = NULL;
	ITP  * itp = NULL;
	VPAX * vpax = NULL;

	item() = default;
	item(unsigned int addr) :addr(addr) {}

	void output_data();

	std::string to_string();

	
};


class IT3File
{
public:
	IT3File(const std::vector<uint8_t> &file_content);
	~IT3File();
	std::vector<item> items;
	std::string to_string();
	void output_data();
};

