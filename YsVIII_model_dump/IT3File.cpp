#include "IT3File.h"
#include "utilities.h"
#include <memory>
#include <string>

std::shared_ptr<data> interpret_data(const std::vector<uint8_t> &content, uint32_t identifier){

	switch (identifier) {
	case 0x4F464E49: //INFO
		return std::make_shared<INFO>();
	default: 
		std::cout << "skipped chunk because data type not yet reversed: " << id_to_ascii(identifier) << std::endl;
		return NULL;
	}
}

Chunk::Chunk(const std::vector<uint8_t> &file_content, unsigned int &addr) {
	this->FourCC = read_data<unsigned int>(file_content, addr);
	this->size = read_data<unsigned int>(file_content, addr);
	this->data = interpret_data(file_content, this->FourCC);
	addr += size;
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
