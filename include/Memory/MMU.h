#ifndef MMU_H
#define MMU_H

#include <vector>
#include <cstdint>
#include <memory>
#include <array>

class MemoryRegion;
class DmaController;

class MMU {
private:   
    std::vector<MemoryRegion*> memory_lookup;
    MemoryRegion* region_of(uint16_t addr);

    std::unique_ptr<DmaController> dmac;
public:
    MMU();
    ~MMU();
    void add_region(MemoryRegion* region);
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    
    //dma stuff
    void start_dma(uint8_t page);
    void tick();    //MMU clocked in M-cycles
    uint8_t dma_read(uint16_t addr);
    void dma_write(uint16_t addr, uint8_t val);

    bool dma_active() const;
};

bool addr_in_hram(uint16_t addr);
bool addr_in_vram(uint16_t addr);

#endif