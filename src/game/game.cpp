#include <random>
#include <iostream>
#include "game.h"
Game::Game(char diff) {
        unsigned int number_balls;
        float paddle_proportion;

        switch(diff) {
                default:
                case 'E':
                        number_balls = 1;
                        paddle_proportion = 2.5;
                        paddle_speed = 300;
                        break;
                case 'M':
                        number_balls = 2;
                        paddle_proportion = 3;
                        paddle_speed = 350;
                        break;
                case 'H':
                        number_balls = 3;
                        paddle_proportion = 3.5;
                        paddle_speed = 400;
                        break;
                case 'D':
                        number_balls = 4;
                        paddle_proportion = 4;
                        paddle_speed = 500;
                        break;
                case 'T':
                        number_balls = 100000;
                        paddle_proportion = 1;
                        paddle_speed = 0;
                        break;

        }

        is_running = true;
        is_paused = false;
        for (unsigned int i = 0; i < number_balls; i++) {
                Ball ball;
                ball.pos.x = WIDTH/2 + thickness;
                ball.pos.y = HEIGHT/2 + thickness;
                srand(i * time(NULL));
                ball.vel.x = rand() % 300 + 200;
                srand(-i * time(NULL));
                ball.vel.y = rand() % 300 + 200;
                std::cout << "Ball " << i << "...\r";
                balls.push_back(ball);
        }

        paddle_lenght = HEIGHT / paddle_proportion;
        paddle_pos.x = 0;
        paddle_pos.y = HEIGHT/2;

        ticks_count = 0;
        framerate = 60;
        frametime = static_cast<int>((1.0f/framerate)*1000.0f);
}

bool Game::Initialize() {
        int sdl_result = SDL_Init(SDL_INIT_VIDEO);

        if (sdl_result != 0) {
                SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
                return false;
        }

        main_window = SDL_CreateWindow(
                "Hell on earth",
                100,
                100,
                WIDTH,
                HEIGHT,
                SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
        );

        if (!main_window) {
                SDL_Log("Failed to create window: %s", SDL_GetError());
                return false;
        }

        main_renderer = SDL_CreateRenderer(
                main_window,
                -1,
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        );

        if (!main_renderer) {
                SDL_Log("Failed to initialize renderer: %s", SDL_GetError());
                return false;
        }

        return true;
}

void Game::RunLoop() {
        while (is_running) {
                ProcessInput();
                if (!is_paused) {
                        UpdateGame();
                        GenerateOutput();
                }
        }
}

void Game::Shutdown() {
        SDL_DestroyRenderer(main_renderer);
        SDL_DestroyWindow(main_window);
        SDL_Quit();
}

void Game::ProcessInput() {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
                switch(event.type) {
                        case SDL_QUIT:
                                is_running = false;
                                break;
                }
        }

        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_ESCAPE]) {
                is_running = false;
        }

        if (state[SDL_SCANCODE_P]) {
                is_paused = true;
        } else if (state[SDL_SCANCODE_O]) {
                is_paused = false;
        }

        paddle_dir = 0;

        if(state[SDL_SCANCODE_W]) {
                paddle_dir -= 1;
        }
        
        if(state[SDL_SCANCODE_S]) {
                paddle_dir += 1;
        }
}

void Game::UpdateGame() {
        while(!SDL_TICKS_PASSED(SDL_GetTicks(), ticks_count + frametime)); // 16 ms from last frame

        double delta_time = (SDL_GetTicks() - ticks_count) / 1000.0f;
        ticks_count = SDL_GetTicks();

        if (delta_time > 0.05f) {
                delta_time = 0.05f;
        }

        if(paddle_dir != 0) {
                paddle_pos.y += paddle_dir * paddle_speed * delta_time;

                if (paddle_pos.y < (paddle_lenght / 2.0f + thickness)) {
                        paddle_pos.y = paddle_lenght/2.0f + thickness;
                } else if (paddle_pos.y > (HEIGHT - paddle_lenght/2.0f - thickness)) {
                        paddle_pos.y = HEIGHT - paddle_lenght/2.0f - thickness;
                }
        }

        for (unsigned int i = 0; i < balls.size(); i++) {
                balls[i].pos.x += balls[i].vel.x * delta_time;
                balls[i].pos.y += balls[i].vel.y * delta_time;

                double diff = paddle_pos.y - balls[i].pos.y;
                diff = (diff > 0.0f) ? diff : -diff;
	        if (
        		diff <= paddle_lenght / 2.0f &&
	        	balls[i].pos.x <= thickness &&
        		balls[i].vel.x < 0.0f) {
		        balls[i].vel.x *= -1.0f;
	        } else if (balls[i].pos.x <= 0.0f) {
                        is_running = false;
	        }

                if (balls[i].pos.x >= (WIDTH - thickness) && balls[i].vel.x > 0.0f) {
		        balls[i].vel.x *= -1;
	        } else if (balls[i].pos.y <= thickness && balls[i].vel.y < 0.0f) {
        		balls[i].vel.y *= -1;
	        } else if (balls[i].pos.y >= (HEIGHT - thickness) &&
        		balls[i].vel.y > 0.0f) {
		        balls[i].vel.y *= -1;
	        }
        }
}

void Game::GenerateOutput() {
        SDL_SetRenderDrawColor(
                main_renderer,
                0,
                0,
                255,
                255
        );

        SDL_RenderClear(main_renderer);
        
        SDL_SetRenderDrawColor(
                main_renderer,
                255,
                255,
                255,
                255
        );

        SDL_Rect wall {
                static_cast<int>(paddle_pos.x - thickness/2),
                static_cast<int>(paddle_pos.y - paddle_lenght/2),
                thickness,
                paddle_lenght
        };

        SDL_RenderFillRect(main_renderer, &wall);


        for (unsigned int i = 0; i < balls.size(); i++) {
                SDL_Rect ball {
                        static_cast<int>(balls[i].pos.x - thickness/2),
                        static_cast<int>(balls[i].pos.y - thickness/2),
                        thickness,
                        thickness
                };

                SDL_RenderFillRect(main_renderer, &ball);
        }

        SDL_RenderPresent(main_renderer);
}
