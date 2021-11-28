#pragma once 
#include <cstdint>
#include <chrono>
#include <random>

const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int KEY_COUNT = 16;
class Chip8 {
    public:
        Chip8();

        void Cycle();

        void load_rom(char const* file);

        uint8_t keypad[KEY_COUNT] {};

        uint32_t screen[VIDEO_WIDTH * VIDEO_HEIGHT] {};
    private:
        uint8_t registers[16] {};
        uint16_t program_counter {};
        uint16_t index_register {};
        uint8_t stack_pointer {};
        uint8_t delay_timer {};
        uint8_t sound_timer {};
        uint16_t stack[16] {};
        uint16_t opcode;

        std::default_random_engine randGen;
        std::uniform_int_distribution<uint8_t> randByte;

        typedef void (Chip8::*Chip8Func)();
        Chip8Func table[0xF + 1]{&Chip8::OP_NULL};
        Chip8Func table0[0xE + 1]{&Chip8::OP_NULL};
        Chip8Func table8[0xE + 1]{&Chip8::OP_NULL};
        Chip8Func tableE[0xE + 1]{&Chip8::OP_NULL};
        Chip8Func tableF[0x65 + 1]{&Chip8::OP_NULL};

        void fetch();

        void increment_pc();

        void randGen(std::chrono::system_clock::now().time_since_epoch().count());

        void OP_NULL();
        
        void OP_00E0();

        void OP_00EE();

        void OP_1nnn();

        void OP_2nnn();

        void OP_3xkk();

        void OP_4xkk();

        void OP_5xy0();

        void OP_6xkk();

        void OP_7xkk();

        void OP_8xy0();

        void OP_8xy1();

        void OP_8xy2();

        void OP_8xy3();

        void OP_8xy4();

        void OP_8xy5();

        void OP_8xy6();

        void OP_8xy7();

        void OP_8xyE();

        void OP_9xy0();

        void OP_Annn();

        void OP_Bnnn();

        void OP_Cxkk();

        void OP_Dxyn();

        void OP_Ex9E();

        void OP_ExA1();

        void OP_Fx07();

        void OP_Fx0A();

        void OP_Fx15();

        void OP_Fx18();

        void OP_Fx1E();

        void OP_Fx29();

        void OP_Fx33();

        void OP_Fx55();

        void OP_Fx65();

        
}