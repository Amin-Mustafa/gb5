#include "Control/JoyPad.h"
#include "Memory/MMU.h" 
#include "Memory/Bus.h"
#include "Memory/InterruptController.h"
#include "Control/InputHandler.h"  
#include "Arithmetic.h"
#include <iostream>

constexpr uint8_t RIGHT_BIT = 0;
constexpr uint8_t LEFT_BIT = 1;
constexpr uint8_t UP_BIT = 2;
constexpr uint8_t DOWN_BIT = 3;
constexpr uint8_t A_BIT = 0;
constexpr uint8_t B_BIT = 1;
constexpr uint8_t SELECT_BIT = 2;
constexpr uint8_t START_BIT = 3;

std::string print_button(InputHandler::Mapping key);

JoyPad::JoyPad(Bus& bus, MMU& mmu, InterruptController& int_controller)
    :data{0xCF},
     ic{int_controller},
     dpad_state{0x0F}, 
     button_state{0x0F}
     {
        mmu.map_io_register(ADDRESS, this);
        bus.connect(*this);
     }
    

void JoyPad::read_input() {   
   if(!ih) return;

   uint8_t old_output = read(ADDRESS);
   //reset input state
   dpad_state = 0x0F;
   button_state = 0x0F;

   static const struct {
      InputHandler::Mapping key;
      uint8_t bit;
   } key_bit_map[2][4] = {
      { 
         //dpad
         {InputHandler::Mapping::RIGHT, RIGHT_BIT},
         {InputHandler::Mapping::LEFT , LEFT_BIT},
         {InputHandler::Mapping::DOWN , DOWN_BIT},
         {InputHandler::Mapping::UP   , UP_BIT},
      },
      {
         //buttons
         {InputHandler::Mapping::A     , A_BIT},
         {InputHandler::Mapping::B     , B_BIT},
         {InputHandler::Mapping::START , START_BIT},
         {InputHandler::Mapping::SELECT, SELECT_BIT},
      }
   };
   
   ih->get_key_state();

   for (int m = 0; m < 2; m++) {
       for(const auto& mapping : key_bit_map[m]) {
          if(ih->key_pressed(mapping.key)) {
             if(m == 0) {
                dpad_state = Arithmetic::bit_clear(dpad_state, mapping.bit);
             } else {
                button_state = Arithmetic::bit_clear(button_state, mapping.bit);
             }
          }
       }
    }

   uint8_t new_output = read(ADDRESS);

   if ((old_output & ~new_output) & 0x0F) {
      //falling edge
      //std::cout << "BUTTON PRESSED: " << print_button(mapping.key) << '\n';
      ic.request(Interrupt::JOYPAD);
   }
}

uint8_t JoyPad::read(uint16_t addr) {
   uint8_t output = data | 0xCF;
   
   if (dpad_enabled()) {
        output &= dpad_state;
    }
    if (buttons_enabled()) {
        output &= button_state;
    }

   return output;
}

void JoyPad::write(uint16_t addr, uint8_t val) {
   //lower nibble is read-only
   data = (val & 0x30) | (data & 0xCF);
}

std::string print_button(InputHandler::Mapping key) {
   std::string name;
   switch(key) {
      case InputHandler::Mapping::LEFT:
         name = "LEFT";
         break;
      case InputHandler::Mapping::RIGHT:
         name = "RIGHT";
         break;
      case InputHandler::Mapping::UP:
         name = "UP";
         break;
      case InputHandler::Mapping::DOWN:
         name = "DOWN";
         break;
      case InputHandler::Mapping::A:
         name = "A";
         break;
      case InputHandler::Mapping::B:
         name = "B";
         break;
      case InputHandler::Mapping::START:
         name = "START";
         break;
      case InputHandler::Mapping::SELECT:
         name = "SELECT";
         break;
      default: break;
   }
   return name;
}