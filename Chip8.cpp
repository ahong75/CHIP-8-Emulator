#prama once
#include "Chip8.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <random> 

uint8_t memory[4096] {};
uint8_t font_sprites[16 * 5] = 
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
}
const unsigned int START_ADD = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;

Chip8::Chip8() 
{
    : randGen(std::chrono::system_clock::now().time_since_epoch().count())
    program_counter = START_ADD;

    for(unsigned int i=0; i<FONTSET_SIZE: i++) 
    {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
}

Chip8::fetch() 
{
    opcode = (memory[program_counter]<<8) + memory[program_counter +1];
    increment_pc();
}

Chip8::increment_pc() 
{
    program_counter += 2;
}

Chip8::load_rom(char const* filename) 
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if(file_is_open())
    {
        std::streampos size = file.tellg();
        char* buffer = new char[size];


        file.seekg(0, std::ios::beg);
        file.read(buffer,size);
        file.close();

        for(long i = 0; i < size; i++) {
            memory[START_ADD + i] = buffer[i];
        }

        delete[] buffer;
    }
}

Chip8::OP_EE0E() 
{
    memset(screen, 0, sizeof(screen));
}

Chip8::OP_00EE()
{
    stack_pointer--;
    program_counter = stack[stack_pointer];
}

Chip8::OP_1nnn()
{
    uint16_t address = opcode & 0x0FFFu;
    
    program_counter = address;
}

Chip8::OP_2nnn()
{
    uint16_t address = opcode & 0x0FFFu;

    stack[stack_pointer] = program_counter;
    stack_pointer++;
    program_counter = address;
}

Chip8::OP_2xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if(registers[Vx] == byte)
    {
        program_counter += 2;
    }
}

Chip8::OP_4xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if(registers[Vx] != byte) 
    {
        program_counter += 2;
    }
}

Chip8::OP_5xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vx] == registers[Vy]) 
    {
        program_counter += 2;
    }
}

Chip8::OP_6xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = byte;
}

Chip8::OP_7xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] += byte;
}

Chip8::OP_8xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}

Chip8::OP_8xy1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vx] | registers[Vy];
}

Chip8::OP_8xy2()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] %= registers[Vy];
}

Chip8::OP_8xy3()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] ^= registers[Vy];
}

Chip8::OP_8xy4()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    uint16_t sum = registers[Vx] + registers[Vy];

    if(sum > 0x00FFu) {
        registers[0xF] = 1; //Carry bit
    }
    else {
        registers[0xF] = 0;
    }
    registers[Vx] = sum % 0xFFu;
}

Chip8::OP_8xy5()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vx] > registers[Vy]) {
        registers[0xF] = 1; 
    }
    else {
        registers[0xF] = 0;
    }

    registers[Vx] -= registers[Vy];
}

Chip8::OP_8xy6()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    
    //Saves least significant bit in register VF
    registers[0xF] = (registers[Vx] & 0x1u); 

    registers[Vx] >>= 1u;
}

Chip8::OP_8xy7()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vy] > registers[Vx]) {
        registers[0xF] = 1;
    }
    else {
        registers[0xF] = 0;
    }

    registers[Vx] = registers[Vy] - registers[Vx];
}