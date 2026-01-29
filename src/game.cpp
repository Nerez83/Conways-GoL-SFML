#include <cstdlib>
#include <iostream>
#include "../include/game.hpp"
#include "../include/rand_bool.hpp"

bool handleBtnEvents(Button &btn, std::optional<sf::Event> event, sf::Vector2i mouse_pos)
{
    if (btn.getGlobalBounds().contains(sf::Vector2f({float(mouse_pos.x), float(mouse_pos.y)})))
    {
        btn.hovered();

        if (auto *mouse = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouse->button == sf::Mouse::Button::Left)
            {
                btn.clicked();
            }
        }
        else if (auto *mouse = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouse->button == sf::Mouse::Button::Left)
            {
                btn.released();
                return true;
            }
        }
    }
    else
    {
        btn.showDefColor();
    }
    return false;
}

Game::Game(int init_width_cells, int init_height_cells, sf::RenderWindow *init_window,std::optional<inipp::Ini<char>> ini)
    : window(init_window), grid(Grid(init_width_cells, init_height_cells)), gridLines(sf::PrimitiveType::Triangles, (init_width_cells + init_height_cells + 2) * 6), width_cells(init_width_cells), height_cells(init_height_cells)
{
    
    // get config
    if (ini.has_value())
    {
        inipp::get_value(ini.value().sections["misc"], "MoveSpeedMultiplier", move_per_frame_multiplier);
    }

    // init vertical grid lines
    for (int x = 0; x <= init_width_cells; x++)
    {

        gridLines[6 * x].position = {(x - grid_lines_width), -grid_lines_width};
        gridLines[(6 * x) + 1].position = {(x + grid_lines_width), -grid_lines_width};
        gridLines[(6 * x) + 2].position = {(x + grid_lines_width), (init_height_cells + grid_lines_width)};

        gridLines[(6 * x) + 3].position = {(x + grid_lines_width), (init_height_cells + grid_lines_width)};
        gridLines[(6 * x) + 4].position = {(x - grid_lines_width), (init_height_cells + grid_lines_width)};
        gridLines[(6 * x) + 5].position = {(x - grid_lines_width), -grid_lines_width};

        for (int i = 0; i < 6; i++)
        {
            gridLines[(6 * x) + i].color = grid_line_color;
        }
    }

    // init horizontal grid lines
    for (int y = 0; y <= init_height_cells; y++)
    {

        gridLines[6 * (init_width_cells + 1 + y)].position = {-grid_lines_width, (y + grid_lines_width)};
        gridLines[(6 * (init_width_cells + 1 + y)) + 1].position = {-grid_lines_width, (y - grid_lines_width)};
        gridLines[(6 * (init_width_cells + 1 + y)) + 2].position = {(init_width_cells + grid_lines_width), (y - grid_lines_width)};

        gridLines[(6 * (init_width_cells + 1 + y)) + 3].position = {(init_width_cells + grid_lines_width), (y - grid_lines_width)};
        gridLines[(6 * (init_width_cells + 1 + y)) + 4].position = {(init_width_cells + grid_lines_width), (y + grid_lines_width)};
        gridLines[(6 * (init_width_cells + 1 + y)) + 5].position = {-grid_lines_width, (y + grid_lines_width)};

        for (int i = 0; i < 6; i++)
        {
            gridLines[(6 * (init_width_cells + 1 + y)) + i].color = grid_line_color;
        }
    }

    // init panel
    pnl_drawable.push_back(&panel.background);
    pnl_drawable.push_back(&panel.controls);
    pnl_drawable.push_back(panel.separator.getLine());
    pnl_drawable.push_back(panel.start_button.getRectangle());
    pnl_drawable.push_back(panel.start_button.getText());
    pnl_drawable.push_back(panel.stop_button.getRectangle());
    pnl_drawable.push_back(panel.stop_button.getText());
    pnl_drawable.push_back(panel.clear_button.getRectangle());
    pnl_drawable.push_back(panel.clear_button.getText());
    pnl_drawable.push_back(panel.rand_button.getRectangle());
    pnl_drawable.push_back(panel.rand_button.getText());
    pnl_drawable.push_back(panel.glines_button.getRectangle());
    pnl_drawable.push_back(panel.glines_button.getText());
    pnl_drawable.push_back(panel.center_button.getRectangle());
    pnl_drawable.push_back(panel.center_button.getText());
    pnl_drawable.push_back(panel.fit_button.getRectangle());
    pnl_drawable.push_back(panel.fit_button.getText());

    win_size = window->getSize();

    // init background
    panel.background.setFillColor(sf::Color(50, 50, 50));
    panel.background.setPosition({win_size.x - panel.pnl_width, 0.f});

    // init controls
    panel.controls.setFillColor(sf::Color::White);
    panel.controls.setPosition({win_size.x - panel.pnl_width + panel.margin, win_size.y - panel.controls.getGlobalBounds().size.y - panel.margin});
    
    if (!panel.font.openFromMemory(arial_ttf,arial_ttf_len))
    {
        throw std::runtime_error("Failed to load font");
    }
    panel.controls.setFont(panel.font);

    curr_scale = 1.f;
    updatePanelSize();
    centerGrid(true);

    if (init_height_cells * init_width_cells > 1000000)
        std::cout << "Warning: Playing with more than 1 000 000 cells is not recommended." << std::endl;
}
void Game::centerGrid(bool fit_to_scale = false)
{
    sf::FloatRect global_bounds = grid_transform.transformRect(grid.getGrid().getBounds());
    win_size = window->getSize();

    // update seen_center (just to be safe)
    seen_center.x = (((win_size.x - panel.pnl_width) / 2) - (global_bounds.position.x)) / curr_scale;
    seen_center.y = ((win_size.y / 2) - global_bounds.position.y) / curr_scale;

    // scale to default (such that curr_scale is 1)
    grid_transform.scale({1 / curr_scale, 1 / curr_scale}, seen_center);

    // center grid
    global_bounds = grid_transform.transformRect(grid.getGrid().getBounds());
    grid_transform.translate({-global_bounds.position.x, -global_bounds.position.y});
    grid_transform.translate({(win_size.x - panel.pnl_width - width_cells) / 2, float(win_size.y - height_cells) / 2});
    seen_center = {float(width_cells / 2), float(height_cells / 2)};

    if (fit_to_scale)
    {
        // calculate the correct scale
        float scale_to_fit;
        float scale_to_fit_x = (win_size.x - panel.pnl_width) / width_cells;
        float scale_to_fit_y = win_size.y / height_cells;

        if (scale_to_fit_x < scale_to_fit_y)
        {
            scale_to_fit = scale_to_fit_x;
        }
        else
        {
            scale_to_fit = scale_to_fit_y;
        }
        // scale to fit
        grid_transform.scale({scale_to_fit, scale_to_fit}, seen_center);
        curr_scale = scale_to_fit;
        move_per_frame = move_per_frame_multiplier / curr_scale;
        return;
    }

    // scale back
    grid_transform.scale({curr_scale, curr_scale}, seen_center);
}

void Game::updatePanelSize()
{
    panel.pnl_width = win_size.x / 4;
    panel.btn_width = (panel.pnl_width / 40) * 15;
    panel.btn_height = win_size.y / 24;
    panel.middle_space = (panel.pnl_width / 40) * 2;
    panel.margin = (panel.pnl_width / 40) * 4;
    panel.rcol_offset = panel.margin + (panel.btn_width / 2);
    panel.lcol_offset = panel.pnl_width - panel.rcol_offset;
    panel.row_height = panel.btn_height + (win_size.y / 40);

    panel.background.setSize({panel.pnl_width, float(win_size.y)});
    panel.background.setPosition({win_size.x - panel.pnl_width, 0.f});
    panel.separator.resize({win_size.x - panel.pnl_width, 0.f}, {win_size.x - panel.pnl_width, float(win_size.y)});

    panel.start_button.setSize({panel.btn_width, panel.btn_height});
    panel.stop_button.setSize({panel.btn_width, panel.btn_height});
    panel.clear_button.setSize({panel.btn_width, panel.btn_height});
    panel.rand_button.setSize({panel.btn_width, panel.btn_height});
    panel.glines_button.setSize({2 * panel.btn_width + panel.middle_space, panel.btn_height});
    panel.center_button.setSize({panel.btn_width, panel.btn_height});
    panel.fit_button.setSize({panel.btn_width, panel.btn_height});
    panel.controls.setCharacterSize(panel.pnl_width / 16);

    panel.start_button.setPosition({win_size.x - panel.lcol_offset, panel.row_height});
    panel.stop_button.setPosition({win_size.x - panel.rcol_offset, panel.row_height});
    panel.clear_button.setPosition({win_size.x - panel.lcol_offset, 2 * panel.row_height});
    panel.rand_button.setPosition({win_size.x - panel.rcol_offset, 2 * panel.row_height});
    panel.glines_button.setPosition({win_size.x - (panel.pnl_width / 2), 3 * panel.row_height});
    panel.center_button.setPosition({win_size.x - panel.lcol_offset, 4 * panel.row_height});
    panel.fit_button.setPosition({win_size.x - panel.rcol_offset, 4 * panel.row_height});
    panel.controls.setPosition({win_size.x - panel.pnl_width + panel.margin, win_size.y - panel.controls.getGlobalBounds().size.y - panel.margin});
}

void Game::handleEvents(sf::RenderWindow &window)
{
    while (const std::optional<sf::Event> event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        else if (event->is<sf::Event::Resized>())
        {
            win_size = window.getSize();
            sf::View view(sf::FloatRect({0.f, 0.f}, sf::Vector2f(win_size)));
            window.setView(view);

            updatePanelSize();
            centerGrid(true);

            // doesnt scale or move the grid
            /*
            sf::FloatRect global_bounds = grid_transform.transformRect(grid.getGrid().getBounds());
            seen_center.x = (((win_size.x - panel.pnl_width) / 2) - (global_bounds.position.x)) / curr_scale;
            seen_center.y = ((win_size.y / 2) - global_bounds.position.y) / curr_scale;

            if (seen_center.x > width_cells)
            {
                grid_transform.translate({seen_center.x - width_cells, 0});
                seen_center.x = width_cells;
            }
            if (seen_center.x < 0)
            {
                grid_transform.translate({-(seen_center.x - width_cells), 0});
                seen_center.x = 0;
            }
            if (seen_center.y > height_cells)
            {
                grid_transform.translate({0, seen_center.y - height_cells});
                seen_center.y = height_cells;
            }
            if (seen_center.y < 0)
            {
                grid_transform.translate({0, -(seen_center.y - height_cells)});
                seen_center.y = 0;
            }
            */
        }
        /*
        else if (auto *mouse = event->getIf<sf::Event::MouseWheelScrolled>())
        {
            mouse->delta;
        }
        */
        else if (event->is<sf::Event::MouseEntered>())
        {
            mouse_in_window = true;
        }
        else if (event->is<sf::Event::MouseLeft>())
        {
            mouse_in_window = false;
        }
        else if (auto *mouse = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouse->button == sf::Mouse::Button::Left)
            {
                sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
                if (panel.start_button.getGlobalBounds().contains(sf::Vector2f({float(mouse_pos.x), float(mouse_pos.y)})))
                {
                    panel.start_button.clicked();
                }
                else if (panel.stop_button.getGlobalBounds().contains(sf::Vector2f({float(mouse_pos.x), float(mouse_pos.y)})))
                {
                    panel.stop_button.clicked();
                }
                else if (panel.clear_button.getGlobalBounds().contains(sf::Vector2f({float(mouse_pos.x), float(mouse_pos.y)})))
                {
                    panel.clear_button.clicked();
                }
                else if (panel.rand_button.getGlobalBounds().contains(sf::Vector2f({float(mouse_pos.x), float(mouse_pos.y)})))
                {
                    panel.rand_button.clicked();
                }
                else if (panel.glines_button.getGlobalBounds().contains(sf::Vector2f({float(mouse_pos.x), float(mouse_pos.y)})))
                {
                    panel.glines_button.clicked();
                }
                else if (panel.center_button.getGlobalBounds().contains(sf::Vector2f({float(mouse_pos.x), float(mouse_pos.y)})))
                {
                    panel.center_button.clicked();
                }
                else if (panel.fit_button.getGlobalBounds().contains(sf::Vector2f({float(mouse_pos.x), float(mouse_pos.y)})))
                {
                    panel.fit_button.clicked();
                }
                else if (mouse_in_window && !panel.background.getGlobalBounds().contains({float(mouse_pos.x), float(mouse_pos.y)}) && grid_transform.transformRect(grid.getGrid().getBounds()).contains({float(mouse_pos.x), float(mouse_pos.y)}))
                {
                    sf::FloatRect global_bounds = grid_transform.transformRect(grid.getGrid().getBounds());
                    grid.toggleCell({(unsigned int)((mouse_pos.x - global_bounds.position.x) / curr_scale), (unsigned int)((mouse_pos.y - global_bounds.position.y) / curr_scale)});
                }
            }
        }
        else if (auto *mouse = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouse->button == sf::Mouse::Button::Left)
            {
                sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
                if (panel.start_button.getGlobalBounds().contains(sf::Vector2f({float(mouse_pos.x), float(mouse_pos.y)})))
                {
                    panel.start_button.released();
                    startGame();
                }
                else if (panel.stop_button.getGlobalBounds().contains(sf::Vector2f({float(mouse_pos.x), float(mouse_pos.y)})))
                {
                    panel.stop_button.released();
                    stopGame();
                }
                else if (panel.clear_button.getGlobalBounds().contains(sf::Vector2f({float(mouse_pos.x), float(mouse_pos.y)})))
                {
                    panel.clear_button.released();
                    grid.clearGrid();
                }
                else if (panel.rand_button.getGlobalBounds().contains(sf::Vector2f({float(mouse_pos.x), float(mouse_pos.y)})))
                {
                    panel.rand_button.released();
                    grid.randGrid();
                }
                else if (panel.glines_button.getGlobalBounds().contains(sf::Vector2f({float(mouse_pos.x), float(mouse_pos.y)})))
                {
                    panel.glines_button.released();
                    is_grid_lines = !is_grid_lines;
                }
                else if (panel.center_button.getGlobalBounds().contains(sf::Vector2f({float(mouse_pos.x), float(mouse_pos.y)})))
                {
                    panel.center_button.released();
                    centerGrid();
                }
                else if (panel.fit_button.getGlobalBounds().contains(sf::Vector2f({float(mouse_pos.x), float(mouse_pos.y)})))
                {
                    panel.fit_button.released();
                    centerGrid(true);
                }
            }
        }
        else
        {
            panel.start_button.showDefColor();
            panel.stop_button.showDefColor();
            panel.clear_button.showDefColor();
            panel.rand_button.showDefColor();
            panel.glines_button.showDefColor();
            panel.center_button.showDefColor();
            panel.fit_button.showDefColor();
        }

        // Start/Stop game via spacebar
        if (auto *keyboard = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyboard->code == sf::Keyboard::Key::Space)
            {
                isPlaying() ? stopGame() : startGame();
            }
            else if (keyboard->code == sf::Keyboard::Key::C)
            {
                stopGame();
                grid.clearGrid();
            }
            else if (keyboard->code == sf::Keyboard::Key::R)
            {
                grid.randGrid();
            }
            else if (keyboard->code == sf::Keyboard::Key::T)
            {
                is_grid_lines = !is_grid_lines;
            }
            else if (keyboard->code == sf::Keyboard::Key::N)
            {
                centerGrid();
            }
            else if (keyboard->code == sf::Keyboard::Key::F)
            {
                centerGrid(true);
            }
        }
    }

    // per frame actions (moving/scaling)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        sf::FloatRect global_bounds = grid_transform.transformRect(grid.getGrid().getBounds());

        grid_transform.translate({-move_per_frame, 0});
        seen_center.x = (((win_size.x - panel.pnl_width) / 2) - (global_bounds.position.x)) / curr_scale;
        if (seen_center.x > width_cells)
        {
            seen_center.x = width_cells;
            grid_transform.translate({move_per_frame, 0});
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        sf::FloatRect global_bounds = grid_transform.transformRect(grid.getGrid().getBounds());

        grid_transform.translate({move_per_frame, 0});
        seen_center.x = (((win_size.x - panel.pnl_width) / 2) - (global_bounds.position.x)) / curr_scale;

        if (seen_center.x < 0)
        {
            seen_center.x = 0;
            grid_transform.translate({-move_per_frame, 0});
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        sf::FloatRect global_bounds = grid_transform.transformRect(grid.getGrid().getBounds());

        /*
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
        {
            move_per_frame += move_per_mpf;
        }
        else
        */

        grid_transform.translate({0, move_per_frame});
        seen_center.y = ((win_size.y / 2) - global_bounds.position.y) / curr_scale;
        if (seen_center.y < 0)
        {
            seen_center.y = 0;
            grid_transform.translate({0, -move_per_frame});
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    {
        sf::FloatRect global_bounds = grid_transform.transformRect(grid.getGrid().getBounds());

        /*
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
        {
            move_per_frame -= move_per_mpf;
        }
        else
        */
        grid_transform.translate({0, -move_per_frame});
        seen_center.y = ((win_size.y / 2) - global_bounds.position.y) / curr_scale;
        if (seen_center.y > height_cells)
        {
            seen_center.y = height_cells;
            grid_transform.translate({0, move_per_frame});
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Add))
    {
        /*
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
        {
            scale_per_frame += scale_per_spf;
        }
        else
        */
        grid_transform.scale({scale_per_frame, scale_per_frame}, seen_center);
        curr_scale *= scale_per_frame;
        move_per_frame = move_per_frame_multiplier / curr_scale;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Subtract))
    {
        /*
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
        {
            scale_per_frame -= scale_per_spf;
        }
        else
        */
        grid_transform.scale({1 / scale_per_frame, 1 / scale_per_frame}, seen_center);
        curr_scale = curr_scale / scale_per_frame;
        move_per_frame = move_per_frame_multiplier / curr_scale;
    }
}
