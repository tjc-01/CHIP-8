#include <stack>
#include <iostream>
#include <string>
#include <sstream>
#include <bitset>

uint8_t font[] = {
//0
    0b01100000,
    0b10010000,
    0b10010000,
    0b10010000,
    0b01100000,

//1
    0b01100000,
    0b00100000,
    0b00100000,
    0b00100000,
    0b01110000,

//2
    0b11100000,
    0b00010000,
    0b00110000,
    0b01100000,
    0b11110000,
//3
    0b11100000,
    0b00010000,
    0b01100000,
    0b00010000,
    0b11100000,

    0b10100000,
    0b10100000,
    0b11100000,
    0b00100000,
    0b00100000,

    0b11110000,
    0b10000000,
    0b11110000,
    0b00010000,
    0b11110000,

    0b10000000,
    0b10000000,
    0b11110000,
    0b10010000,
    0b11110000,

    0b11110000,
    0b00010000,
    0b00100000,
    0b00100000,
    0b00100000,

    0b11110000,
    0b10010000,
    0b11110000,
    0b10010000,
    0b11110000,

    0b11110000,
    0b10010000,
    0b11110000,
    0b00010000,
    0b00010000,

    0b01100000,
    0b10010000,
    0b11110000,
    0b10010000,
    0b10010000,

    0b10000000,
    0b10000000,
    0b11110000,
    0b10010000,
    0b11110000,

    0b11110000,
    0b10000000,
    0b10000000,
    0b10000000,
    0b11110000,

    0b11100000,
    0b10010000,
    0b10010000,
    0b10010000,
    0b11100000,

    0b11110000,
    0b10000000,
    0b11100000,
    0b10000000,
    0b11110000,

    0b11110000,
    0b10000000,
    0b11100000,
    0b10000000,
    0b10000000,
};
//takes an 8 bit integer and outputs an array corresponding to its binary string
std::array<bool, 8> extractBits(uint8_t value) {
    std::array<bool, 8> output;
    for (int i = 7; i >= 0; --i) {
        output[7-i] = (value >> i) & 1;
    }
    return output;

};
class shell{
    public:
//The 16 8-bit registers are stored in an array.
    std::array<uint8_t, 16> vRegisters = {0};
//The I register (misspelled)
    int iRegsiter = 0;
//ram should be 4096 bytes but why not use 10000 bytes?
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




****************/
//load font into the CHIP-8 memory, starting at addr 0x50
    void load_font() {
    for (int i = 0; i < 80; i++) {
        ram[i] = font[i+0x50];
    };
    }
// sets screen to be completely dark
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

/********fetch instruction

*******/

//fetches instruction to be executed from memory  as a string and increments by 2 to be ready for next instruction.
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
    void jump(std::string address){
        //definitely works
        std::stringstream ss;
        ss<<std::hex<< address;
        int x;
        ss>>x;

        pc = mem_ptr + x;
    };
//sets VX register to NN    
    void set(std::string registr, std::string value){
        //definitely works
        std::stringstream ss; ss<<std::hex<< registr;
        unsigned int v; ss >> v;//v is the vregister being changed.

        std::stringstream ss_2;
        unsigned int intValue;
        ss_2<<std::hex<< value;
        ss_2>>intValue;


        vRegisters[v] = static_cast<uint8_t>(intValue);

    };
//add value to  whatever value is stored in VX
    void add(std::string registr, std::string value){
        //definitely works
        std::stringstream ss; ss<<std::hex<< registr;
        unsigned int v; ss >> v;//v = vregister being changed.

        unsigned int intValue;
        std::stringstream ss1; ss1<<std::hex<< value;
        ss1>>intValue;
        vRegisters[v] += static_cast<uint8_t>(intValue);
   
    };
//sets the i Reigister to the value.
    void set_index(std::string value){
        //definitely works

        int intValue;

        std::stringstream ss; ss<<std::hex<< value;
        ss>>intValue;

        iRegsiter = intValue;


    };
//drawing instruction   
    void draw(std::string x, std::string y, std::string n){        
        uint8_t x_coord; uint8_t y_coord;
        int x_int = std::stoi(x, nullptr, 16);
        int y_int = std::stoi(y, nullptr, 16);
        int n_int = std::stoi(n, nullptr, 16);
        x_coord = vRegisters[x_int];

      
        y_coord = vRegisters[y_int];

        x_coord = x_coord % 64; y_coord = y_coord % 32;

        vRegisters[0xf]  = 0;
           
        for (int row = 0; row < (n_int); ++row){
            uint8_t line = ram[row+iRegsiter];

            std::array<bool, 8> bits;
            bits = extractBits(line);
            for (bool i : bits){
               if ((screen[x_coord][y_coord] && i)){
                screen[x_coord][y_coord] = 0;
                vRegisters[0xf] = 1;
               } 
                else if((i && !(screen[x_coord][y_coord]))){
                    screen[x_coord][y_coord] = 1;
                }
                if (x_coord > 63){
                    break;
                }
                x_coord++;

            }
            y_coord++;
            if (y_coord > 31){
                break;
            }
        }
    //std::cout<<"end of draw"<<std::endl;
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

            jump(instruction.substr(1));break;
            case '6':
            set(instruction.substr(1,1), instruction.substr(2)); break;
            case '7':
 
            add(instruction.substr(1,1), instruction.substr(2)); break;
            case 'a':

            set_index(instruction.substr(1)); break;
            case 'd':
            draw(instruction.substr(1,1), instruction.substr(2,1), instruction.substr(3,1)); break;
            default:
            std::cout<<"instruction not implemented yet"<<std::endl;
            break;

            };
            
        };
};