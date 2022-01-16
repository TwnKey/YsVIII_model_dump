#pragma once
#include "utilities.h"
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include "DataBlock.h"
#include <map>
#include "MTBFile.h"

class chunk {
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
	KAN7* kan7 = NULL;
	std::vector<TEXI> texi;
	std::vector<TEX2> tex2;
	std::vector<ITP> itp;
	VPAX * vpax = NULL;

	chunk() = default;
	chunk(unsigned int addr) :addr(addr) {}

	void output_data(std::string scene_folder);
	
	std::string to_string();

	
};


class IT3File
{
public:
	IT3File(const std::vector<uint8_t> &file_content);
	~IT3File();
	IT3File() = default;
	std::map<std::string, chunk> chunks;
	std::string to_string();
	void output_data(std::string scene_folder);
	void add_kan7_from_m_file(IT3File m_file);
};

