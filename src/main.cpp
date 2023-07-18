#include <iostream>
#include <fstream>
#include <iomanip>
#include <typeinfo>
#include <sstream>
#include <thread>
#include "shell.hpp"
#include "display.hpp"
#include <time.h>


int main(){
    std::string f_name = "IBM Logo.ch8";
    shell emulator(f_name);

    for(int i =0; i<100; ++i){
        std::string instruction = emulator.fetch();
        emulator.execute(instruction);
    }
   display(&(emulator.screen));
    return 0;
};