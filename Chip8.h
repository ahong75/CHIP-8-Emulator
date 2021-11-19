#pragma once 
#include <cstdint>

class Chip8 {
    public:
        Chip8();
    private:
        uint8_t registers[16] {};
        uint16_t program_counter {};
        uint16_t index_register {};
        uint8_t stack_pointer {};
        uint8_t delay_timer {};
        uint8_t sound_timer {};
        uint16_t stack[16] {};
        uint16_t opcode;
        uint32_t screen[64 * 32] {};

        void fetch();

        void increment_pc();

        void load_rom(char const* file);

}