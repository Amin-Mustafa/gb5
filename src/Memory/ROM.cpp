#include <fstream>
#include <iostream>
#include "../../include/Memory/ROM.h"
#include "../../include/Memory/MMU.h"
#include "../../include/Memory/MemoryRegion.h"

ROM::ROM(MMU& mmu)
	:data(0x8000),
	 region{this, START, END} 
	 {
		mmu.add_region(&region);
	 }

void ROM::load(const std::string& filename){
	std::ifstream ifs(filename, std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(ifs), {});
	std::cout << buffer.size() << '\n';
	std::copy(buffer.begin(), buffer.end(), data.begin());
}