#include <fstream>
#include <iostream>
#include "../include/ROM.h"
#include "../include/MMU.h"
#include "../include/MemoryRegion.h"

ROM::ROM(MMU& mmu)
	:data(0x8000),
	 region{
		mmu,
		0x0000, 0x7FFF,
		[this](uint16_t addr) {return data[addr];},
		[](uint16_t, uint8_t){}
	 } {}

void ROM::load(const std::string& filename){
	std::ifstream ifs(filename, std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(ifs), {});
	std::cout << buffer.size() << '\n';
	std::copy(buffer.begin(), buffer.end(), data.begin());
}