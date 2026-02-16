#ifndef ROM_H
#define ROM_H

#include <array>
#include <cstdint>
#include <string>

class MMU;

class ROM {
private:
    std::array<uint8_t, 0x8000> container;
public:
    ROM(MMU& memory);
    void load(const std::string& filename); //load cartridge
};


#endif