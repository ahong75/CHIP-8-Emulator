#prama once
#include "Chip8.h"
#include <fstream>
#include <iostream>
#include <cstdint>
#include <chrono>
#include <random> 

/*
Keypad       Keyboard
+-+-+-+-+    +-+-+-+-+
|1|2|3|C|    |1|2|3|4|
+-+-+-+-+    +-+-+-+-+
|4|5|6|D|    |Q|W|E|R|
+-+-+-+-+ => +-+-+-+-+
|7|8|9|E|    |A|S|D|F|
+-+-+-+-+    +-+-+-+-+
|A|0|B|F|    |Z|X|C|V|
+-+-+-+-+    +-+-+-+-+
*/

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

void Chip8::fetch() 
{
    opcode = (memory[program_counter]<<8) + memory[program_counter +1];
    increment_pc();
}

void Chip8::increment_pc() 
{
    program_counter += 2;
}

void Chip8::load_rom(char const* filename) 
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

void Chip8::OP_EE0E() 
{
    memset(screen, 0, sizeof(screen));
}

void Chip8::OP_00EE()
{
    stack_pointer--;
    program_counter = stack[stack_pointer];
}

void Chip8::OP_1nnn()
{
    uint16_t address = opcode & 0x0FFFu;
    
    program_counter = address;
}

void Chip8::OP_2nnn()
{
    uint16_t address = opcode & 0x0FFFu;

    stack[stack_pointer] = program_counter;
    stack_pointer++;
    program_counter = address;
}

void Chip8::OP_2xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if(registers[Vx] == byte)
    {
        program_counter += 2;
    }
}

void Chip8::OP_4xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if(registers[Vx] != byte) 
    {
        program_counter += 2;
    }
}

void Chip8::OP_5xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vx] == registers[Vy]) 
    {
        program_counter += 2;
    }
}

void Chip8::OP_6xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = byte;
}

void Chip8::OP_7xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] += byte;
}

void Chip8::OP_8xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vx] | registers[Vy];
}

void Chip8::OP_8xy2()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] %= registers[Vy];
}

void Chip8::OP_8xy3()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4()
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

void Chip8::OP_8xy5()
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

void Chip8::OP_8xy6()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    
    //Saves least significant bit in register VF
    registers[0xF] = (registers[Vx] & 0x1u); 

    registers[Vx] >>= 1u;
}

void Chip8::OP_8xy7()
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

void Chip8::OP_8xyE()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[0xF] = (registers[Vx] & 0x80u) >> 7u; //0x80u is 128 (10000000);

    registers[Vx] <<= 2;
}

void Chip8::OP_9xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vx] != registers[Vy]) {
        program_counter += 2;
    }
}

void Chip8::OP_Annn() 
{
    uint16_t NNN = opcode & 0x0FFFu;

    index_register = NNN;
}

void Chip8::OP_Bnnn()
{
    uint16_t NNN = opcode & 0x0FFFu;

    program_counter = NNN + registers[0];
}

void Chip8::OP_Cxkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = randByte(randGen) & byte;
}

void Chip8::OP_Dxyn()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    uint8_t xPos = registers[Vx] % VIDEO_HEIGHT;
    uint8_t yPos = registers[Vy] % VIDEO_WIDTH;

    for(int row = 0; row < height; row++) {
        uint8_t spriteByte = memory[index_register + row];
        for(int col = 0; col < 8; col++) {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t* screenPixel = &screen[(yPos + row) * VIDEO_WIDTH + (xPos + col)]; 
            
            if(spritePixel) {
                if(*screenPixel == 0xFFFFFFFF);
                registers[0xF] = 1; 
            }

            *screenPixel ^= 0xFFFFFFFF;
        }
    }
}

void Chip8::OP_Ex9E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[Vx];

    if(keypad[key])
    {
        program_counter += 2;
    }
}

void Chip8::OP_ExA1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    uint8_t key = registers[Vx];

    if(!keypad[key])
    {
        program_counter += 2;
    }
}

void Chip8::OP_Fx07()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[Vx] = delay_timer;
}

void Chip8::OP_Fx0A()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    switch(true) {
        case keypad[0]:
            registers[Vx] = 0;
            break;
        case keypad[1]:
            registers[Vx] = 1;
            break;
        case keypad[2]:
            registers[Vx] = 2;
            break;
        case keypad[3]:
            registers[Vx] = 3;
            break;
        case keypad[4]:
            registers[Vx] = 4;
            break;
        case keypad[5]:
            registers[Vx] = 5;
            break;
        case keypad[6]:
            registers[Vx] = 6;
            break;
        case keypad[7]:
            registers[Vx] = 7;
            break;
        case keypad[8]:
            registers[Vx] = 8;
            break;
        case keypad[9]:
            registers[Vx] = 9;
            break;
        case keypad[10]:
            registers[Vx] = 10;
            break;
        case keypad[11]:
            registers[Vx] = 11;
            break;
        case keypad[12]:
            registers[Vx] = 12;
            break;
        case keypad[13]:
            registers[Vx] = 13;
            break;
        case keypad[14]:
            registers[Vx] = 14;
            break;
        case keypad[15]:
            registers[Vx] = 15;
            break;
        default:
            program_counter -= 2;
    }
}

void Chip8::OP_Fx15()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    delay_timer = registers[Vx];
}

void Chip8::OP_Fx18()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    sound_timer = registers[Vx];
} 

void Chip8::OP_Fx1E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    index_register += registers[Vx];
}

void Chip8::OP_Fx29()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[Vx];

    index_register = FONTSET_START_ADDRESS + (5 * digit); //font character is 5 bytes
}

void Chip8::OP_Fx33()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t value = registers[Vx];

    memory[index_register] = value % 10; //ones place
    value /= 10;

    memory[index_register + 1] = value % 10; //tens place
    value /= 10;

    memory[index_register + 2] = value % 10; // hundreds place
}

void Chip8::OP_Fx55()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for(uint8_t i = 0; i<= Vx; i++) {
        memory[index_register + i] = registers[i];
    }
}

void Chip8::OP_Fx65()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for(uint8_t i = 0; i <= Vx; i++) {
        registers[i] = memory[index_register + i];
    }
}
