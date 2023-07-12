#include <SFML/Graphics.hpp>
#include <random>
void display(std::array<std::array<bool, 32>, 64>* screen){
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

// int main(){
//     int Rows = 64;
//     int Cols = 32;
//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_int_distribution<> dist(0, 1);

//     std::array<std::array<bool, 32>, 64> array;

//     for (int i = 0; i < Rows; ++i)
//     {
//         for (int j = 0; j < Cols; ++j)
//         {
//             array[i][j] = dist(gen);
//         }
//     }
//     std::array<std::array<bool, 32>, 64>* ptr = &array;

//     display stfl_window(ptr);
//     return 0;
// }