#pragma once
#include <vector>
#include "utilities.h"
#include "IT3File.h"
enum identifier {
	INFO_ID = 0x4F464E49,
	RTY2_ID = 0x32595452,
	LIG3_ID = 0x3347494C,
	INFZ_ID = 0x5A464E49,
	BBOX_ID = 0x584F4242,
	CHID_ID = 0x44494843,
	JNTV_ID = 0x56544e4a,
	MAT6_ID = 0x3654414d,
	BON3_ID = 0x334e4f42,
	TEXI_ID = 0x49584554,
	ITP_ID = 0xFF505449,
	IHDR_ID = 0x52444849,
	IALP_ID = 0x504c4149,
	IMIP_ID = 0x50494d49,
	IDAT_ID = 0x54414449,
	IEND_ID = 0x444E4549
};


struct BlockDesc {
	unsigned int flags;
	vector4<int> v0; 

};

class DataBlock { //6b7057
public:
	std::vector<uint8_t> content;
	BlockDesc caracs;


	DataBlock() = default;

	unsigned int reading_method; //1 is standard, 2 is hybrid


	DataBlock(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t sz, unsigned int reading_method, BlockDesc caracs);

	void output_data(std::string filepath);


};


class INFO : public data {
public:
	INFO() = default;
	std::string text_id1, text_id2, text_id3;
	std::vector<float> floats;

	INFO(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);

	void output_data();
};
class RTY2 : public data {
public:
	RTY2() = default;

	float float0;
	uint8_t byte;
	vector3<float> v0;

	RTY2(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data();
};
class LIG3 : public data {
public:
	LIG3() = default;

	vector4<float> v0;
	uint8_t byte;
	float float0;
	vector4<float> v1;

	LIG3(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data();

};
class INFZ : public data {
public:
	INFZ() = default;

	vector4<int> v0;

	INFZ(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data();
};

class BBOX : public data {
public:
	BBOX() = default;

	vector3<float> a;
	vector3<float> b;
	vector3<float> c;
	vector3<float> d;

	BBOX(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data();

};
class CHID : public data {
public:
	CHID() = default;

	std::string intro;
	std::vector<std::string> strs;

	CHID(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);
	void output_data();

};

class JNTV : public data {
public:
	JNTV() = default;

	vector4<float> v0;
	int id;

	JNTV(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data();

};




class MAT6 : public data {
public:
	MAT6() = default;

	int count;
	std::vector <int> int0s;
	std::vector <DataBlock> matm;
	//0x6d1165
	MAT6(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data();

};

class BON3 : public data {
public:
	BON3() = default;

	int int0, int1;
	std::string name;

	std::vector <DataBlock> matm;

	BON3(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data();

};

struct IHDR {
	unsigned int int0, dim1, dim2, compressed_size;
	uint16_t s1, bpp, s3, s4, type, s6;
	int not_used;
};
struct IALP {
	int i1, i2;
};

struct IMIP {
	vector3<int> v0;
};



class ITP : public data {
public:
	std::string name;
	ITP() = default;
	IHDR hdr;
	IALP alp;
	IMIP mip;
	std::vector<uint8_t> content;

	ITP(const std::vector<uint8_t> &file_content, unsigned int &addr, std::string name);
	void output_data();
	void unswizzle(size_t width, size_t height, size_t blockSize);
	void add_header();
};


class TEXI : public data {
public:
	TEXI() = default;

	std::string name;
	ITP itp;

	TEXI(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data();

};
