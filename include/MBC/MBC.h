#ifndef MBC_H
#define MBC_H

#include <cstdint>
#include <array>

class Cart;

class MBC {
protected: 
    Cart& cart; 
    MBC(Cart& cartridge) 
        :cart{cartridge} {}
public:
    virtual ~MBC()=default;
    virtual void write(uint16_t addr, uint8_t val) = 0;
};

#endif