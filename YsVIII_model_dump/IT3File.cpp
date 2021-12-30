#include "IT3File.h"
#include <memory>
#include <string>
#include "DataBlock.h"

std::shared_ptr<data> interpret_data(const std::vector<uint8_t> &content, uint32_t identifier, unsigned int &addr, size_t sz){
	std::cout << "Processing chunk: " << id_to_ascii(identifier) << " at " << std::hex << addr << std::endl;
	switch (identifier) {
	case INFO_ID: 
		return std::make_shared<INFO>(content, addr, sz);
	case RTY2_ID: 
		return std::make_shared<RTY2>(content, addr, sz);
	case LIG3_ID: 
		return std::make_shared<LIG3>(content, addr, sz);
	case INFZ_ID: 
		return std::make_shared<INFZ>(content, addr, sz);
	case BBOX_ID: 
		return std::make_shared<BBOX>(content, addr, sz);
	case CHID_ID:
		return std::make_shared<CHID>(content, addr, sz);
	case JNTV_ID:
		return std::make_shared<JNTV>(content, addr, sz);
	case MAT6_ID:
		return std::make_shared<MAT6>(content, addr, sz);
	case BON3_ID:
		return std::make_shared<BON3>(content, addr, sz);
	case TEXI_ID:
		return std::make_shared<TEXI>(content, addr, sz);
	case ITP_ID:
		addr -= 8; //removing the "size" and identifier, since in the case of pure ITP files, there is no size after the four cc
		return std::make_shared<ITP>(content, addr,"test");
	default: 
		addr += sz;
		std::cout << "skipped chunk because data type not yet reversed: " << id_to_ascii(identifier) << std::endl;
		return NULL;
	}
}

Chunk::Chunk(const std::vector<uint8_t> &file_content, unsigned int &addr) {
	this->addr = addr;
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

void IT3File::output_data() {
	for (auto chunk : this->chunks) {
		std::cout << id_to_ascii(chunk.FourCC) << " addr: " << std::hex << chunk.addr << std::endl;
		if (chunk.data != NULL)
			chunk.data->output_data();
	}
}

IT3File::~IT3File()
{
}
