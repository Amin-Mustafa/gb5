#include <fstream>
#include <iostream>
#include "Memory/Cart.h"
#include "Memory/MMU.h"
#include "Memory/Spaces.h"
#include "MBC/MBC1.h"


Cart::Cart(MMU& memory)
	:mmu{memory}, rom_container(0x8000)	//rom 32kB by default
	 {
		rom_bank1 = rom_container.data();
		rom_bank2 = rom_bank1 + 0x4000;
		mmu.map_region(Space::ROM_BANK1_START, Space::ROM_BANK1_END, rom_bank1);
		mmu.map_region(Space::ROM_BANK2_START, Space::ROM_BANK2_END, rom_bank2);

		//ram empty by default
		ram_bank  = nullptr;
	 }

void Cart::load(const std::string& filename){
	//ram sizes in number of banks
	static constexpr size_t ram_sizes[] = {0, 1, 1, 4, 16, 8};

	//read file into buffer
	std::ifstream ifs(filename, std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(ifs), {});
	std::cout << buffer.size() << '\n';

	init_hardware(static_cast<CartType>(buffer[0x147]));
	if(mbc) {
		mmu.connect_MBC(mbc.get());
	}

	//get rom size from header
	num_rom_banks = 2 << buffer[0x148];
	size_t rom_size = 0x4000 * (num_rom_banks);
	rom_container.resize(rom_size);
	rom_bank1 = rom_container.data();
	rom_bank2 = rom_bank1 + 0x4000;
	mmu.map_region(Space::ROM_BANK1_START, Space::ROM_BANK1_END, rom_bank1);
	mmu.map_region(Space::ROM_BANK2_START, Space::ROM_BANK2_END, rom_bank2);

	if(ram_exists) {
		num_ram_banks = ram_sizes[buffer[0x149]];
		size_t ram_size = 0x2000 * num_ram_banks;
		if(ram_size) {
			ram_container.resize(ram_size);
			ram_bank = ram_container.data();
		}
	}

	std::copy(buffer.begin(), buffer.end(), rom_container.begin());
}

void Cart::enable_ext_ram() {
	if(!ram_bank) {
		//ignore attempt to enable if no ext ram in cart
		return;
	}
	if(!ram_enable) {
		ram_enable = true;
		mmu.map_region(Space::EXTRAM_START, Space::EXTRAM_END, ram_bank);
	}
}

void Cart::disable_ext_ram() {
	if(ram_enable) {
		ram_enable = false;
		mmu.unmap_region(Space::EXTRAM_START, Space::EXTRAM_END);
	}
}

void Cart::init_hardware(CartType type) {
	//cases intentionally fall through
	switch(type) {
		case CartType::ROM_RAM_BAT:
			battery_exists = true; 
		case CartType::ROM_RAM:
			ram_exists = true; 
		case CartType::ROM_ONLY:
			break;

		case CartType::MBC1_RAM_BAT:
            battery_exists = true; 
        case CartType::MBC1_RAM:
            ram_exists = true; 
        case CartType::MBC1:
            mbc = std::make_unique<MBC1>(*this);
            break;

		default: break;
	}
}