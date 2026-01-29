#include <iostream>
#include <vector>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "include/game.hpp"
#include "include/inipp.hpp"

typedef unsigned int uint;

uint WINDOW_WIDTH = 800;
uint WINDOW_HEIGHT = 600;

uint init_grid_width = 200;
uint init_grid_height = 200;

uint fps_limit = 60;

sf::Color background_color = sf::Color(70, 70, 70);

int main()
{
    
    std::optional<inipp::Ini<char>> ini = {};
    std::ifstream is("config.ini");

    if (!is)
    {
        std::cerr << "Could not open config.ini. Using default values.\n";
        ini = {};
    }
    else
    {
        ini = inipp::Ini<char>();
        ini.value().parse(is);
        inipp::get_value(ini.value().sections["window"], "WindowWidth", WINDOW_WIDTH);
        inipp::get_value(ini.value().sections["window"], "WindowHeight", WINDOW_HEIGHT);
        inipp::get_value(ini.value().sections["window"], "FPSLimit", fps_limit);
        inipp::get_value(ini.value().sections["grid"], "GridWidth", init_grid_width);
        inipp::get_value(ini.value().sections["grid"], "GridHeight", init_grid_height);
    }

    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Game of Life");
    window.setFramerateLimit(60);

    Game *GoL = nullptr;
    try
    {
        GoL = new Game(init_grid_width, init_grid_height, &window, ini);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    while (window.isOpen())
    {
        GoL->handleEvents(window);

        if (GoL->isPlaying())
        {
            GoL->nextFrame();
        }

        // render all
        window.clear(background_color);

        window.draw(GoL->getGrid(), GoL->getGridTrans());
        std::optional<sf::VertexArray *> grid_lines = GoL->getGridLines();
        if (grid_lines.has_value())
        {
            window.draw(*grid_lines.value(), GoL->getGridTrans());
        }
        for (sf::Drawable *i : GoL->getPanelDrawable())
        {
            window.draw(*i);
        }
        window.display();
    }
    delete GoL;
    return 0;
}