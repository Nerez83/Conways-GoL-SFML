#include "../include/button.hpp"
inline sf::Color offsetColor(sf::Color color, int offset)
{
    float red_out = color.r + offset <= 0 ? 0 : (color.r + offset >= 255 ? 255 : color.r + offset);
    float green_out = color.g + offset <= 0 ? 0 : (color.g + offset >= 255 ? 255 : color.g + offset);
    float blue_out = color.b + offset <= 0 ? 0 : (color.b + offset >= 255 ? 255 : color.b + offset);
    return sf::Color(red_out, green_out, blue_out);
}

Button::Button(sf::Vector2f size, sf::Vector2f position, std::string text_str, sf::Text::Style style, sf::Color color)
{
    if (!font.openFromMemory(arial_ttf,arial_ttf_len))
    {
        throw std::runtime_error("Failed to load font");
    }
    
    rectangle = sf::RectangleShape(size);
    rectangle.setOrigin(rectangle.getGeometricCenter());
    rectangle.setPosition(position);
    rectangle.setOutlineThickness(size.y / 10);
    rectangle.setOutlineColor(border_color);

    text.setFont(font);
    text.setString(text_str);
    text.setCharacterSize((unsigned int)((rectangle.getSize().y) * 0.6f));
    float char_size_factor = 1;
    sf::FloatRect bounds = text.getLocalBounds();
    while (bounds.size.x > size.x - 4 * border_thickness)
    {
        char_size_factor *= 0.9f;
        text.setCharacterSize((unsigned int)((rectangle.getSize().y) * 0.6f * char_size_factor));
        bounds = text.getLocalBounds();
    }

    text.setOrigin(text.getLocalBounds().getCenter());
    text.setFillColor(sf::Color::Black);
    text.setStyle(style);
    setColor(color);
    setPosition(position);
}
sf::RectangleShape *Button::getRectangle()
{
    return &rectangle;
}
sf::Text *Button::getText()
{
    return &text;
}
void Button::setColor(sf::Color color)
{
    int outline_color_offset = -40;
    int hovered_color_offset = 20;
    int clicked_color_offset = -40;
    def_color = sf::Color(color.r, color.g, color.b);
    rectangle.setOutlineColor(offsetColor(color, outline_color_offset));
    hover_color = offsetColor(color, hovered_color_offset);
    clicked_color = offsetColor(color, clicked_color_offset);
    rectangle.setFillColor(def_color);
}
void Button::setPosition(sf::Vector2f position)
{
    rectangle.setOrigin(rectangle.getGeometricCenter());
    rectangle.setPosition(position);
    text.setPosition(position); // for some reason this alone doesnt work

    sf::FloatRect bounds = text.getGlobalBounds();
    text.setPosition({position.x - (bounds.getCenter().x - position.x), position.y - (bounds.getCenter().y - position.y)});
}
void Button::clicked()
{
    isClicked = true;
    rectangle.setFillColor(clicked_color);
}
void Button::hovered()
{
    if (!isClicked)
    {
        rectangle.setFillColor(hover_color);
    }
}
void Button::released()
{
    isClicked = false;
    rectangle.setFillColor(def_color);
}
void Button::showDefColor()
{
    rectangle.setFillColor(def_color);
}
sf::FloatRect Button::getGlobalBounds()
{
    return rectangle.getGlobalBounds();
}
void Button::setSize(sf::Vector2f new_size){
    rectangle.setSize(new_size);
    rectangle.setOutlineThickness(new_size.y / 10);

    text.setCharacterSize(static_cast<unsigned int>((new_size.y) * 0.6f));
    float char_size_factor = 1;
    sf::FloatRect bounds = text.getLocalBounds();
    while (bounds.size.x > new_size.x - 4 * border_thickness)
    {
        char_size_factor *= 0.9f;
        text.setCharacterSize(static_cast<unsigned int>((new_size.y) * 0.6f * char_size_factor));
        bounds = text.getLocalBounds();
    }
}