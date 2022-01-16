#include "IT3File.h"
#include <memory>
#include <string>
#include "DataBlock.h"
#include "DAEExporter.h"



IT3File::IT3File(const std::vector<uint8_t>& file_content)
{
	unsigned int current_addr = 0;
	chunk current_chunk(current_addr + 4);
	bool started = false;
	while (current_addr < file_content.size()) {

		unsigned int FourCC = read_data<unsigned int>(file_content, current_addr);
		size_t size = read_data<uint32_t>(file_content, current_addr);
		switch (FourCC) {
		case INFO_ID:
			if (!started)
				started = true;
			else
				this->chunks[current_chunk.info->text_id1] = current_chunk;

			current_chunk = chunk(current_addr); //reset
			current_chunk.info = new INFO(file_content, current_addr, size);
			break;
		case RTY2_ID:
			current_chunk.rty2 = new RTY2(file_content, current_addr, size);
			break;
		case LIG3_ID:
			current_chunk.lig3 = new LIG3(file_content, current_addr, size);
			break;
		case INFZ_ID:
			current_chunk.infz = new INFZ(file_content, current_addr, size);
			break;
		case BBOX_ID:
			current_chunk.bbox = new BBOX(file_content, current_addr, size);
			break;
		case CHID_ID:
			current_chunk.chid = new CHID(file_content, current_addr, size);
			break;
		case JNTV_ID:
			current_chunk.jntv = new JNTV(file_content, current_addr, size);
			break;
		case MAT6_ID:
			current_chunk.mat6 = new MAT6(file_content, current_addr, size);
			break;
		case BON3_ID:
			current_chunk.bon3 = new BON3(file_content, current_addr, size);
			break;
		case TEXI_ID:
			current_chunk.texi.push_back(TEXI(file_content, current_addr, size));
			break;
		case TEX2_ID:
			current_chunk.tex2.push_back(TEX2(file_content, current_addr, size));
			break;
		case ITP_ID:
			current_addr -= 8; //removing the "size" and identifier, since in the case of pure ITP files, there is no size after the four cc
			current_chunk.itp.push_back(ITP(file_content, current_addr, "test"));
			break;
		case VPAX_ID:
			current_chunk.vpax = new VPAX(file_content, current_addr, current_chunk.info->text_id1, 1);
			break;
		case VP11_ID:
			current_chunk.vpax = new VPAX(file_content, current_addr, current_chunk.info->text_id1, 2);
			break;
		case KAN7_ID:
			current_chunk.kan7 = new KAN7(file_content, current_addr, size);
			break;
		default:
			current_addr += size;
			std::cout << "skipped chunk because data type not yet reversed: " << id_to_ascii(FourCC) << std::hex << " size: " << size << std::endl;
			break;

		}


	}
	if (started)
		this->chunks[current_chunk.info->text_id1] = current_chunk;
}


std::string IT3File::to_string() {
	std::string str = "";
	for (auto it : this->chunks) {
		
		str = str + it.second.to_string() + "\n";
	}
	return str;
}

void IT3File::output_data(std::string scene_folder) {
	for (auto it : this->chunks) {
		it.second.output_data(scene_folder);
	}
}





IT3File::~IT3File()
{
}


void chunk::output_data(std::string scene_folder) {
	if (info){
		//info->output_data(info->text_id1, scene_folder);
		/*if (rty2)
			rty2->output_data(info->text_id1, scene_folder);
		if (lig3)
			lig3->output_data(info->text_id1, scene_folder);
		if (infz)
			infz->output_data(info->text_id1, scene_folder);
		if (bbox)
			bbox->output_data(info->text_id1, scene_folder);
		if (chid)
			chid->output_data(info->text_id1, scene_folder);
		if (jntv)
			jntv->output_data(info->text_id1, scene_folder);
		if (mat6)
			mat6->output_data(info->text_id1, scene_folder);
		if (bon3)
			bon3->output_data(info->text_id1, scene_folder);*/
		if (!texi.empty()) {
			for (auto tex : texi)
				tex.output_data(info->text_id1, scene_folder);
		}
		if (!tex2.empty()) {
			for (auto tex : tex2)
				tex.output_data(info->text_id1, scene_folder);
		}
		if (!itp.empty()) {
			for (auto tp : itp)
				tp.output_data(info->text_id1, scene_folder);
		}
		/*if (vpax)
			vpax->output_data(info->text_id1, scene_folder);
		if (kan7)
			kan7->output_data(info->text_id1, scene_folder);*/
	}
}

std::string chunk::to_string() {
	std::string result = "";
	if (info)
		result = "chunk:" + info->text_id1 +"\n";
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
	if (!texi.empty())
		result = result + "TEXI ";
	if (!tex2.empty())
		result = result + "TEX2 ";
	if (!itp.empty()) 
		result = result + "ITP ";
	if (vpax)
		result = result + "VPAX ";
	if (kan7)
		result = result + "KAN7 ";


	return result;
}

void IT3File::add_kan7_from_m_file(IT3File m_file) {

	for (auto it : m_file.chunks) {
		//
		std::string chunk_name = it.second.info->text_id1;
		if (it.second.kan7) {
			
			if (this->chunks.count(chunk_name) == 1) {
				this->chunks[chunk_name].kan7 = it.second.kan7;
			}
		
		}
	}
}