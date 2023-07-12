#include <iostream>
#include <array>
#include <random>
#include <SFML/Graphics.hpp>
#include "display.hpp"
const int Rows = 3;
const int Cols = 4;

std::array<bool, 8> extractBits(uint8_t value) {
    std::array<bool, 8> output;
    for (int i = 7; i >= 0; --i) {
        output[7-i] = (value >> i) & 1;
    }
    return output;

};

int main(){
    sf::RenderWindow window(sf::VideoMode(64.f, 32.f), "CHIP-8");
    window.setVerticalSyncEnabled(true);
    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop

        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed){
                window.close();
            }
        }
        window.clear();
      

        sf::RectangleShape rectangle(sf::Vector2f(1.f, 1.f));
        rectangle.setPosition(63, 31);
        window.draw(rectangle);
        sf::RectangleShape rectangle2(sf::Vector2f(1.f, 1.f));
        rectangle2.setPosition(12, 21);
        window.draw(rectangle2);
        window.display();
                }
                
            


    
    return 0;
}

