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

class BBOX : public data {
public:
	BBOX() = default;

	vector3<float> a;
	vector3<float> b;
	vector3<float> c;
	vector3<float> d;

	BBOX(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
		a = read_data<vector3<float>>(file_content, addr);
		b = read_data<vector3<float>>(file_content, addr);
		c = read_data<vector3<float>>(file_content, addr);
		d = read_data<vector3<float>>(file_content, addr);
	}


	void output_data() {
		std::cout << a.x << ", " << a.y << ", " << a.z << std::endl;
		std::cout << b.x << ", " << b.y << ", " << b.z << std::endl;
		std::cout << c.x << ", " << c.y << ", " << c.z << std::endl;
		std::cout << d.x << ", " << d.y << ", " << d.z << std::endl;
	}

}; 
class CHID : public data {
public:
	CHID() = default;

	std::string intro;
	std::vector<std::string> strs;

	CHID(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
		unsigned int start_addr = addr;

		intro = read_string(file_content, addr);
		addr = start_addr + 0x40;
		size_t nb_strings = read_data<unsigned int>(file_content, addr);

		for (unsigned int idx = 0; idx < nb_strings; idx++) {
			start_addr = addr;
			strs.push_back(read_string(file_content, addr));
			addr = start_addr + 0x40;
		}
	}


	void output_data() {
		std::cout << intro <<  std::endl;
		for (unsigned int idx_str = 0; idx_str < strs.size(); idx_str++)
			std::cout << strs[idx_str] << std::endl;
	}

};

class JNTV : public data {
public:
	JNTV() = default;

	vector4<float> v0;
	int id;

	JNTV(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
		
		v0 = read_data<vector4<float>>(file_content, addr);
		id = read_data<int>(file_content, addr);
		
	}


	void output_data() {
		std::cout << id << std::endl;
		std::cout << v0.x << ", " << v0.y << ", " << v0.z << ", " << v0.t << std::endl;
	}

};

struct chunk_carac{
	unsigned int flags;
	vector4<int> v0; //first integer is number of chunks 
	
};

struct chunk { //6b7057
	std::vector <std::vector<uint8_t>> type1;
	std::vector <std::vector<uint8_t>> type2;

	chunk_carac caracs;


	chunk() = default;

	unsigned int reading_method; //1 is standard, 2 is hybrid


	chunk(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t sz, unsigned int reading_method, chunk_carac caracs) {
		this->reading_method = reading_method;
		this->caracs = caracs;

		if (reading_method == 2){

			unsigned int start = addr;
			//MATM 6dd4f0
			while (addr < (start + sz)){
				uint8_t current_byte1 = read_data<uint8_t>(file_content, addr);
				uint8_t current_byte2 = read_data<uint8_t>(file_content, addr);
				if (current_byte1 == 0) {
					std::vector<uint8_t> el = std::vector<uint8_t>(current_byte2);
					std::copy(file_content.begin()+ addr, file_content.begin() + addr + current_byte2, el.begin());
					addr = addr + current_byte2;
					type1.push_back(el);
				}
				else {
					std::vector<uint8_t> el = std::vector<uint8_t>(3);
					std::copy(file_content.begin() + addr-2, file_content.begin() + addr, el.begin());
					addr = addr + 1;
					type2.push_back(el);
				}
			}
		}
		else {
			std::vector<uint8_t> el = std::vector<uint8_t>(sz);
			std::copy(file_content.begin() + addr, file_content.begin() + addr + sz, el.begin());
			addr = addr + sz;
		}

	}
	
	
};

static std::vector<chunk> read_chunks(const std::vector<uint8_t> &file_content, unsigned int &addr) {
	std::vector<chunk> result;
	chunk_carac mat = read_data<chunk_carac>(file_content, addr);

	//if type == 8, second method has to be applied

	for (size_t idx = 0; idx < mat.v0.x; idx++) {
		size_t count = read_data<size_t>(file_content, addr);
		size_t size_0 = read_data<size_t>(file_content, addr);
		unsigned int type = read_data<unsigned int>(file_content, addr);
		std::cout << "MatType: " << std::hex << type << " addr: " << addr << std::endl;

		if (type == 8) {


			result.push_back(chunk(file_content, addr, count - 4, 2, mat));


		}
		else
		{

			result.push_back(chunk(file_content, addr, count - 4, 1, mat));
		}
	}
	return result;
}
class MAT6 : public data {
public:
	MAT6() = default;

	int count;
	std::vector <int> int0s;
	std::vector <chunk> matm;
	//0x6d1165
	MAT6(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {

		count = read_data<size_t>(file_content, addr);
		
		for (unsigned int idx = 0; idx < count; idx++) {
			int0s.push_back(read_data<int>(file_content, addr));

			std::vector<chunk> chks = read_chunks(file_content, addr);
			matm.insert(matm.end(), chks.begin(), chks.end()); //le 4 est hardcodé
			
		}

		

	}


	void output_data() {
		
	}

};

class BON3 : public data {
public:
	BON3() = default;

	int int0, int1;
	std::string name;

	std::vector <chunk> matm;

	BON3(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {

		int0 = read_data<int>(file_content, addr);
		unsigned int addr_ = addr;
		name = read_string(file_content, addr);
		addr = addr_ + 0x40;
		int int1 = read_data<int>(file_content, addr);

		for (unsigned int idx = 0; idx < 3; idx++) {
			std::vector<chunk> chks = read_chunks(file_content, addr);
			matm.insert(matm.end(), chks.begin(), chks.end());
			/*mat_struct mat = read_data<mat_struct>(file_content, addr);
			mats.push_back(mat);
			matm.push_back(chunk(file_content, addr, mat.count - 4,2)); //le 4 est hardcodé*/

		}

	}


	void output_data() {

	}

};

struct IHDR {
	unsigned int int0, dim1, dim2, total_size; 
	uint16_t s1, bpp, s3, s4, type, s6;
	int not_used;
};
struct IALP {
	int i1, i2;
};

struct IMIP {
	vector3<int> v0;
};

inline size_t get_bpp(size_t id) {
	switch (id) {
		case 0:
		case 1:
		case 2:
		case 7:
		case 8:
		case 10:
			return 8;
		default:
			return 0;
		case 4:
			return 0x10;
		case 5:
			return 0x20;
		case 6:
			return 4;
			
	}
}

class ITP : public data {
	public:
		ITP() = default;
		IHDR hdr;
		IALP alp;
		IMIP mip;
		std::vector<uint8_t> content;
		std::vector <chunk> chunks;

		ITP(const std::vector<uint8_t> &file_content, unsigned int &addr){
			unsigned int magic = read_data<unsigned int>(file_content, addr);

			bool keep_reading = true;

			if (magic == ITP_ID) {
				while (keep_reading) {
					unsigned int magic2 = read_data<unsigned int>(file_content, addr);
					unsigned int size = read_data<unsigned int>(file_content, addr);
					switch (magic2) {
					case IHDR_ID:
						hdr = read_data<IHDR>(file_content, addr);
						break;
					case IALP_ID:
						alp = read_data<IALP>(file_content, addr);
						break;
					case IMIP_ID:
						mip = read_data<IMIP>(file_content, addr);
						break;
					case IDAT_ID: {

						unsigned int int0 = read_data<unsigned int>(file_content, addr);
						unsigned int int1 = read_data<unsigned int>(file_content, addr);
						if ((hdr.type & 0xffffff00) | (hdr.type == 2)) {

							throw std::exception("à debug plus tard, je ne sais pas combien d'itérations ont lieu à 6895fa\n");

							unsigned int int0 = read_data<unsigned int>(file_content, addr);
							unsigned int int1 = read_data<unsigned int>(file_content, addr);
							std::vector<chunk> chks = read_chunks(file_content, addr);
							chunks.insert(chunks.end(), chks.begin(), chks.end());
						}
						else {
							if ((hdr.type == 1) || (hdr.type == 2) || (hdr.type == 3)) {

								chunks = read_chunks(file_content, addr);

							}
							else {
								size_t content_size = get_bpp(hdr.bpp)  * hdr.dim1 * hdr.dim2;
								std::cout << "Texture size: " << std::hex << content_size << std::endl;
								std::cout << std::hex << hdr.dim1 << " x " << hdr.dim2 << " x " << get_bpp(hdr.bpp) << std::endl;
								std::cout << "Type: " << std::hex << hdr.type << std::endl;
								content.insert(content.end(), file_content.begin() + addr, file_content.begin() + addr + content_size);
								addr = addr + content_size;
							}

						}


						break; }
					case IEND_ID: {
						keep_reading = false;
						break; }
					default:
						throw std::exception(id_to_ascii(magic2).c_str());
					}
				}
		}
		

		}
		void output_data() {

		}
};
class TEXI : public data {
public:
	TEXI() = default;

	std::string name;
	ITP itp;

	TEXI(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {

		unsigned int addr_ = addr;
		name = read_string(file_content, addr);
		addr = addr_ + 0x24;
		itp = ITP(file_content, addr);

	}


	void output_data() {

	}

};

/*class CHID : public data {};
class JNTV : public data {}; 
class TEXI : public data {}; 
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

