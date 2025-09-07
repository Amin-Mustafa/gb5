#include "../../include/Graphics/PPU.h"
#include "../../include/Memory/MMU.h"
#include "../../include/Memory/InterruptController.h"


PPU::PPU(MMU& mmu, InterruptController& interrupt_controller)
    :vram{mmu},
     oam{mmu},
     regs{mmu},
     ic{interrupt_controller}
     {}
