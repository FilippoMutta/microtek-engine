#include "game/game.h"
#include <iostream>

int main(int argc, char **argv) {
        char diff;
        std::cout << "Enter difficulty:";

        diff = getchar();
        
        Game game(diff);

        bool success = game.Initialize();
        
        if (success) {
                std::cout << "Game initialized!\n";
                
                game.RunLoop();
        }

        std::cout << "Quitting.\n";

        game.Shutdown();

        std::cout << "Done.\n";
        return 0;
}
