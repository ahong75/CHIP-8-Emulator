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

Chip8::randGen(std::chrono::system_clock::now().time_since_epoch().count())
