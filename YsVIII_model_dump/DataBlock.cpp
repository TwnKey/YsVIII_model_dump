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

void DataBlock::output_data(std::string filepath) {
	std::ofstream OutFile;
	OutFile.open(filepath, std::ios::out | std::ios::binary);
	OutFile.write((char*)content.data(), sizeof(char));
	OutFile.close();
}



std::vector<DataBlock> read_DataBlocks(const std::vector<uint8_t> &file_content, unsigned int &addr) {
std::vector<DataBlock> result;
BlockDesc mat = read_data<BlockDesc>(file_content, addr);

//if type == 8, second method has to be applied

for (size_t idx = 0; idx < mat.v0.x; idx++) {
	size_t count = read_data<size_t>(file_content, addr);
	size_t size_0 = read_data<size_t>(file_content, addr);
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

	while (addr < start_addr + size) {
		floats.push_back(read_data<float>(file_content, addr));
	}
}

void INFO::output_data() {
	std::cout << text_id1 << std::endl;
	for (auto f : floats)
		std::cout << f << std::endl;
}

RTY2::RTY2(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
	float0 = read_data<float>(file_content, addr);
	byte = read_data<uint8_t>(file_content, addr);
	v0 = read_data<vector3<float>>(file_content, addr);
}


void RTY2::output_data() {
	std::cout << float0 << ", " << byte << ", " << v0.x << ", " << v0.y << ", " << v0.z << std::endl;
}

LIG3::LIG3(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
	v0 = read_data<vector4<float>>(file_content, addr);
	byte = read_data<uint8_t>(file_content, addr);
	float0 = read_data<float>(file_content, addr);
	v1 = read_data<vector4<float>>(file_content, addr);
}


void LIG3::output_data() {
	std::cout << v0.x << ", " << v0.y << ", " << v0.z << ", " << v0.t << std::endl;
	std::cout << byte << std::endl;
	std::cout << float0 << std::endl;
	std::cout << v1.x << ", " << v1.y << ", " << v1.z << ", " << v1.t << std::endl;

}


INFZ::INFZ(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
	v0 = read_data<vector4<int>>(file_content, addr);
}


void INFZ::output_data() {
	std::cout << v0.x << ", " << v0.y << ", " << v0.z << ", " << v0.t << std::endl;
}


BBOX::BBOX(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
	a = read_data<vector3<float>>(file_content, addr);
	b = read_data<vector3<float>>(file_content, addr);
	c = read_data<vector3<float>>(file_content, addr);
	d = read_data<vector3<float>>(file_content, addr);
}


void BBOX::output_data() {
	std::cout << a.x << ", " << a.y << ", " << a.z << std::endl;
	std::cout << b.x << ", " << b.y << ", " << b.z << std::endl;
	std::cout << c.x << ", " << c.y << ", " << c.z << std::endl;
	std::cout << d.x << ", " << d.y << ", " << d.z << std::endl;
}

CHID::CHID(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {
	unsigned int start_addr = addr;

	intro = read_string(file_content, addr);
	addr = start_addr + 0x40;
	size_t nb_strings = read_data<unsigned int>(file_content, addr);

	for (unsigned int idx = 0; idx < nb_strings; idx++) {
		start_addr = addr;
		strs.push_back(read_string(file_content, addr));
		addr = start_addr + 0x40;
	}
}


void CHID::output_data() {
	std::cout << intro << std::endl;
	for (unsigned int idx_str = 0; idx_str < strs.size(); idx_str++)
		std::cout << strs[idx_str] << std::endl;
}



JNTV::JNTV(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {

	v0 = read_data<vector4<float>>(file_content, addr);
	id = read_data<int>(file_content, addr);

}


void JNTV::output_data() {
	std::cout << id << std::endl;
	std::cout << v0.x << ", " << v0.y << ", " << v0.z << ", " << v0.t << std::endl;
}





//0x6d1165
MAT6::MAT6(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {

	count = read_data<size_t>(file_content, addr);

	for (unsigned int idx = 0; idx < count; idx++) {
		int0s.push_back(read_data<int>(file_content, addr));

		std::vector<DataBlock> chks = read_DataBlocks(file_content, addr);
		matm.insert(matm.end(), chks.begin(), chks.end()); //le 4 est hardcodé

	}



}


void MAT6::output_data() {

}



BON3::BON3(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {

	int0 = read_data<int>(file_content, addr);
	unsigned int addr_ = addr;
	name = read_string(file_content, addr);
	addr = addr_ + 0x40;
	int int1 = read_data<int>(file_content, addr);

	for (unsigned int idx = 0; idx < 3; idx++) {
		std::vector<DataBlock> chks = read_DataBlocks(file_content, addr);
		matm.insert(matm.end(), chks.begin(), chks.end());
		/*mat_struct mat = read_data<mat_struct>(file_content, addr);
		mats.push_back(mat);
		matm.push_back(DataBlock(file_content, addr, mat.count - 4,2)); //le 4 est hardcodé*/

	}

}


void BON3::output_data() {

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
						std::cout << "Texture size: " << std::hex << content_size << std::endl;
						std::cout << std::hex << hdr.dim1 << " x " << hdr.dim2 << " x " << get_bpp(hdr.bpp) << std::endl;
						std::cout << "Type: " << std::hex << hdr.type << std::endl;
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

void ITP::output_data() {
	std::cout << "height: " << hdr.dim1 << ", width: " << hdr.dim2 << std::endl;
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


void TEXI::output_data() {
	
	itp.output_data();
}



TEX2::TEX2(const std::vector<uint8_t> &file_content, unsigned int &addr, size_t size) {

	unsigned int uint0 = read_data<unsigned int>(file_content, addr);
	name = read_string(file_content, addr);
	itp = ITP(file_content, addr, name);

}


void TEX2::output_data() {

	itp.output_data();
}

VPAX::VPAX(const std::vector<uint8_t> &file_content, unsigned int &addr, std::string name) {

	this->name = name;

	size_t count = read_data<size_t>(file_content, addr);
	//I think that's the vertices
	for (unsigned int idx = 0; idx < count; idx++) {
		size_t sz = read_data<size_t>(file_content, addr); //Idk what that is yet.
		std::vector<DataBlock> DataBlocks = read_DataBlocks(file_content, addr);
		for (auto chk : DataBlocks) {
			content_vertices.insert(content_vertices.end(), chk.content.begin(), chk.content.end());

			/*std::vector<float> floats((chk.content.size() / sizeof(float)) - 1, 0.0f);
			memcpy(floats.data(), chk.content.data() + 1, floats.size() * sizeof(float));
			vertices.insert(vertices.end(), floats.begin(), floats.end());*/
		}
			
	}
	//and here the indexes
	for (unsigned int idx = 0; idx < count; idx++) {
		size_t sz = read_data<size_t>(file_content, addr); 
		std::vector<DataBlock> DataBlocks = read_DataBlocks(file_content, addr);
		for (auto chk : DataBlocks) {
			content_indexes.insert(content_indexes.end(), chk.content.begin(), chk.content.end());

			/*std::vector<unsigned int> uints(chk.content.size() / sizeof(unsigned int), 0.0f);
			memcpy(uints.data(), chk.content.data(), uints.size() * sizeof(unsigned int));
			indexes.insert(indexes.end(), indexes.begin(), uints.end());*/
		}
			
	}
	//Now to retrieve vertices and indexes
	unsigned int addr_vpax = 0;
	header = read_data<header_VPAC>(content_vertices, addr_vpax);

	if (header.FourCC == VPAC_ID) {
		uint32_t mask = header.uint0[2]; 
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
		size_t count2 = header.uint0[0];

		block_size = iVar4;
		nb_blocks = header.uint0[0];
		
		size_t first_part_sz = block_size * nb_blocks; //taille total de la partie des vertices, je pense
		//le mesh est parsé là: 0x6b2432

		for (unsigned int idx = 0; idx < nb_blocks; idx++) {
			vertex v = read_data<vertex>(content_vertices, addr_vpax);
			vertices.push_back(v);
		}

		addr_vpax = 0;
		size_t nb_indexes = content_indexes.size()/sizeof(uint16_t);

		for (unsigned int idx = 0; idx < nb_indexes; idx++) {
			uint16_t i = read_data<uint16_t>(content_indexes, addr_vpax);
			indexes.push_back(i);
		}

		/*std::vector<uint8_t> el = std::vector<uint8_t>(first_part_sz);
		first_part.insert(first_part.end(), content.begin() + addr_vpax, content.begin() + addr_vpax + first_part_sz);
		addr_vpax = addr_vpax + first_part_sz;*/


	}
	else {
		throw std::exception("Not sure if it happens but needs to be investigated if it does");
	}

	

}


void VPAX::output_data() {
	std::ofstream OutFile;
	std::cout << "Vertex size: " << std::hex << block_size << " Nb blocks: " << nb_blocks << std::endl;
	OutFile.open(name + "_vertices", std::ios::out | std::ios::binary);
	OutFile.write((char*)this->content_vertices.data(), this->content_vertices.size() * sizeof(char));
	OutFile.close();
	OutFile.open(name + "_indexes", std::ios::out | std::ios::binary);
	OutFile.write((char*)this->content_indexes.data(), this->content_indexes.size() * sizeof(char));
	OutFile.close();
	FBXExporter exporter;
	exporter.GenerateScene(*this);
}
