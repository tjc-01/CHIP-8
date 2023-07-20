#include <iostream>
#include <fstream>
#include <iomanip>
#include <typeinfo>
#include <sstream>
#include <thread>
#include "emulator.hpp"
#include "display.hpp"
#include <time.h>
#include "timers.hpp"

int main(){
    std::string f_name = "IBM Logo.ch8";
    shell emulator(f_name);
    bool window_open = true;
    std::thread window_thread(display, &emulator.screen, &window_open);
    std::thread timer_thread(handle_timers, &emulator.delay_timer, &emulator.sound_timer);
    while(window_open){
        std::string instruction = emulator.fetch();
        emulator.execute(instruction);
    }
    timer_thread.join();
    window_thread.join(); 
    return 0;
};