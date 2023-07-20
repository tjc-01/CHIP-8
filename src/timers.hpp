#include <iostream>
#include <chrono>
#include <thread>
//decrement these timers every 60th of a second, so around every 16.667ms.
const int interval = 16667;
void handle_timers(uint8_t * delay_timer, uint8_t * sound_timer){
    if (*delay_timer != 0){
        *delay_timer--;
    }
    if (*sound_timer != 0){
        *sound_timer--;
    }
        std::this_thread::sleep_for(std::chrono::microseconds(interval));
    }