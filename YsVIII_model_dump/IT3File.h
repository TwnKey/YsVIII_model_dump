#pragma once
#include "utilities.h"
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>


class data {
public:
	data() {};
	virtual ~data() = default;
	virtual void output_data() = 0;
	uint32_t id;
};


class Chunk {


	public:
	uint32_t FourCC;
	size_t size;
	std::shared_ptr<data> data;
	unsigned int addr;

	Chunk(const std::vector<uint8_t> &file_content, unsigned int &addr);
};


class IT3File
{
public:
	IT3File(const std::vector<uint8_t> &file_content);
	~IT3File();
	std::vector<Chunk> chunks;
	std::string to_string();
	void output_data();
};

