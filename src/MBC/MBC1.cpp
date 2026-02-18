#include "MBC/MBC1.h"
#include "Memory/Cart.h"

void MBC1::write(uint16_t addr, uint8_t val) {
    if(addr <= 0x1FFF) {
        if((val & 0xF) == 0xA) {
            cart.enable_ext_ram();
        } else {
            cart.disable_ext_ram();
        }
    } else if(addr <= 0x3FFF) {
        rom_select_lo = val & 0x1F;
        if(rom_select_lo == 0) {
            rom_select_lo = 1;
        } 
        cart.swap_rom_bank((rom_select_hi << 5) | rom_select_lo);
    } else if(addr <= 0x5FFF) {
        if(select_mode == SelectMode::RAM) {
            cart.swap_ram_bank(val & 0x3);
        } else {
            rom_select_hi = val & 0x3;
            cart.swap_rom_bank((rom_select_hi << 5) | rom_select_lo);
        }
    } else if(addr <= 0x7FFF) {
        if(val <= 1) {
            select_mode = static_cast<SelectMode>(val);
        }
    } else {
        return;
    }
}