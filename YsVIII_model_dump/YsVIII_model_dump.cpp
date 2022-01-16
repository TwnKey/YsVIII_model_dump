// YsVIII_model_dump.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <fstream>
#include <vector>
#include "IT3File.h"
#include <string>
#include "DAEExporter.h"
#include "MTBFile.h"
#include <filesystem>




int main(int argc, char ** argv)
{
	if (argc > 1){
		std::string filepath = std::string(argv[1]);
		std::string base_filename = filepath.substr(filepath.find_last_of("/\\") + 1);
		std::string::size_type const p(base_filename.find_last_of('.'));
		std::string scene_name = base_filename.substr(0, p);

		std::filesystem::create_directory(std::filesystem::path(scene_name));


		if (argc == 2) {
		
			std::ifstream input(filepath, std::ios::binary);
			std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
			IT3File it3(buffer);
			std::cout << it3.to_string() << std::endl;
			it3.output_data(scene_name);
			Scene sc(scene_name, it3, IT3File(), MTBFile());

			DAEExporter dae_exp;
			dae_exp.ExportScene(sc);

		}
	
		if (argc == 3) {
		
			std::string mtb_filepath = std::string(argv[2]);

			std::ifstream input_mtb(mtb_filepath, std::ios::binary);
			std::vector<unsigned char> buffer3(std::istreambuf_iterator<char>(input_mtb), {});
			MTBFile mtb_f(buffer3);


			std::ifstream input(filepath, std::ios::binary);
			std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
			IT3File it3(buffer);
			//std::cout << it3.to_string() << std::endl;
			it3.output_data(scene_name);
			Scene sc(scene_name, it3, IT3File(), mtb_f);
		
			DAEExporter dae_exp;
			dae_exp.ExportScene(sc);
		}


		if (argc == 4) {
		
			std::string m_filepath = std::string(argv[2]);
			std::string mtb_filepath = std::string(argv[3]);

			std::ifstream input_mtb(mtb_filepath, std::ios::binary);
			std::vector<unsigned char> buffer3(std::istreambuf_iterator<char>(input_mtb), {});
			MTBFile mtb_f(buffer3);
		

			std::ifstream input(filepath, std::ios::binary);
			std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
			IT3File it3(buffer);
			//std::cout << it3.to_string() << std::endl;


			std::ifstream input_m(m_filepath, std::ios::binary);
			std::vector<unsigned char> buffer2(std::istreambuf_iterator<char>(input_m), {});
			IT3File it3_m(buffer2);
			it3.add_kan7_from_m_file(it3_m);
			it3.output_data(scene_name);
			Scene sc(scene_name, it3, it3_m, mtb_f);
		
			DAEExporter dae_exp;
			dae_exp.ExportScene(sc);
		}
	}
	



}
