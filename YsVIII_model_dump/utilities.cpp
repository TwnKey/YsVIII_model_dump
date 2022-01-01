#include "utilities.h"


std::string read_string(const std::vector<uint8_t> &file_content, unsigned int &addr) {
	std::string result = std::string((char *)(file_content.data() + addr));
	addr += result.length()+1;
	return result;
}


std::string id_to_ascii(unsigned int identifier_uint) {
	char identifier[5] = { 0 };
	memcpy(identifier, &identifier_uint, 4);

	return std::string(identifier);
}
std::vector<uint8_t> int_to_bytes(int x) {
	std::vector<uint8_t> result = std::vector<uint8_t>(4);
	result[0] = (x & 0x000000FF);
	result[1] = (x & 0x0000FF00) >> 8;
	result[2] = (x & 0x00FF0000) >> 16;
	result[3] = (x & 0xFF000000) >> 24;
	return result;
}