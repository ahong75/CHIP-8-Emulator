#pragma once 
#include <cstdint>
#include <chrono>
#include <random>

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
        std::default_random_engine randGen;
        std::uniform_int_distribution<uint8_t> randByte;

        void fetch();

        void increment_pc();

        void load_rom(char const* file);

        void randGen(std::chrono::system_clock::now().time_since_epoch().count());

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