#pragma once
#include <string>
#include <map>
#include <vector>
struct animation_data {
	std::string name;
	
};
class MTBFile
{
public:
	std::map <std::string, animation_data> data;

	MTBFile(const std::vector<uint8_t>& file_content);
	~MTBFile() = default;

};

