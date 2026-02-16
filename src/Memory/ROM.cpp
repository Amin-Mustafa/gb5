#include <fstream>
#include <iostream>
#include "../../include/Memory/ROM.h"
#include "../../include/Memory/MMU.h"
#include "Memory/Spaces.h"

ROM::ROM(MMU& mmu)
	 {
		mmu.map_region(Space::ROM::START, Space::ROM::END, container.data());
	 }

void ROM::load(const std::string& filename){
	std::ifstream ifs(filename, std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(ifs), {});
	std::cout << buffer.size() << '\n';
	std::copy(buffer.begin(), buffer.end(), container.begin());
}