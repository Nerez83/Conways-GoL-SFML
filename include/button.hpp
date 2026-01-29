#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "arial.hpp"

class Button
{
private:
    sf::RectangleShape rectangle;

    sf::Color def_color;
    sf::Color hover_color;
    sf::Color clicked_color;

    float border_thickness;
    sf::Color border_color = sf::Color(200, 200, 200);

    sf::Font font;
    sf::Text text = sf::Text(font, "", 0);

    bool isClicked = false;

public:
    Button() {}
    Button(sf::Vector2f, sf::Vector2f, std::string, sf::Text::Style, sf::Color);
    ~Button() {}

    sf::RectangleShape *getRectangle();
    sf::Text *getText();
    void setColor(sf::Color);
    void setPosition(sf::Vector2f);
    void clicked();
    void released();
    void hovered();
    void showDefColor();
    sf::FloatRect getGlobalBounds();
    void setSize(sf::Vector2f new_size);
};
