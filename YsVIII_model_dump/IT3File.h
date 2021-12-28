#pragma once
#include <vector>
#include <memory>
#include <iostream>
/* They will all have their own way of outputting data */

class data {
	public:
	data() {};
	virtual ~data() = default;
	virtual void output_data() = 0;
};
class INFO : public data {
	public:
	INFO() = default;
	std::vector<float> floats;
	void output_data() {
		for (auto f : floats)
			std::cout << f << std::endl;
	}
};
/*class RTY2 : public data {};
class LIG3 : public data {};
class INFZ : public data {};
class BBOX : public data {}; //Bounding Box ?
class CHID : public data {};
class JNTV : public data {}; //Joints ?
class TEXI : public data {}; //Textures ?
class VPAX : public data {};
class MAT6 : public data {};
class BON3 : public data {};*/



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

