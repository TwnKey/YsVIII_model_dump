#include "IT3File.h"
#include <memory>
#include <string>

std::shared_ptr<data> interpret_data(const std::vector<uint8_t> &content, uint32_t identifier, unsigned int &addr, size_t sz){

	switch (identifier) {
	case INFO_ID: //INFO
		return std::make_shared<INFO>(content, addr, sz);
	case RTY2_ID: //INFO
		return std::make_shared<RTY2>(content, addr, sz);
	case LIG3_ID: //INFO
		return std::make_shared<LIG3>(content, addr, sz);
	default: 
		addr += sz;
		std::cout << "skipped chunk because data type not yet reversed: " << id_to_ascii(identifier) << std::endl;
		return NULL;
	}
}

Chunk::Chunk(const std::vector<uint8_t> &file_content, unsigned int &addr) {
	this->FourCC = read_data<unsigned int>(file_content, addr);
	this->size = read_data<unsigned int>(file_content, addr);
	this->data = interpret_data(file_content, this->FourCC, addr, this->size);
}





IT3File::IT3File(const std::vector<uint8_t> &file_content)
{
	unsigned int current_addr = 0;

	while (current_addr < file_content.size()) {
		Chunk chk = Chunk(file_content, current_addr);
		this->chunks.push_back(chk);
	}
}


std::string IT3File::to_string() {
	std::string str = "";
	for (Chunk chk : this->chunks) {
		str = str + id_to_ascii(chk.FourCC) + "\n";
	}
	return str;
}


IT3File::~IT3File()
{
}
