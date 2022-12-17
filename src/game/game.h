#pragma once
#include <SDL2/SDL.h>
#include <vector>

struct Vector2 {
        double x;
        double y;
};

struct Ball {
        Vector2 pos;
        Vector2 vel;
};

class Game {
public:
        Game(char diff);

        bool Initialize();

        void RunLoop();

        void Shutdown();
private:
        void ProcessInput();
        void UpdateGame();
        void GenerateOutput();

        const short WIDTH = 1024;
        const short HEIGHT = 768;

        SDL_Window *main_window;
        SDL_Renderer *main_renderer;
        bool is_running;
        bool is_paused;
        Uint32 ticks_count;

        const int thickness = 12;
        std::vector<Ball> balls;

        int paddle_lenght;
        Vector2 paddle_pos;
        short paddle_dir;
        short paddle_speed;
        short framerate;
        double frametime;
};
