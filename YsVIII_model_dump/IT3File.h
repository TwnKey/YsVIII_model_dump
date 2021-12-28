#pragma once
#include "utilities.h"
#include <vector>
#include <memory>
#include <iostream>
/* They will all have their own way of outputting data */

enum identifier {
	INFO_ID = 0x4F464E49,
	RTY2_ID = 0x32595452,
	LIG3_ID = 0x3347494C,
	INFZ_ID = 0x5A464E49
};


class data {
	public:
	data() {};
	virtual ~data() = default;
	virtual void output_data() = 0;
	uint32_t id;
};
class INFO : public data {
	public:
	INFO() = default;
	std::string text_id1, text_id2, text_id3;
	std::vector<float> floats;
	
	INFO(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
		unsigned int start_addr = addr;

		text_id1 = read_string(file_content, addr);
		addr = start_addr + 0x40; 

		while (addr < start_addr + size) {
			floats.push_back(read_data<float>(file_content, addr));
		}
	}

	void output_data() {
		std::cout << text_id1 << std::endl;
		for (auto f : floats)
			std::cout << f << std::endl;
	}
};
class RTY2 : public data {
	public:
	RTY2() = default;

	float float0;
	uint8_t byte;
	vector3<float> v0;

	RTY2(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
		float0 = read_data<float>(file_content, addr);
		byte = read_data<uint8_t>(file_content, addr);
		v0 = read_data<vector3<float>>(file_content, addr);
	}


	void output_data() {
		std::cout << float0 << ", " << byte << ", " << v0.x << ", " << v0.y << ", " << v0.z << std::endl;
	}
}; 
class LIG3 : public data {
public:
	LIG3() = default;

	vector4<float> v0;
	uint8_t byte;
	float float0;
	vector4<float> v1;

	LIG3(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
		v0 = read_data<vector4<float>>(file_content, addr);
		byte = read_data<uint8_t>(file_content, addr);
		float0 = read_data<float>(file_content, addr);
		v1 = read_data<vector4<float>>(file_content, addr);
	}


	void output_data() {
		std::cout << v0.x << ", " << v0.y << ", " << v0.z << ", " << v0.t << std::endl;
		std::cout << byte << std::endl;
		std::cout << float0 << std::endl;
		std::cout << v1.x << ", " << v1.y << ", " << v1.z << ", " << v1.t << std::endl;

	}

};
class INFZ : public data {
public:
	INFZ() = default;

	vector4<int> v0;

	INFZ(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
		v0 = read_data<vector4<int>>(file_content, addr);
	}


	void output_data() {
		std::cout << v0.x << ", " << v0.y << ", " << v0.z << ", " << v0.t << std::endl;
	}
};
/*
class BBOX : public data {}; //Bounding Box ?
class CHID : public data {};
class JNTV : public data {}; //Joints ?
class TEXI : public data {}; //Textures ?
class VPAX : public data {};
class MAT6 : public data {};
class BON3 : public data {}; */



class Chunk {

	/*IT3 files contain several chunks, all of them have a 4 bytes identifier
	which allows the application to skip them if they're not compatible.
	Ideally we list all the possible identifier and make one class for each*/

	public:
	uint32_t FourCC;
	size_t size;
	std::shared_ptr<data> data;

	Chunk(const std::vector<uint8_t> &file_content, unsigned int &addr);
};


class IT3File
{
public:
	IT3File(const std::vector<uint8_t> &file_content);
	~IT3File();
	std::vector<Chunk> chunks;
	std::string to_string();
};

