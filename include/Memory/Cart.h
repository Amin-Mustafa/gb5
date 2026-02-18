#ifndef CART_H
#define CART_H

#include <vector>
#include <cstdint>
#include <string>
#include <memory>
#include "MBC/MBC.h"
#include "MMU.h"

class MMU;

enum class CartType : uint8_t;

class Cart {
private:
    MMU& mmu;
    
    size_t num_rom_banks = 2;   //two rom banks
    size_t num_ram_banks = 0;   //no ext ram by default

    bool ram_enable = false;
    bool ram_exists = false;
    bool battery_exists = false;

    uint8_t* rom_bank1 = nullptr;
    uint8_t* rom_bank2 = nullptr;
    uint8_t* ram_bank  = nullptr;
    std::vector<uint8_t> rom_container;    //whole cart data
    std::vector<uint8_t> ram_container;     //external cart ram if any

    std::unique_ptr<MBC> mbc = nullptr;
public:
    Cart(MMU& memory);
    void load(const std::string& filename); //load cartridge
    void init_hardware(CartType type);
    void swap_rom_bank(uint8_t bank_number) {
        size_t max_bank = num_rom_banks - 1;    
        rom_bank2 = rom_bank1 + (bank_number & max_bank) * 0x4000;
		mmu.map_region(Space::ROM_BANK2_START, Space::ROM_BANK2_END, rom_bank2);
    }
    void swap_ram_bank(uint8_t bank_number) {
        size_t max_bank = num_ram_banks - 1;
        ram_bank = ram_container.data() + (bank_number & max_bank) * 0x2000;
    }

    void enable_ext_ram();
    void disable_ext_ram();
};

enum class CartType : uint8_t {
    ROM_ONLY = 0x00,
    MBC1 = 0x01, 
    MBC1_RAM = 0x02, 
    MBC1_RAM_BAT = 0x03,

    //----unimplemented----//
    MBC2 = 0x05, 
    MBC2_BAT = 0x06,
    ROM_RAM = 0x08, 
    ROM_RAM_BAT = 0x09, //unused in licensed carts
    MMM01 = 0x0B, 
    MMM01_RAM = 0x0C, 
    MMM01_RAM_BAT = 0x0D,
    MBC3_TIMER_BAT = 0x0F, 
    MBC3_TIMER_RAM_BAT = 0x10,
    MBC3 = 0x11, 
    MBC3_RAM = 0x12, 
    MBC3_RAM_BAT = 0x13,
    MBC5 = 0x19, 
    MBC5_RAM = 0x1A, 
    MBC5_RAM_BAT = 0x1B,
    MBC5_RUMBLE = 0x1C, 
    MBC5_RUMBLE_RAM = 0x1D, 
    MBC5_RUMBLE_RAM_BAT = 0x1E,
    MBC6 = 0x20, 
    MBC7_SENSOR_RUMBLE_RAM_BAT = 0x22,
    POCKET_CAMERA = 0xFC,
    BANDAI_TAMA5 = 0xFD,
    HuC3 = 0xFE, 
    HuC1_RAM_BAT = 0xFF,
};

#endif