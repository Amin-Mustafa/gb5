#include <fstream>
#include <iostream>
#include "../include/ROM.h"
#include "../include/MMU.h"

ROM::ROM(MMU& mmu)
	:data(0x8000), 
	region {
		mmu,
		0x0000, 0x7FFF,
		[&mem = data](uint16_t addr)->uint8_t { return mem[addr]; },
		[](uint16_t, uint8_t) {}
	} {}

void ROM::load(const std::string& filename){
	std::ifstream ifs(filename, std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(ifs), {});
	std::cout << buffer.size() << '\n';
	std::copy(buffer.begin(), buffer.end(), data.begin());
}