#include "IT3File.h"
#include <memory>
#include <string>
#include "DataBlock.h"



/* It looks like the chunk "INFO" is delimiting each entry in the file, and I can't really describe what is an entry
except it's a group of chunks of different types, all unique*/


IT3File::IT3File(const std::vector<uint8_t> &file_content)
{
	unsigned int current_addr = 0;
	item current_item(current_addr+4);
	bool started = false;
	while (current_addr < file_content.size()) {
		
		unsigned int FourCC = read_data<unsigned int>(file_content, current_addr);
		size_t size = read_data<size_t>(file_content, current_addr);

		switch (FourCC) {
			case INFO_ID:
				if (!started)
					started = true;
				else
					this->items.push_back(current_item);

				current_item = item(current_addr); //reset
				current_item.info = new INFO(file_content, current_addr, size);
				break;
			case RTY2_ID:
				current_item.rty2 = new RTY2(file_content, current_addr, size);
				break;
			case LIG3_ID:
				current_item.lig3 = new LIG3(file_content, current_addr, size);
				break;
			case INFZ_ID:
				current_item.infz = new INFZ(file_content, current_addr, size);
				break;
			case BBOX_ID:
				current_item.bbox = new BBOX(file_content, current_addr, size);
				break;
			case CHID_ID:
				current_item.chid = new CHID(file_content, current_addr, size);
				break;
			case JNTV_ID:
				current_item.jntv = new JNTV(file_content, current_addr, size);
				break;
			case MAT6_ID:
				current_item.mat6 = new MAT6(file_content, current_addr, size);
				break;
			case BON3_ID:
				current_item.bon3 = new BON3(file_content, current_addr, size);
				break;
			case TEXI_ID:
				current_item.texi = new TEXI(file_content, current_addr, size);
				break;
			case TEX2_ID:
				current_item.tex2 = new TEX2(file_content, current_addr, size);
				break;
			case ITP_ID:
				current_addr -= 8; //removing the "size" and identifier, since in the case of pure ITP files, there is no size after the four cc
				current_item.itp = new ITP(file_content, current_addr, "test");
				break;
			case VPAX_ID:
				current_item.vpax = new VPAX(file_content, current_addr, current_item.info->text_id1);
				break;
			default:
				current_addr += size;
				std::cout << "skipped chunk because data type not yet reversed: " << id_to_ascii(FourCC) << std::endl;
				break;
			
		}


	}
	if (started)
		this->items.push_back(current_item);
}


std::string IT3File::to_string() {
	std::string str = "";
	for (item it : this->items) {
		
		str = str + it.to_string() + "\n";
	}
	return str;
}

void IT3File::output_data() {
	for (auto it : this->items) {
		//std::cout << id_to_ascii(chunk.FourCC) << " addr: " << std::hex << chunk.addr << std::endl;
		it.output_data();
	}
}

IT3File::~IT3File()
{
}

void item::output_data() {
	if (info)
		info->output_data();
	if (rty2)
		rty2->output_data();
	if (lig3)
		lig3->output_data();
	if (infz)
		infz->output_data();
	if (bbox)
		bbox->output_data();
	if (chid)
		chid->output_data();
	if (jntv)
		jntv->output_data();
	if (mat6)
		mat6->output_data();
	if (bon3)
		bon3->output_data();
	if (texi)
		texi->output_data();
	if (tex2)
		tex2->output_data();
	if (itp)
		itp->output_data();
	if (vpax)
		vpax->output_data();

}

std::string item::to_string() {
	std::string result = "ITEM:\n";
	if (info)
		result = result + "INFO ";
	if (rty2)
		result = result + "RTY2 ";
	if (lig3)
		result = result + "LIG3 ";
	if (infz)
		result = result + "INFZ ";
	if (bbox)
		result = result + "BBOX ";
	if (chid)
		result = result + "CHID ";
	if (jntv)
		result = result + "JNTV ";
	if (mat6)
		result = result + "MAT6 ";
	if (bon3)
		result = result + "BON3 ";
	if (texi)
		result = result + "TEXI ";
	if (tex2)
		result = result + "TEX2 ";
	if (itp)
		result = result + "ITP ";
	if (vpax)
		result = result + "VPAX ";


	return result;
}