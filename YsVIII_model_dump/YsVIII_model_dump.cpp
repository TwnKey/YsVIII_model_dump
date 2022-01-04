// YsVIII_model_dump.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <fstream>
#include <vector>
#include "IT3File.h"
#include <string>
#include "FBXExporter.h"
#include "MTBFile.h"
int main(int argc, char ** argv)
{

	if (argc == 2) {
		std::string filepath = std::string(argv[1]);
		std::ifstream input(filepath, std::ios::binary);
		std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
		IT3File it3(buffer);
		std::cout << it3.to_string() << std::endl;
		it3.output_data();
	}
	if (argc == 3) {
		std::string filepath = std::string(argv[1]);
		std::string m_filepath = std::string(argv[2]);

		std::ifstream input(filepath, std::ios::binary);
		std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
		IT3File it3(buffer);
		std::cout << it3.to_string() << std::endl;
		

		std::ifstream input_m(m_filepath, std::ios::binary);
		std::vector<unsigned char> buffer2(std::istreambuf_iterator<char>(input_m), {});
		IT3File it3_m(buffer2);
		it3.add_kan7_from_m_file(it3_m);
		it3.output_data();
	}
	if (argc == 4) {
		std::string filepath = std::string(argv[1]);
		std::string m_filepath = std::string(argv[2]);
		std::string mtb_filepath = std::string(argv[3]);

		std::ifstream input_mtb(mtb_filepath, std::ios::binary);
		std::vector<unsigned char> buffer3(std::istreambuf_iterator<char>(input_mtb), {});
		MTBFile mtb_f(buffer3);
		exit(0);

		std::ifstream input(filepath, std::ios::binary);
		std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
		IT3File it3(buffer);
		std::cout << it3.to_string() << std::endl;


		std::ifstream input_m(m_filepath, std::ios::binary);
		std::vector<unsigned char> buffer2(std::istreambuf_iterator<char>(input_m), {});
		IT3File it3_m(buffer2);
		it3.add_kan7_from_m_file(it3_m);
		it3.output_data();
	}
	
	



}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
