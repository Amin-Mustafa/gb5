#include "../../include/Graphics/OAM.h"
#include "../../include/Memory/MMU.h"

OAM::OAM(MMU& mmu) 
    :region{this, START, END}
    {
        mmu.add_region(&region);
        //test sprites
        /* write(START + 0x00, 16);
        write(START + 0x01, 3);
        write(START + 0x02, 1); 

        write(START + 0x10, 16);
        write(START + 0x11, 24);
        write(START + 0x12, 1);

        write(START + 0x20, 16);
        write(START + 0x21, 5);
        write(START + 0x22, 2); */
    }