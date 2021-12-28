#include "utilities.h"


std::string read_string(const std::vector<uint8_t> &file_content, unsigned int &addr) {
	std::string result = std::string((char *)(file_content.data() + addr));
	addr += result.length();
	return result;
}
std::string id_to_ascii(unsigned int identifier_uint) {
	char identifier[5] = { 0 };
	memcpy(identifier, &identifier_uint, 4);

	return std::string(identifier);
}
