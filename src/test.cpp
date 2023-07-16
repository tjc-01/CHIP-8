#include <iostream>
#include <array>
#include <random>

const int Rows = 3;
const int Cols = 4;

std::array<bool, 8> extractBits(uint8_t value) {
    std::array<bool, 8> output;
    for (int i = 7; i >= 0; --i) {
        output[7-i] = (value >> i) & 1;
    }
    return output;

};

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

int main(){
    std::string test  = "af26";
    std::array<int,3> ints = extract_integers(test,2);
    for (int i : ints)
    std::cout<<i<<std::endl;
    return 0;
}

