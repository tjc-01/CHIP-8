#include <SFML/Graphics.hpp>
#include <random>
void display(std::array<std::array<bool, 32>, 64>* screen, bool* is_window_open){
    sf::RenderWindow window(sf::VideoMode(64.f, 32.f), "CHIP-8");
    window.setVerticalSyncEnabled(true);
    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed){
                *is_window_open = false;
            //terminates the main function when the user closes the emulator
                window.close();
                break;
            }          
        }

        window.clear();
        for (int i =0; i<64; i++){
        for(int j=0; j<32; j++){
            if ((*screen)[i][j] == 1){
                sf::RectangleShape rectangle(sf::Vector2f(1.f, 1.f));
                rectangle.setPosition(i, j);
                window.draw(rectangle);
                }
                }
            }

        window.display();
    }

    };
