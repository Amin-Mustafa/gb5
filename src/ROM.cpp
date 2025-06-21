#include "../include/ROM.h"
#include <fstream>
#include <vector>
#include <iostream>

namespace SM83{

void ROM::load(const std::string& filename){
	std::ifstream ifs(filename, std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(ifs), {});
	std::cout << buffer.size() << '\n';
	contents = std::move(buffer);
}

}