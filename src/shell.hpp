#include <stack>
#include <iostream>
#include <string>
#include <sstream>
#include <bitset>
#include <array>
#include "font.h"
//takes an 8 bit integer and outputs an array corresponding to its binary string
//with the most significant bit stored first.
std::array<bool, 8> extractBits(uint8_t value) {
    std::array<bool, 8> output;
    for (int i = 7; i >= 0; --i) {
        output[7-i] = (value >> i) & 1;
    }
    return output;

};
/*
extracts integers from commands, from a command as an array, given the number of integers to extract.
e.g "dxyn" as the first argument and arg_nums = 3 returns x y and n as an integer array.
e.g. "4xnn" would return and arg_nums = 2 would return x and nn as two integers in an array.
*/
std::array<int, 3> extract_integers(std::string instruction, int arg_nums){
    std::array<int,3> integers;
    switch (arg_nums){
    case 3:
        for (int i=1; i<4; i++){
        std::string x =  instruction.substr(i,1);
        int x_int = std::stoi(x, nullptr, 16);
        integers[i-1] = x_int;
        };
        break;
    case 2:
    for (int i=1; i<4; i++){    
    std::string x = instruction.substr(i,i);
    int x_int = std::stoi(x, nullptr, 16);
    integers[i-1] = x_int;
    };
    break;
    case 1:
    std::string x = instruction.substr(1,3);
    int x_int = std::stoi(x, nullptr, 16);
    integers[0] = x_int;
    break; 

};
return integers;
}
class shell{
    public:
//The 16 8-bit registers are stored in an array.
    std::array<uint8_t, 16> vRegisters = {0};
//The I register (misspelled)
    int iRegsiter = 0;
    std::array<uint8_t, 10000> ram;
 // not used for implementation yet   
    unsigned char delay = 0;
    unsigned char sound = 0;
//array controlling the 64 by 32 display, with 1 corresponding to a pixel being on and 0 being off.
    std::array<std::array<bool, 32>, 64> screen = {0};
// stack currently not implemented    
    std::stack<int> stack;
//program counter initially pointing to 0x200, where the program  start is loaded in to CHIP-8 memory.
    uint8_t * pc = ram.data()+0x200;

    uint8_t * mem_ptr = ram.data();
 
 /******constructor

********/
    shell(std::string rom){
        load_font();
        load_screen();
        load_rom(rom);
    };
 /************constructor functions   
  * 
****************/
//load font into the CHIP-8 memory, starting at addr 0x50
    void load_font() {
    for (int i = 0; i < 80; i++) {
        ram[i] = font[i+0x50];
    };
    }
// sets screen to be completely dark on loading.
    void load_screen(){
        for (auto& row : screen) {
        row.fill(false);
    }
    };
//puts rom instructions into memory starting at 0x200
    void load_rom(std::string fname){
    std::ifstream file(fname, std::ios::binary);
    if (!file) {
        throw "file not found";
    }

    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    uint8_t byte;
    for(int byte_pos=0; byte_pos<fileSize; byte_pos++){
    file.seekg(byte_pos, std::ios::beg);
    file.read(reinterpret_cast<char*>(&byte), sizeof(byte));
    ram[0x200+byte_pos] = byte;
    };
    file.close();
    };
    void clear_screen() {
    for (auto& row : screen) {
        for (auto& pixel : row) {
            pixel = false;
        }
    }
    };

//fetches instruction to be executed from memory  as a string and increments by 2 to be ready to read next instruction.
std::string fetch(){
    uint8_t first_half = *pc;

    uint8_t second_half = *(pc+1);
    
    std::stringstream ss;
    ss << std::hex <<  std::setw(2) << 
    std::setfill('0') << static_cast<int>(first_half);
    std::string first_half_str = ss.str();

    std::stringstream ss_2;
    ss_2 <<std::hex <<  std::setw(2) <<
    std::setfill('0') << static_cast<int>(second_half);
    
    std::string second_half_str = ss_2.str();
    
    first_half_str += second_half_str;
    pc += 2;
    return first_half_str;
}


/***********functions for executing instructions

************/
//sets program counter to address
    void jump(std::array<int,3> address){
        pc = mem_ptr + address[0];
    };
//sets VX register to NN    
    void set(std::array<int,3> bytes){   
        vRegisters[bytes[0]] = static_cast<uint8_t>(bytes[1]);

    };
//add value to  whatever value is stored in VX
    void add(std::array<int,3> bytes){
        //definitely works
        vRegisters[bytes[0]] += static_cast<uint8_t>(bytes[1]);  
    };
//sets the i Reigister to the value.
    void set_index(std::array<int,3> bytes){
        //definitely works
        iRegsiter = bytes[0];
        std::cout<<iRegsiter<<std::endl;

    };
//drawing instruction   
    void draw(std::array<int,3> bytes){
    //bytes[0] = x, bytes[1] = y, bytes [2] = n for the chip-8 draw instruction dxyn.        
        uint8_t x_coord = vRegisters[bytes[0]];
        uint8_t y_coord= vRegisters[bytes[1]];
        x_coord = x_coord % 64; y_coord = y_coord % 32;
        vRegisters[0xf]  = 0;
        int n = bytes[2];
        for (int row = 0; row < (n); ++row){
            uint8_t line = ram[row+iRegsiter];
            std::array<bool, 8> bits;
            bits = extractBits(line);
            for (bool i : bits){
               if ((i)){
                screen[x_coord][y_coord] = !screen[x_coord][y_coord];

                if(!screen[x_coord][y_coord]){
                    vRegisters[0xf] = 1;
                }
                }
                x_coord++;
                if (x_coord > 63){
                    break;
                }
            }
            y_coord++;
            if (y_coord > 31){
                break;
            }
        }
    }


/******* switch statements for executing instructions

************/
    void execute(std::string instruction){
        char firstNibble = instruction[0];

        switch(firstNibble){
            case '0':
            if(instruction.substr(1) == "0e0"){
            clear_screen();}
            else{
                std::cout<<"instruction not implemented yet"<<std::endl;  
            }
            break;
            case '1':
            jump(extract_integers(instruction, 1));break;
            case '6':
            set(extract_integers(instruction, 2)); break;
            case '7':
            add(extract_integers(instruction, 2)); break;
            case 'a':
            std::cout<<instruction<<std::endl;
            set_index(extract_integers(instruction,1)); break;
            case 'd':
            draw(extract_integers(instruction, 3)); break;
            default:
            std::cout<<"instruction not implemented yet"<<std::endl;
            break;

            };
            
        };
};