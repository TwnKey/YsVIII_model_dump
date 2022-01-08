#pragma once
#include <string>
#include <map>
#include <vector>



/* My guess is that one of the parts of the file points to the KAN7 data for each bone; it gives start and end index
for a given motion, and those motions are reused throughout animations (which is a concatenation of motions) but that's just a guess*/
struct motion_data {
	std::string name;
	unsigned int duration;
	unsigned int start;
	unsigned int end;
};
class MTBFile
{
public:
	std::vector<motion_data> data;

	std::vector<std::string> bones;

	MTBFile(const std::vector<uint8_t>& file_content);
	~MTBFile() = default;

};

