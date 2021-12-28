#pragma once
#include <vector>
#include <iostream>
#include <string>

template<typename T>
struct vector4 {
	T x;
	T y;
	T z; 
	T t;
};
template<typename T>
struct vector3 {
	T x;
	T y;
	T z;
};

template<typename T>
T read_data(const std::vector<uint8_t> &file_content, unsigned int &addr) {
	T result;
	memcpy(&result, file_content.data() + addr, sizeof(T));
	addr += sizeof(T);
	return result;
}
std::string read_string(const std::vector<uint8_t> &file_content, unsigned int &addr);

std::string id_to_ascii(unsigned int identifier_uint);
