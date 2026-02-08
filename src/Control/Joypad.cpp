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
     region{this, ADDRESS, ADDRESS},
     ic{int_controller} 
     {
        mmu.add_region(&region);
        bus.connect(*this);
     }
    

void JoyPad::read_input() {   
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
   
   int mode = buttons_enabled()? 1 : dpad_enabled()? 0 : -1;
   uint8_t old_data = data;

   ih->get_key_state();

   if(mode >= 0) {
      //can read keys
      for(const auto& mapping : key_bit_map[mode]) {
         if(ih->key_pressed(mapping.key)) {
            //bit clear because active low
            data = Arithmetic::bit_clear(data, mapping.bit);
            if(Arithmetic::bit_check(old_data, mapping.bit)) {
               //falling edge
               //std::cout << "BUTTON PRESSED: " << print_button(mapping.key) << '\n';
               ic.request(Interrupt::JOYPAD);
            }
         } else {
            data = Arithmetic::bit_set(data, mapping.bit);
         }
      }
   }

}

uint8_t JoyPad::ext_read(uint16_t addr) {
   return data | 0xC0;
}

void JoyPad::ext_write(uint16_t addr, uint8_t val) {
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