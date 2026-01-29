#include "../include/line.hpp"
#include <cmath>

Line::Line()
{
    line = sf::RectangleShape({0.f, 0.f});
}

Line::Line(sf::Vector2f begin, sf::Vector2f end, float line_width, sf::Color line_color)
{
    length = std::sqrt(std::pow(std::abs(begin.x - end.x), 2) + std::pow(std::abs(begin.y - end.y), 2));
    width = line_width;
    line = sf::RectangleShape({length, width});
    line.setPosition(begin);
    line.setRotation(sf::radians(std::asin(std::abs(begin.y - end.y) / length)));
    line.setFillColor(line_color);
}

Line::~Line()
{
}

void Line::drawOn(sf::RenderWindow &window)
{
    window.draw(line);
}
void Line::resize(sf::Vector2f begin, sf::Vector2f end)
{
    length = std::sqrt(std::pow(std::abs(begin.x - end.x), 2) + std::pow(std::abs(begin.y - end.y), 2));
    line.setSize({length, width});
    line.setPosition(begin);
    line.setRotation(sf::radians(std::asin(std::abs(begin.y - end.y) / length)));
}

sf::RectangleShape* Line::getLine()
{
    return &line;
}
