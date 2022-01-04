#include "DataBlock.h"
#include "utilities.h"
#include <fstream>
#include <iostream>
#include "FBXExporter.h"
//I should name it something else because I actually don't know what that is
size_t get_bpp(size_t id) {
switch (id) {
case 0:
case 1:
case 2:
case 7:
case 8:
case 10:
	return 8;
default:
	return 0;
case 4:
	return 0x10;
case 5:
	return 0x20;
case 6:
	return 4;

}
}




DataBlock::DataBlock(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t sz, unsigned int reading_method, BlockDesc caracs) {
	this->reading_method = reading_method;
	this->caracs = caracs;

	if (reading_method == 2) {

		unsigned int start = addr;
		//MATM 6dd4f0
		while (addr < (start + sz)) {
			uint8_t current_byte1 = read_data<uint8_t>(file_content, addr);
			uint8_t current_byte2 = read_data<uint8_t>(file_content, addr);
			if (current_byte1 == 0) {
				std::vector<uint8_t> el = std::vector<uint8_t>(current_byte2);
				std::copy(file_content.begin() + addr, file_content.begin() + addr + current_byte2, el.begin());
				addr = addr + current_byte2;
				content.insert(content.end(), el.begin(), el.end());
			}
			else {

				
				unsigned int start_idx = content.size() - 1 - current_byte2;


				for (unsigned int i = start_idx; i < start_idx + current_byte1; i++) {
					uint8_t prev_one = content[i];
					content.push_back(prev_one);
				}
				content.push_back(read_data<uint8_t>(file_content, addr));
				
			}
		}
	}
	else {
		std::vector<uint8_t> el = std::vector<uint8_t>(sz);
		content.insert(content.end(), file_content.begin() + addr, file_content.begin() + addr + sz);
		addr = addr + sz;
	}

}

void DataBlock::output_data(std::string node_name) {
	std::ofstream OutFile;
	OutFile.open(node_name, std::ios::out | std::ios::binary);
	OutFile.write((char*)content.data(), sizeof(char));
	OutFile.close();
}



std::vector<DataBlock> read_DataBlocks(const std::vector<uint8_t> &file_content, unsigned int &addr) {
std::vector<DataBlock> result;
BlockDesc mat = read_data<BlockDesc>(file_content, addr);

//if type == 8, second method has to be applied

for (size_t idx = 0; idx < mat.v0.x; idx++) {
	size_t count = read_data<uint32_t>(file_content, addr);
	size_t size_0 = read_data<uint32_t>(file_content, addr);
	unsigned int type = read_data<unsigned int>(file_content, addr);
	

	if (type == 8) {


		result.push_back(DataBlock(file_content, addr, count - 4, 2, mat));


	}
	else
	{
		result.push_back(DataBlock(file_content, addr, count - 4, 1, mat));
	}
}
return result;
}




INFO::INFO(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
	unsigned int start_addr = addr;

	text_id1 = read_string(file_content, addr);
	addr = start_addr + 0x40;
	transform = read_data<matrix4>(file_content, addr);
	v0 = read_data<vector3<float>>(file_content, addr);
	
	
}

void INFO::output_data(std::string node_name) {

	
}

RTY2::RTY2(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
	material_variant = read_data<unsigned int>(file_content, addr);
	byte = read_data<uint8_t>(file_content, addr);
	v0 = read_data<vector3<float>>(file_content, addr);
}


void RTY2::output_data(std::string node_name) {
	if (material_variant != 0){
	std::ofstream text_file;
	
	//text_file.open(node_name + "_MAT6.txt", std::ios_base::app);
	text_file.open("Liste Variants.txt", std::ios_base::app);
	text_file << "Material variant: " << material_variant << std::endl;
	text_file.close();
	text_file.open("Variants.txt", std::ios_base::app);
	text_file << "Material variant: " << material_variant << std::endl;
	text_file.close();
	}
	// << ", " << byte << ", " << v0.x << ", " << v0.y << ", " << v0.z << std::endl;
}

LIG3::LIG3(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
	
	v0 = read_data<vector4<float>>(file_content, addr);
	byte = read_data<uint8_t>(file_content, addr);
	float0 = read_data<float>(file_content, addr);
	v1 = read_data<vector4<float>>(file_content, addr);
}


void LIG3::output_data(std::string node_name) {
	

}


INFZ::INFZ(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
	v0 = read_data<vector4<int>>(file_content, addr);
}


void INFZ::output_data(std::string node_name) {
}


BBOX::BBOX(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
	a = read_data<vector3<float>>(file_content, addr);
	b = read_data<vector3<float>>(file_content, addr);
	c = read_data<vector3<float>>(file_content, addr);
	d = read_data<vector3<float>>(file_content, addr);
}


void BBOX::output_data(std::string node_name) {
}

CHID::CHID(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
	unsigned int start_addr = addr;

	parent = read_string(file_content, addr);
	addr = start_addr + 0x40;
	size_t nb_strings = read_data<unsigned int>(file_content, addr);

	for (unsigned int idx = 0; idx < nb_strings; idx++) {
		start_addr = addr;
		children.push_back(read_string(file_content, addr));
		addr = start_addr + 0x40;
	}
}


void CHID::output_data(std::string node_name) {
}



JNTV::JNTV(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {

	v0 = read_data<vector4<float>>(file_content, addr);
	id = read_data<int>(file_content, addr);

}


void JNTV::output_data(std::string node_name) {
}





//0x006B7076
//419b2b Info sur le material parsées ici 419b2b
MAT6::MAT6(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {

	count = read_data<uint32_t>(file_content, addr);

	for (unsigned int idx = 0; idx < count; idx++) {
		int0s.push_back(read_data<int>(file_content, addr));

		std::vector<DataBlock> chks = read_DataBlocks(file_content, addr);
		matm.insert(matm.end(), chks.begin(), chks.end()); //le 4 est hardcodé

	}
	for (unsigned int idx = 0; idx < matm.size(); idx++) {
		material current_mat;
		unsigned int addr_mat = 0;
		unsigned int magic_matm = read_data<uint32_t>(matm[idx].content, addr_mat);
		unsigned int MATM_flags = read_data<uint32_t>(matm[idx].content, addr_mat);
		unsigned int MATM_part_size = read_data<uint32_t>(matm[idx].content, addr_mat);

		addr_mat = 0x28;
		size_t count_parameters = read_data<uint32_t>(matm[idx].content, addr_mat);
		addr_mat += 4;
		size_t count_tex = read_data<uint32_t>(matm[idx].content, addr_mat);
		for (unsigned int idx_param = 0; idx_param < count_parameters; idx_param++)
			current_mat.parameters.push_back(read_data<vector4<float>>(matm[idx].content, addr_mat));
		unsigned int addr_tex = addr_mat;

		

		addr_mat = MATM_part_size;
		unsigned int magic_mate = read_data<uint32_t>(matm[idx].content, addr_mat);
		unsigned int MATE_flags = read_data<uint32_t>(matm[idx].content, addr_mat);
		size_t MATE_part_size = read_data<uint32_t>(matm[idx].content, addr_mat);
		unsigned int offset_to_first_texture = read_data<uint32_t>(matm[idx].content, addr_mat);
		unsigned int start_addr = addr_mat;
		current_mat.name = read_string(matm[idx].content, addr_mat);
		addr_mat = start_addr + offset_to_first_texture;
		size_t block_size = read_data<uint32_t>(matm[idx].content, addr_mat);

		std::vector<std::string> names;
		for (unsigned int idx_tex = 0; idx_tex < count_tex; idx_tex++){
		//while ((addr_mat + block_size) <= matm[idx].content.size()) {
			start_addr = addr_mat;
			std::string tex_name = read_string(matm[idx].content, addr_mat);
			names.push_back(tex_name);
			addr_mat = start_addr + block_size;
		}
		addr_mat = addr_tex;
		for (unsigned int idx_tex = 0; idx_tex < count_tex; idx_tex++) {
			texture tex = texture(matm[idx].content, addr_mat);
			tex.name = names[idx_tex];
			current_mat.textures.push_back(tex);
		}
			
		this->mats.push_back(current_mat);
	}



}


void MAT6::output_data(std::string node_name) {
	std::ofstream text_file;
	text_file.open("Liste Variants.txt", std::ios_base::app);
	//text_file.open(node_name + "_MAT6.txt", std::ios_base::app);
	
	text_file << "MAT6: " << node_name << std::endl;
	text_file << "count: " << count << std::endl;
	for (int i : int0s)
		text_file << i << std::endl;

	for (material mat : mats) {
		text_file << "MAT: " << mat.name << std::endl;
		for (auto param : mat.parameters) {
			text_file << param.x << ", " << param.y << ", " << param.z << ", " << param.t << std::endl;
		}
		for (texture tex : mat.textures) {
			text_file << "Name: " << tex.name << std::endl;
			text_file << std::hex << "Type: " << tex.texture_type << std::endl;
			text_file << std::hex << tex.uint0 << " " << tex.uint2 << " " << tex.uint3 << " " << tex.uint4 << " " << tex.uint5 << " " << tex.uint6 << std::endl;
		}
	}
	
	text_file << std::endl << std::endl;
	text_file.close();
	
	for (unsigned int i = 0; i < count; i++) {
		std::ofstream OutFile;
		OutFile.open(node_name + "_material_" + std::to_string(i) + ".bin", std::ios::out | std::ios::binary);
		OutFile.write((char*)matm[i].content.data(), matm[i].content.size() * sizeof(char));
		OutFile.close();
	}

}



BON3::BON3(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {

	int0 = read_data<int>(file_content, addr);
	unsigned int addr_ = addr;
	mesh_name = read_string(file_content, addr);
	addr = addr_ + 0x40;
	int int1 = read_data<int>(file_content, addr);
	std::vector <DataBlock> matm;

	for (unsigned int idx = 0; idx < 3; idx++) {
		std::vector<DataBlock> chks = read_DataBlocks(file_content, addr);
		matm.insert(matm.end(), chks.begin(), chks.end());
	}
	matms = matm;
	unsigned int addr_bone = 0;
	unsigned int idx_bone = 0;

	while (addr_bone < matm[0].content.size()) {
		unsigned int addr_start = addr_bone;
		std::string name = read_string(matm[0].content, addr_bone);
		addr_bone = addr_start;
		if (name.compare("") != 0)
			joints_names.push_back(name);
		addr_bone = addr_start + 0x40;
	}
	addr_bone = 0;
	while (addr_bone < matm[0].content.size()) {
		unsigned int addr_start = addr_bone;
		std::string name = read_string(matm[0].content, addr_bone);
		addr_bone = addr_start;
		if (name.compare("") != 0){
			matrix4 offset_mat = read_data<matrix4>(matm[2].content, addr_bone);
			bone b(name, offset_mat);
			bones[name] = b;
		}
		addr_bone = addr_start + 0x40;
	}


}


void BON3::output_data(std::string node_name) {
	std::ofstream text_file;
	text_file.open(node_name + "BON3.txt");
	text_file << "BON3: " << std::endl;
	text_file << "int0: " << int0 << std::endl;
	text_file << "int1: " << int0 << std::endl;
	text_file << "mesh name: " << mesh_name << std::endl;

	text_file.close();

	
	unsigned int idx = 0;
	for (auto mat : matms) {
		std::ofstream OutFile;
		OutFile.open(node_name + "_" + std::to_string(idx) + ".bones", std::ios::out | std::ios::binary);
		OutFile.write((char*)mat.content.data(), mat.content.size() * sizeof(char));
		OutFile.close();
		idx++;
	}
		
	
}







ITP::ITP(const std::vector<uint8_t> &file_content, unsigned int &addr, std::string name) {
	this->name = name;
	unsigned int magic = read_data<unsigned int>(file_content, addr);

	bool keep_reading = true;

	if (magic == ITP_ID) {
		while (keep_reading) {
			unsigned int magic2 = read_data<unsigned int>(file_content, addr);
			unsigned int size = read_data<unsigned int>(file_content, addr);
			switch (magic2) {
			case IHDR_ID:
				hdr = read_data<IHDR>(file_content, addr);
				break;
			case IALP_ID:
				alp = read_data<IALP>(file_content, addr);
				break;
			case IMIP_ID:
				mip = read_data<IMIP>(file_content, addr);
				break;
			case IDAT_ID: {

				unsigned int int0 = read_data<unsigned int>(file_content, addr);
				unsigned int int1 = read_data<unsigned int>(file_content, addr);
				if ((hdr.type & 0xffffff00) | (hdr.type == 2)) {

					throw std::exception("à debug plus tard, je ne sais pas combien d'itérations ont lieu à 6895fa\n");

					unsigned int int0 = read_data<unsigned int>(file_content, addr);
					unsigned int int1 = read_data<unsigned int>(file_content, addr);
					std::vector<DataBlock> chks = read_DataBlocks(file_content, addr);
					for (auto chk : chks)
						content.insert(content.end(), chk.content.begin(), chk.content.end());

				}
				else {
					if ((hdr.type == 1) || (hdr.type == 2) || (hdr.type == 3)) {

						std::vector<DataBlock> DataBlocks = read_DataBlocks(file_content, addr);
						for (auto chk : DataBlocks)
							content.insert(content.end(), chk.content.begin(), chk.content.end());
					}
					else {
						size_t content_size = get_bpp(hdr.bpp)  * hdr.dim1 * hdr.dim2;
						
						content.insert(content.end(), file_content.begin() + addr, file_content.begin() + addr + content_size);
						addr = addr + content_size;
					}

				}


				break; }
			case IHAS_ID: {
				ihas = read_data<IHAS>(file_content, addr);
				break;
			}
			case IEND_ID: {
				keep_reading = false;
				break;
			}
			default:
				throw std::exception(id_to_ascii(magic2).c_str());
			}
		}
	}


}
void ITP::add_header() {
	std::vector<uint8_t> height_b = int_to_bytes(hdr.dim1);
	std::vector<uint8_t> width_b = int_to_bytes(hdr.dim2);
	std::vector<uint8_t> total_size_b = int_to_bytes(this->content.size());

	std::vector<uint8_t> header = {
		0x44, 0x44, 0x53, 0x20, 0x7C, 0x00, 0x00, 0x00, 0x07, 0x10, 0x00, 0x00, height_b[0], height_b[1], height_b[2], height_b[3],
		width_b[0], width_b[1], width_b[2], width_b[3], total_size_b[0], total_size_b[1], total_size_b[2], total_size_b[3], 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
		0x04, 0x00, 0x00, 0x00, 0x44, 0x58, 0x31, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x62, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00 };
	this->content.insert(this->content.begin(), header.begin(), header.end());
}

void ITP::output_data(std::string node_name) {

	std::ofstream text_file;
	text_file.open(node_name + ""+ this->name+".txt");
	text_file << "ITP: " << std::endl; 
	text_file << "height: " << hdr.dim1 << std::endl;
	text_file << "width: " << hdr.dim2 << std::endl;
	text_file.close();

	
	this->unswizzle(hdr.dim1, hdr.dim2, 0x10);
	this->add_header();
	std::ofstream OutFile;
	OutFile.open(this->name + ".dds", std::ios::out | std::ios::binary);
	OutFile.write((char*)this->content.data(), this->content.size()*sizeof(char));
	OutFile.close();
}

//Adapted to C++ from GFD Studio after finding out about it using RawTex tool by daemon1

unsigned int Morton(unsigned int t, size_t sx, size_t sy)
{
	unsigned int num1;
	unsigned int num2 = num1 = 1;
	unsigned int num3 = t;
	unsigned int num4 = sx;
	unsigned int num5 = sy;
	unsigned int num6 = 0;
	unsigned int num7 = 0;

	while (num4 > 1 || num5 > 1)
	{
		if (num4 > 1)
		{
			num6 += num2 * (num3 & 1);
			num3 >>= 1;
			num2 *= 2;
			num4 >>= 1;
		}
		if (num5 > 1)
		{
			num7 += num1 * (num3 & 1);
			num3 >>= 1;
			num1 *= 2;
			num5 >>= 1;
		}
	}

	return num7 * sx + num6;
}


void ITP::unswizzle(size_t width, size_t height, size_t blockSize)
{
	std::vector<uint8_t> result = std::vector<uint8_t>(this->content);
	size_t heightTexels = height / 4;
	size_t heightTexelsAligned = (heightTexels + 7) / 8;
	size_t widthTexels = width / 4;
	size_t widthTexelsAligned = (widthTexels + 7) / 8;
	unsigned int dataIndex = 0;

	for (unsigned int  y = 0; y < heightTexelsAligned; ++y)
	{
		for (unsigned int  x = 0; x < widthTexelsAligned; ++x)
		{
			for (unsigned int  t = 0; t < 64; ++t)
			{
				unsigned int pixelIndex = Morton(t, 8, 8);
				unsigned int num8 = pixelIndex / 8;
				unsigned int num9 = pixelIndex % 8;
				unsigned int yOffset = (y * 8) + num8;
				unsigned int xOffset = (x * 8) + num9;

				if (xOffset < widthTexels && yOffset < heightTexels)
				{
					unsigned int destPixelIndex = yOffset * widthTexels + xOffset;
					unsigned int destIndex = blockSize * destPixelIndex;
					std::copy(this->content.begin() + dataIndex, this->content.begin() + dataIndex + blockSize, result.begin()+ destIndex);
					
					
				}

				dataIndex += blockSize;
			}
		}
	}
	this->content = result;
}



TEXI::TEXI(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {

	unsigned int addr_ = addr;
	name = read_string(file_content, addr);
	addr = addr_ + 0x24;
	itp = ITP(file_content, addr, name);

}


void TEXI::output_data(std::string node_name) {
	
	itp.output_data(node_name);
}



TEX2::TEX2(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {

	unsigned int uint0 = read_data<unsigned int>(file_content, addr);
	name = read_string(file_content, addr);
	itp = ITP(file_content, addr, name);

}


void TEX2::output_data(std::string node_name) {

	itp.output_data(node_name);
}

VPAX::VPAX(const std::vector<uint8_t> &file_content, unsigned int &addr, std::string name, int game_version) {

	this->name = name;

	size_t count = read_data<uint32_t>(file_content, addr);

	this->content_indexes.resize(count);
	this->content_vertices.resize(count);
	//I think that's the vertices
	for (unsigned int idx = 0; idx < count; idx++) {
		size_t sz = read_data<uint32_t>(file_content, addr); //Idk what that is yet.
		std::vector<DataBlock> DataBlocks = read_DataBlocks(file_content, addr);
		for (auto chk : DataBlocks) {
			content_vertices[idx].insert(content_vertices[idx].end(), chk.content.begin(), chk.content.end());
		}
			
	}
	//and here the indexes
	for (unsigned int idx = 0; idx < count; idx++) {
		size_t sz = read_data<uint32_t>(file_content, addr);
		std::vector<DataBlock> DataBlocks = read_DataBlocks(file_content, addr);
		for (auto chk : DataBlocks) {
			content_indexes[idx].insert(content_indexes[idx].end(), chk.content.begin(), chk.content.end());
		}
			
	}
	//Now to retrieve vertices and indexes


	for (unsigned int nb_mesh = 0; nb_mesh < count; nb_mesh++) {
		unsigned int addr_vpax = 0;

		mesh_data mesh;
		mesh.header = read_data<header_VPAC>(content_vertices[nb_mesh], addr_vpax);

		if (mesh.header.FourCC == VPAC_ID) {
			uint32_t mask = mesh.header.uint0[2];
			size_t count = 0x10;
			uint32_t uVar1 = 1, iVar4 = 0, local_c = 0;
			do {

				if ((mask & uVar1) != 0) {
					switch (uVar1) {
					case 1:
					case 2:
					case 4:
					case 8:
					case 0x200:
					case 0x400:
					case 0x800:
					case 0x100:
						iVar4 = iVar4 + 0x10;
						local_c++;
						break;
					case 0x10:
					case 0x20:
					case 0x40:
					case 0x80:
					case 0x1000:
					case 0x2000:
					case 0x4000:
					case 0x8000:
						iVar4 = iVar4 + 4;
						local_c++;
						break;

					}
				}
				uVar1 = uVar1 << 1 | (unsigned int)((int)uVar1 < 0);
				count = count - 1;
			} while (count != 0);

			size_t count1 = local_c;
			size_t count2 = mesh.header.uint0[0];

			mesh.material_id = mesh.header.uint0[0x44];
			mesh.block_size = iVar4;
			mesh.nb_blocks = mesh.header.uint0[0];

			size_t first_part_sz = mesh.block_size * mesh.nb_blocks; //taille total de la partie des vertices, je pense
			//le mesh est parsé là: 0x6b2432

			for (unsigned int idx = 0; idx < mesh.nb_blocks; idx++) {
				vertex v = read_data<vertex>(content_vertices[nb_mesh], addr_vpax);
				mesh.vertices.push_back(v);
			}

			addr_vpax = 0;
			size_t nb_indexes;
			if (game_version == 1) {
				nb_indexes = content_indexes[nb_mesh].size() / sizeof(uint16_t);
				for (unsigned int idx = 0; idx < nb_indexes; idx++) {
					unsigned int i = read_data<uint16_t>(content_indexes[nb_mesh], addr_vpax);
					mesh.indexes.push_back(i);
				}
			}
				
			else if (game_version == 2) {
				nb_indexes = content_indexes[nb_mesh].size() / sizeof(uint32_t);
				for (unsigned int idx = 0; idx < nb_indexes; idx++) {
					unsigned int i = read_data<uint32_t>(content_indexes[nb_mesh], addr_vpax);
					mesh.indexes.push_back(i);
				}
			}
				

			
			this->meshes_d.push_back(mesh);
		}
		else {
			throw std::exception("Not sure if it happens but needs to be investigated if it does");
		}
	}
	

	

}


void VPAX::output_data(std::string node_name) {
	for (unsigned int idx_mesh = 0; idx_mesh < meshes_d.size(); idx_mesh++) {
		std::ofstream OutFile;
		OutFile.open(name + "_vertices_"+std::to_string(idx_mesh)+".idx", std::ios::out | std::ios::binary);
		OutFile.write((char*)this->content_vertices[idx_mesh].data(), this->content_vertices[idx_mesh].size() * sizeof(char));
		OutFile.close();
		OutFile.open(name + "_indexes_" + std::to_string(idx_mesh) + ".idx", std::ios::out | std::ios::binary);
		OutFile.write((char*)this->content_indexes[idx_mesh].data(), this->content_indexes[idx_mesh].size() * sizeof(char));
		OutFile.close();
	}
	
}

KAN7::KAN7(const std::vector<uint8_t>& file_content, unsigned int& addr, size_t size) {

	//
	for (unsigned int i = 0; i < 0x28 / 4; i++)
		things[i] = read_data<unsigned int>(file_content, addr);
	
	if (things[0] > 0){
		unsigned int uint0 = read_data<unsigned int>(file_content, addr);
		std::vector<DataBlock> chks = read_DataBlocks(file_content, addr);
		matms.push_back(chks);
	}
	if (things[1] > 0) {
		unsigned int uint1 = read_data<unsigned int>(file_content, addr);
		std::vector<DataBlock> chks = read_DataBlocks(file_content, addr);
		matms.push_back(chks);
	}
	if (things[2] > 0) {
		unsigned int uint2 = read_data<unsigned int>(file_content, addr);
		std::vector<DataBlock> chks = read_DataBlocks(file_content, addr);
		matms.push_back(chks);
	}
	if (things[3] > 0) {
		unsigned int uint3 = read_data<unsigned int>(file_content, addr);
		std::vector<DataBlock> chks = read_DataBlocks(file_content, addr);
		matms.push_back(chks);
	}
	if (things[4] > 0) {
		unsigned int uint4 = read_data<unsigned int>(file_content, addr);
		std::vector<DataBlock> chks = read_DataBlocks(file_content, addr);
		matms.push_back(chks);
	}
	for (auto mats : matms) {
		std::vector<uint8_t> content;
		for (auto mat : mats) {
			content.insert(content.end(), mat.content.begin(), mat.content.end());
		}
		unsigned int addr_mat = 0;
		unsigned int magic = read_data<unsigned int>(content, addr_mat);
		unsigned int check = read_data<unsigned int>(content, addr_mat);
		addr_mat += 4;
		unsigned int nb_structs = read_data<unsigned int>(content, addr_mat);
		addr_mat += 0x30;
		std::vector<kan> current_kan;
		for (unsigned int i = 0; i < nb_structs; i++) {
			kan k = read_data<kan>(content, addr_mat);
			current_kan.push_back(k);
		}
		kans.push_back(current_kan);
		

	}
}


void KAN7::output_data(std::string node_name) {

	unsigned int idx = 0;
	for (auto mats : matms) {
		std::ofstream OutFile;
		OutFile.open(node_name + "_" + std::to_string(idx) + ".kan7", std::ios::out | std::ios::binary);
		std::vector<uint8_t> content;
		for (auto mat : mats) {
			content.insert(content.end(), mat.content.begin(), mat.content.end());
		}
		OutFile.write((char*)content.data(), content.size() * sizeof(char));
		OutFile.close();
		idx++;
	}
}