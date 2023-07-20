#include <stack>
#include <iostream>
#include <string>
#include <sstream>
#include <bitset>
#include <array>
#include "font.h"
#include <vector>
#include <random>
#include <SFML/Graphics.hpp>
#include <map>

//mapping from the COSMAC VIP keypad
const std::unordered_map<int, sf::Keyboard::Key> key_map{
    {0x1, sf::Keyboard::Num1},
    {0x2, sf::Keyboard::Num2},
    {0x3, sf::Keyboard::Num3},
    {0xc, sf::Keyboard::Num4},
    {0x4, sf::Keyboard::Q},
    {0x5, sf::Keyboard::W},
    {0x6, sf::Keyboard::E},
    {0xd, sf::Keyboard::R},        
    {0x7, sf::Keyboard::A}, 
    {0x8, sf::Keyboard::S},
    {0x9, sf::Keyboard::D},  
    {0xe, sf::Keyboard::F},
    {0xa, sf::Keyboard::Z},
    {0x0, sf::Keyboard::X},
    {0xb, sf::Keyboard::C},
    {0xf, sf::Keyboard::V},             
};
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
extracts integers from commands, from a command as a vector, given the number of integers to extract.
e.g "dxyn" as the first argument and arg_nums = 3 returns x y and n as an vector of integers.
e.g. "4xnn" would return and arg_nums = 2 would return x and nn as two integers in an array.
*/
std::vector<int> extract_integers(std::string instruction, int arg_nums){
    std::vector<int> integers = {};
    switch (arg_nums){
    case 3:
        for (int i=1; i<4; i++){
        std::string x =  instruction.substr(i,1);
        int x_int = std::stoi(x, nullptr, 16);
        integers.push_back(x_int);
        };
        break;
    case 2:
    for (int i=1; i<4; i++){    
    std::string x = instruction.substr(i,i);
    int x_int = std::stoi(x, nullptr, 16);
    integers.push_back(x_int);
    };
    break;
    case 1:
    std::string x = instruction.substr(1,3);
    int x_int = std::stoi(x, nullptr, 16);
    integers.push_back(x_int);
    break; 
    }
return integers;
};


class shell{
    public:
//The 16 8-bit registers are stored in an array.
    std::array<uint8_t, 16> vRegisters = {0};
    int iRegister = 0;
    std::array<uint8_t, 4096> ram;
 // not used for implementation yet   
    unsigned char delay = 0;
    unsigned char sound = 0;
//array controlling the 64 by 32 display, with 1 corresponding to a pixel being on and 0 being off.
    std::array<std::array<bool, 32>, 64> screen = {0}; 
//stack stores memory addresses. used to return from an executed sub-routine.
    std::stack<int> stack;
//program counter initially pointing to 0x200, where the program  start is loaded in to CHIP-8 memory.
    uint8_t * pc = ram.data()+0x200;
    uint8_t * mem_ptr = ram.data();
//timers    
    uint8_t delay_timer;
    uint8_t sound_timer;
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
/***********functions for executing instructions

************/
//00e0
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
    /******* switch statement for executing instructions

************/
    void execute(std::string instruction){
        char firstNibble = instruction[0];

        auto instructionv_1 = extract_integers(instruction, 1);
        auto instructionv_2 = extract_integers(instruction, 2);
        auto instructionv_3 = extract_integers(instruction, 3);
        switch(firstNibble){
            
            case '0':
            if(instruction.substr(1) == "0e0"){
            clear_screen();}
            else if(instruction.substr(1) == "0ee"){
                return_from_subroutine(); 
            }
            break;
            case '1':
            jump(instructionv_1); break;
            case '2':
            start_subroutine(instructionv_1); break;
            case '3':
            skip(instructionv_2); break;  
            case '4':
            skip_not(instructionv_2); break;
            case '5':
            skip(instructionv_3); break;              
            case '6':
            set(instructionv_2); break;
            case '7':
            add(instructionv_2); break;
            case '8':
            logic_arithmetic_group(instructionv_3); break;
            case '9':
            skip_not(instructionv_3); break;
            case 'a':
            set_index(instructionv_1); break;
            case 'b':
            jump_with_offset(instructionv_1);break;
            case 'c':
            random(instructionv_2); break;
            case 'd':
            draw(instructionv_3); break;
            case 'e':
            skip_if_key(instructionv_3); break;
            case 'f':
                switch(instructionv_3[2])
            break;
            default:
            std::cout<<"instruction not implemented yet"<<std::endl;
            break;
            };
        };
//00ee
    void return_from_subroutine(){
        pc = mem_ptr + stack.top();
        stack.pop();
    }
//sets program counter to address
    void jump(std::vector<int> address){
        pc = mem_ptr + address[0];
    };
//sets program counter to address at the start of a subroutine, stores
//pc on the stack before it goes back
    void start_subroutine(std::vector<int> address){
        stack.push(*pc);
        pc = mem_ptr + address[0];
    }
//implements 3 and 5 instructions
    void skip(std::vector<int> bytes){
        if (vRegisters[bytes[0]] == bytes[1]){
            pc += 2;
        };
    }
//implements 4 and 9 instructions
    void skip_not(std::vector<int> bytes){
        if (vRegisters[bytes[0]] != bytes[1]){
            pc += 2;
        };
    }
    void set(std::vector<int> bytes){   
        vRegisters[bytes[0]] = static_cast<uint8_t>(bytes[1]);

    };
//add value to  whatever value is stored in VX
    void add(std::vector<int> bytes){
        //definitely works
        vRegisters[bytes[0]] += static_cast<uint8_t>(bytes[1]);  
    };
    void logic_arithmetic_group(std::vector<int> bytes){
        auto x = bytes[0]; auto y = bytes[1]; auto n = bytes[2];
        int total;
        switch (n){
            case 0:
            vRegisters[x] = vRegisters[y];break;
            case 1:
            vRegisters[x] = vRegisters[x] | vRegisters[y];break;
            case 2:
            vRegisters[x] = vRegisters[x] & vRegisters[y];break;
            case 3:
            vRegisters[x] = vRegisters[x] ^ vRegisters[y];break;    
            case 4:
            total = static_cast<int>(vRegisters[x]) + static_cast<int>(vRegisters[y]);
            //if vx+vy overflows set VF to 1
            vRegisters[0xf] = static_cast<uint8_t>(total>255);
            vRegisters[x] = total;break;  
            case 5:
            total = static_cast<int>(vRegisters[x]) - static_cast<int>(vRegisters[y]);
            //if result underflows, set vf accordingly.
            vRegisters[0xf] = static_cast<uint8_t>(total>0);
            vRegisters[x] = vRegisters[x] - vRegisters[y];break;
            case 7:
            total = static_cast<int>(vRegisters[y]) - static_cast<int>(vRegisters[x]);
            //if reesult underflows, set vf accordingly..
            vRegisters[0xf] = static_cast<uint8_t>(total>=0);
            vRegisters[x] = vRegisters[y] - vRegisters[x];break;
            case 6:
            vRegisters[x] = vRegisters[y];
            //carry flag is set to least significant bit being shifted out in the final step.
            vRegisters[0xf] = extractBits(vRegisters[x])[7];
            vRegisters[x] = vRegisters[x] >> 1;
            break;
            case 0xe:
            vRegisters[x] = vRegisters[y];
            //carry flag is set to most significant bit being shifted out in the final step.
            vRegisters[0xf] = extractBits(vRegisters[x])[0];
            vRegisters[x] = vRegisters[x] << 1;
            break;
        };
        };
//sets the i Reigister to the value.
    void set_index(std::vector<int> bytes){
        //definitely works
        iRegister = bytes[0];
    };
//BNNN, jumps with an offset NNN + whatever is stored in V0.
    void jump_with_offset(std::vector<int> bytes){
        pc = mem_ptr + bytes[0]+vRegisters[0];
    }
//CXNN, binary AND NN with a random 8-bit integer and stores it in X.
    void random(std::vector<int> bytes){
        int random_num = rand()%256;
        vRegisters[bytes[0]] = bytes[1] & random_num;

    }
//drawing instruction   
    void draw(std::vector<int> bytes){
    //bytes[0] = x, bytes[1] = y, bytes [2] = n for the chip-8 draw instruction dxyn.
     
        int x_coord_original = vRegisters[bytes[0]];
        int y_coord= vRegisters[bytes[1]];
        x_coord_original = x_coord_original % 64; y_coord = y_coord % 32;
        vRegisters[0xf]  = 0;
        int n = bytes[2];

        for (int row = 0; row < (n); row++){
            uint8_t x_coord = x_coord_original;
            uint8_t line = ram[row+iRegister];
            auto bits = extractBits(line);
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
void skip_if_key(std::vector<int> bytes){
    int x = vRegisters[bytes[0]];
    auto key = key_map.find(x);
    switch(bytes[1]){
    case 9:
    if (sf::Keyboard::isKeyPressed(key->second)){
        pc += 2;
    }
    break;
    case 0xa:
    if (!sf::Keyboard::isKeyPressed(key->second)){
        pc += 2;    
    }
    break;
    }
}



};