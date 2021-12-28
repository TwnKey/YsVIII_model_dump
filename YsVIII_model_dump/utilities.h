#pragma once
#include <vector>
#include <iostream>
template<typename T>
T read_data(std::vector<uint8_t> file_content, unsigned int &addr) {
	T result;
	memcpy(&result, file_content.data() + addr, sizeof(T));
	addr += sizeof(T);
	return result;
}

std::string id_to_ascii(unsigned int identifier_uint) {
	char identifier[5] = { 0 };
	memcpy(identifier, &identifier_uint, 4);

	return std::string(identifier);
}
