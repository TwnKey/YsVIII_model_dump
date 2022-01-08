#pragma once
#include <vector>
#include "utilities.h"
#include <unordered_map>
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
	IEND_ID = 0x444E4549,
	VPAX_ID = 0x58415056,
	VPAC_ID = 0x43415056,
	TEX2_ID = 0x32584554,
	IHAS_ID = 0x53414849,
	VP11_ID = 0x31315056,
	KAN7_ID = 0x374E414B,
};

class data {
public:
	data() {};
	virtual ~data() = default;
	virtual void output_data(std::string node_name) = 0;
	uint32_t id;
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

	void output_data(std::string node_name);


};


class INFO : public data {
public:
	INFO() = default;
	std::string text_id1, text_id2, text_id3;
	vector3<float> v0;
	matrix4 transform;
	

	INFO(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);

	void output_data(std::string node_name);
};
class RTY2 : public data {
public:
	RTY2() = default;

	unsigned int material_variant;
	uint8_t byte;
	vector3<float> v0;

	RTY2(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data(std::string node_name);
};
class LIG3 : public data {
public:
	LIG3() = default;

	vector4<float> v0;
	uint8_t byte;
	float float0;
	vector4<float> v1;

	LIG3(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data(std::string node_name);

};
class INFZ : public data {
public:
	INFZ() = default;

	vector4<int> v0;

	INFZ(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data(std::string node_name);
};

class BBOX : public data {
public:
	BBOX() = default;

	vector3<float> a;
	vector3<float> b;
	vector3<float> c;
	vector3<float> d;

	BBOX(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data(std::string node_name);

};
class CHID : public data {
public:
	CHID() = default;

	std::string parent;
	std::vector<std::string> children;

	CHID(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);
	void output_data(std::string node_name);

};

class JNTV : public data {
public:
	JNTV() = default;

	vector4<float> v0;
	int id;

	JNTV(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data(std::string node_name);

};

struct texture {
	std::string name;

	unsigned int texture_type, uint0, uint2, uint3, uint4, uint5, uint6;
	texture(const std::vector<uint8_t>& file_content, unsigned int& addr){
		uint0 = read_data<unsigned int>(file_content, addr);
		texture_type = read_data<unsigned int>(file_content, addr);
		uint2 = read_data<unsigned int>(file_content, addr);
		uint3 = read_data<unsigned int>(file_content, addr);
		uint4 = read_data<unsigned int>(file_content, addr);
		uint5 = read_data<unsigned int>(file_content, addr);
		uint6 = read_data<unsigned int>(file_content, addr);
	}
};
struct material_data {
	std::string name;
	std::vector<texture> textures;
	std::vector<vector4<float>> parameters;
};

class MAT6 : public data {
public:
	MAT6() = default;

	int count;
	std::vector <int> int0s;
	std::vector <DataBlock> matm;
	unsigned int MATM_flags, MATE_flags;
	std::vector<material_data> mats;
	//0x6d1165
	MAT6(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data(std::string node_name);

};

struct bone_data {
	std::string name;
	matrix4 offset_matrix;
	bone_data() = default;
	bone_data(std::string name, matrix4 mat) : name(name), offset_matrix(mat) {}
};

class BON3 : public data {
public:
	BON3() = default;

	int int0, int1;
	std::string mesh_name;
	std::vector<std::string> joints_names; 
	std::unordered_map<std::string, bone_data> bones;
	std::vector<DataBlock> matms;
	BON3(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data(std::string node_name);

};
struct key_animation {
	vector3<float> position;
	vector4<float> rotation;
	vector4<float> rotation2;
	vector3<float> scale;
	
	unsigned int tick;
	unsigned int something;
	key_animation(const std::vector<uint8_t>& file_content, unsigned int& addr) {
		position = read_data < vector3<float>>(file_content, addr);
		addr += 4;
		rotation = read_data<vector4<float>>(file_content, addr);
		rotation2 = read_data<vector4<float>>(file_content, addr);
		scale = read_data < vector3<float>>(file_content, addr);
		addr += 4;
		tick = read_data<unsigned int>(file_content, addr);
		something = read_data<unsigned int>(file_content, addr);
	}
};
class KAN7 : public data {
public:
	KAN7() = default;

	int int0;
	unsigned int things[0x28 / 4];
	std::vector<std::vector<DataBlock>> matms;
	std::vector<std::vector<key_animation>> kans;
	KAN7(const std::vector<uint8_t>& file_content, unsigned int& addr, size_t size);


	void output_data(std::string node_name);

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
struct IHAS {
	vector2<int> v0;
	vector2<float> v1;
};


class ITP : public data {
public:
	std::string name;
	ITP() = default;
	IHDR hdr;
	IALP alp;
	IMIP mip;
	IHAS ihas;
	std::vector<uint8_t> content;

	ITP(const std::vector<uint8_t> &file_content, unsigned int &addr, std::string name);
	void output_data(std::string node_name);
	void unswizzle(size_t width, size_t height, size_t blockSize);
	void add_header();
};


class TEXI : public data {
public:
	TEXI() = default;

	std::string name;
	ITP itp;

	TEXI(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data(std::string node_name);

};

class TEX2 : public data {
public:
	TEX2() = default;

	std::string name;
	ITP itp;

	TEX2(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size);


	void output_data(std::string node_name);

};

struct header_VPAC {
	unsigned int FourCC;
	unsigned int version;
	vector4<float> v0, v1, v2;
	unsigned int uint0[0x4D];
};

struct vertex {
	//I'm not sure all models have vertexes of size 0xA0 since it is not hardcoded, so for now I'll stay with 0xA0 and change when it throws an exception
	vector4<float> position;
	vector4<float> no_idea;
	vector4<float> no_idea1;
	vector4<float> no_idea2;
	vector2<float> noidea3;
	unsigned int no_idea4;
	unsigned int no_idea5;
	vector2<float> uv;
	vector2<float> uv2;
	vector4<float> no_idea7;
	vector4<float> no_idea8;
	vector4<float> no_idea9;
	uint8_t weights[8];
	uint8_t bones_indexes[8];

};
struct mesh_data {
	header_VPAC header;
	std::vector<uint8_t> first_part;
	std::vector<vertex> vertices;
	std::vector<unsigned int>indexes;
	size_t block_size;
	size_t nb_blocks;
	unsigned int material_id;
};
class VPAX : public data {
public:
	VPAX() = default;

	std::string name;
	std::vector<std::vector<uint8_t>> content_vertices, content_indexes;
	
	
	std::vector<mesh_data> meshes_d;

	VPAX(const std::vector<uint8_t> &file_content, unsigned int &addr, std::string name, int game_version);


	void output_data(std::string node_name);

};