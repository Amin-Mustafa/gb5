#ifndef MEMORYREGION_H
#define MEMORYREGION_H

#include <functional>
#include <cstdint>
#include <cassert>

class MMU;

class MemoryRegion {
private:
    uint16_t start_addr;
    uint16_t end_addr;
    void* ctx;

    using ReadFunc = uint8_t(*)(void*, uint16_t);
    using WriteFunc = void(*)(void*, uint16_t, uint8_t);

    ReadFunc read_fn;
    WriteFunc write_fn;

    template<typename Device>
    static uint8_t device_read(void* ctx, uint16_t addr) {
        return static_cast<Device*>(ctx)->ext_read(addr);
    }

    template<typename Device>
    static void device_write(void* ctx, uint16_t addr, uint8_t val) {
        static_cast<Device*>(ctx)->ext_write(addr, val);
    }

public:
    uint16_t start() const {return start_addr;}
    uint16_t end() const {return end_addr;}

    uint8_t read(uint16_t addr) {
        assert(read_fn && ctx);
        return read_fn(ctx, addr);
    }
    void write(uint16_t addr, uint8_t val) {
        assert(read_fn && ctx);
        write_fn(ctx, addr, val);
    }

    template<typename Device>
    MemoryRegion(Device* device, uint16_t start, uint16_t end) 
        :start_addr{start},
         end_addr{end},
         ctx{device},
         read_fn{&device_read<Device>},
         write_fn{&device_write<Device>}
        {}

};

#endif