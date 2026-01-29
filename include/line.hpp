#pragma once 
#include <SFML/Graphics.hpp>

class Line
{
private:
    sf::RectangleShape line;
    float length;
    float width;
public:
    Line();
    Line(sf::Vector2f, sf::Vector2f, float, sf::Color);
    ~Line();
    void drawOn(sf::RenderWindow& );
    void resize(sf::Vector2f, sf::Vector2f);
    sf::RectangleShape* getLine();
};


