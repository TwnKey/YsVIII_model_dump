#include "MTBFile.h"
#include "utilities.h"

MTBFile::MTBFile(const std::vector<uint8_t>& file_content) {
	unsigned int addr_mtb = 0;
	std::string magic_str = read_string(file_content, addr_mtb);
	if (magic_str == "YS7_MTD") {
		unsigned int version = read_data<unsigned int>(file_content, addr_mtb);
		if (version >= 0x2000000)
			addr_mtb += 8;

		unsigned int count1 = read_data<unsigned int>(file_content, addr_mtb);
		for (unsigned int i = 0; i < count1; i++) {
			addr_mtb += 6;
		}
		addr_mtb += 0x12;

		//6f8360
		uint16_t id;
		while (addr_mtb < file_content.size()) {
			
			id = read_data<uint16_t>(file_content, addr_mtb);
			std::cout << std::hex << "ID: " << id << ", addr = " << addr_mtb << std::endl;
			switch (id) {
				case 0x104:
					addr_mtb += 0x40;
					
					break;
				case 0x110:
				case 0x107:
					addr_mtb += 0x84;
					break;
				case 0x102: //The names section
					{
					unsigned int nb_entries = read_data<unsigned int>(file_content, addr_mtb);
					unsigned int size_entry = read_data<unsigned int>(file_content, addr_mtb);
					for (unsigned int i = 0; i < nb_entries; i++) {
						motion_data ani;

						unsigned int addr_ani = addr_mtb;
						ani.name = read_string(file_content, addr_mtb);
						addr_mtb = addr_ani + 0x20;
						ani.duration = read_data<unsigned int>(file_content, addr_mtb);
						ani.start = read_data<unsigned int>(file_content, addr_mtb);
						ani.end = read_data<unsigned int>(file_content, addr_mtb);
						this->data.push_back(ani);
					}
					break; }
				case 0x10B: {
					unsigned int nb_entries = read_data<unsigned int>(file_content, addr_mtb);
					unsigned int size_entry = read_data<unsigned int>(file_content, addr_mtb);
					unsigned int total_size_in_bytes = nb_entries * size_entry;
					addr_mtb += total_size_in_bytes;
					break; }
				case 0x108:
				{
					unsigned int count = read_data<unsigned int>(file_content, addr_mtb);

					for (unsigned int i = 0; i < count; i++) {
						addr_mtb += 0x20;
						addr_mtb += 1;//byte
						addr_mtb += 4;
						unsigned int next_size = read_data<unsigned int>(file_content, addr_mtb);
						addr_mtb += next_size;
					}
					
					break;
				}
				case 0x109:
				{
					unsigned int count = read_data<unsigned int>(file_content, addr_mtb);
					for (unsigned int i = 0; i < count; i++) {
						addr_mtb += 4;
						unsigned int size_entry = read_data<unsigned int>(file_content, addr_mtb);
						addr_mtb += size_entry;
					}
					
					break;
				}
				case 0x10D:
				{
					unsigned int count = read_data<unsigned int>(file_content, addr_mtb);
					addr_mtb += 4;
					addr_mtb += count * 0x1C - 4;
					break;
				}
				case 0x10F: //Bones
				{
					unsigned int nb_entries = read_data<unsigned int>(file_content, addr_mtb);
					unsigned int size_entry = read_data<unsigned int>(file_content, addr_mtb);
					for (unsigned int i = 0; i < nb_entries; i++) {
						unsigned int addr_bone = addr_mtb;
						bones.push_back(read_string(file_content, addr_mtb));
						addr_mtb = addr_bone + size_entry;
					}
					std::cout << "Number of bones: " << std::hex << bones.size()<< std::endl;
					break;
				}
				case 0x117: 
				{
					unsigned int nb_entries = read_data<unsigned int>(file_content, addr_mtb);
					addr_mtb += 4;
					for (unsigned int i = 0; i < nb_entries; i++) {
						addr_mtb += 4;
					}
					
					break;
				}
				case 0x111: //Bone head
				{
					unsigned int size_entry = read_data<unsigned int>(file_content, addr_mtb);
					
					addr_mtb += size_entry;
					break; 
				}
				case 0x112: //Bone top
				{
					unsigned int size_entry = read_data<unsigned int>(file_content, addr_mtb);

					addr_mtb += size_entry;

					float f1 = read_data<float>(file_content, addr_mtb);
					float f2 = read_data<float>(file_content, addr_mtb);
					float f3 = read_data<float>(file_content, addr_mtb);
					break;
				}
				case 0x113: //Bone top again
				{

					unsigned int size_entry = read_data<unsigned int>(file_content, addr_mtb);

					addr_mtb += size_entry;
					break;
				}
				case 0x114: //Bone top again again
				{

					unsigned int size_entry = read_data<unsigned int>(file_content, addr_mtb);

					addr_mtb += size_entry;
					break;
		}
				case 0x115: //Nothing
				{

					unsigned int size_entry = read_data<unsigned int>(file_content, addr_mtb);

					addr_mtb += size_entry;
					float f1 = read_data<float>(file_content, addr_mtb);
					break;
					}
				case 0x116: //Bone Root
				{

					unsigned int size_entry = read_data<unsigned int>(file_content, addr_mtb);

					addr_mtb += size_entry;
					break;
					}
				case 0x119: //Nothing
				{

					unsigned int size_entry = read_data<unsigned int>(file_content, addr_mtb);

					addr_mtb += size_entry;
					float f1 = read_data<float>(file_content, addr_mtb);
					float f2 = read_data<float>(file_content, addr_mtb);
					float f3 = read_data<float>(file_content, addr_mtb);
					break;
					}
				case 0x11A: {

					vector4<float> f1 = read_data<vector4<float>>(file_content, addr_mtb);
					vector4<float> f2 = read_data<vector4<float>>(file_content, addr_mtb);
					break;
					}
				case 0x11C: {

					float f = read_data<float>(file_content, addr_mtb);
					
					break;
					}
				case 0x11B: {
					unsigned int nb_entries = read_data<unsigned int>(file_content, addr_mtb);
					unsigned int size_entry = read_data<unsigned int>(file_content, addr_mtb);
					unsigned int total_size_in_bytes = nb_entries * size_entry;
					addr_mtb += total_size_in_bytes;
					

					break;
					}
				case 0x11D:
				{
					unsigned int size_entry = read_data<unsigned int>(file_content, addr_mtb);
					addr_mtb += size_entry;
					addr_mtb += 4;

					break;
					}
				case 0x11E:
				{
					unsigned int nb_entries = read_data<unsigned int>(file_content, addr_mtb);
					unsigned int size_entry = read_data<unsigned int>(file_content, addr_mtb);
					unsigned int total_size_in_bytes = nb_entries * size_entry;
					addr_mtb += total_size_in_bytes;
					std::cout << "Finished last part MTB " << std::endl;
					break;
				}
				default: {
					throw std::exception("Pas encore fait");
					break;
					}
			}
		}
	
	}

}