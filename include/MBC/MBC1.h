#ifndef MBC1_H
#define MBC1_H

#include "MBC.h"

class MBC1 : public MBC {
private:
    uint8_t rom_select_lo : 5;  
    uint8_t rom_select_hi : 2; 
    enum class SelectMode {RAM, ROM_UPPER} select_mode;
public:
    MBC1(Cart& cartridge) 
        :MBC{cartridge} 
        {
        }

    void write(uint16_t addr, uint8_t val) override;
};


#endif