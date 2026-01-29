#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "grid.hpp"
#include "line.hpp"
#include "button.hpp"
#include "inipp.hpp"
#include "arial.hpp"

typedef unsigned int uint;

class Game
{
private:
    sf::RenderWindow *window;

    std::vector<sf::Drawable *> pnl_drawable;

    Grid grid;
    sf::VertexArray gridLines;
    bool is_grid_lines = false;
    sf::Transform grid_transform;

    int width_cells;
    int height_cells;
    bool isplaying = false;
    sf::Vector2u win_size;
    bool mouse_in_window;

    sf::Color alive_color = sf::Color::White;
    sf::Color dead_color = sf::Color::Black;
    sf::Color grid_line_color = sf::Color(100, 100, 100);
    sf::Color background_color = sf::Color(70, 70, 70);

    sf::Vector2u cell_clicked;
    sf::Vector2f seen_center;

    const float grid_lines_width = 0.05f; // percentage of cell_size
    float curr_scale;
    float move_per_frame_multiplier = 30.f;
    float move_per_frame = move_per_frame_multiplier / curr_scale;
    const float scale_per_frame = 1.04f; // 1.04 -> scale = scale * 1.04 meaning it scales 4% per frame
    // const float move_per_mpf = 0.1f;
    // const float scale_per_spf = 0.005f;

    // Panel:
    struct Panel
    {
        uint WINDOW_WIDTH = 1600u;
        uint WINDOW_HEIGHT = 1200u;

        float pnl_width = WINDOW_WIDTH / 4;
        float btn_width = (pnl_width / 40) * 15;
        float btn_height = WINDOW_HEIGHT / 24;
        float middle_space = (pnl_width / 40) * 2;
        float margin = (pnl_width / 40) * 4;
        float rcol_offset = margin + (btn_width / 2);
        float lcol_offset = pnl_width - rcol_offset;
        float row_height = btn_height + (WINDOW_HEIGHT / 40);
        float separator_width = 2.f;

        sf::Font font = sf::Font(arial_ttf,arial_ttf_len);
        sf::Text::Style style = sf::Text::Style::Bold;
        sf::Color def_btn_color = sf::Color(120, 120, 120);

        Line separator = Line({WINDOW_WIDTH - pnl_width, 0.f}, {WINDOW_WIDTH - pnl_width, float(WINDOW_HEIGHT)}, separator_width, sf::Color::Black);
        sf::RectangleShape background = sf::RectangleShape({pnl_width, float(WINDOW_HEIGHT)});

        Button start_button = Button({btn_width, btn_height}, {WINDOW_WIDTH - lcol_offset, row_height}, "Start", style, sf::Color(70, 210, 70));
        Button stop_button = Button({btn_width, btn_height}, {WINDOW_WIDTH - rcol_offset, row_height}, "Stop", style, sf::Color(210, 70, 70));
        Button clear_button = Button({btn_width, btn_height}, {WINDOW_WIDTH - lcol_offset, 2 * row_height}, "Clear", style, def_btn_color);
        Button rand_button = Button({btn_width, btn_height}, {WINDOW_WIDTH - rcol_offset, 2 * row_height}, "Rand", style, def_btn_color);
        Button glines_button = Button({2 * btn_width + middle_space, btn_height}, {WINDOW_WIDTH - (pnl_width / 2), 3 * row_height}, "Toggle grid lines", style, def_btn_color);
        Button center_button = Button({btn_width, btn_height}, {WINDOW_WIDTH - (pnl_width / 2), 4 * row_height}, "Center", style, def_btn_color);
        Button fit_button = Button({btn_width, btn_height}, {WINDOW_WIDTH - (pnl_width / 2), 4 * row_height}, "Fit", style, def_btn_color);

        std::string controls_text = "CONTROLS:\nStart/Stop - space\nClear - C\nRandomize - R\nToggle grid lines - T\nCenter grid - N\nFit grid to size - F\nMove - arrows\nZoom - +/-";
        sf::Text controls = sf::Text(font, controls_text, 25);
    };
    Panel panel;

public:
    Game() {};
    Game(int init_width_cells, int init_height_cells, sf::RenderWindow *init_window, std::optional<inipp::Ini<char>> ini);
    ~Game() {}

    void startGame() { isplaying = true; }
    void stopGame() { isplaying = false; }
    void centerGrid(bool fit_to_scale);
    // void fitGrid();
    bool isPlaying() { return isplaying; }
    void nextFrame() { grid.nextFrame(); }
    void nextFrameSkip(uint n) { grid.nextFrameSkip(n); }

    void handleEvents(sf::RenderWindow &window);
    void updatePanelSize();

    std::vector<sf::Drawable *> getPanelDrawable() { return pnl_drawable; }
    sf::VertexArray &getGrid() { return grid.getGrid(); }
    sf::Transform &getGridTrans() { return grid_transform; }
    std::optional<sf::VertexArray *> getGridLines()
    {
        if (is_grid_lines)
            return &gridLines;
        return {};
    }
};
