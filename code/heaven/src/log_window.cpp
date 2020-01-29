/**
 * @file log_window.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for LogWindow class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "log_window.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Static Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string LogWindow::FONT_FILE = "Ressources/Gravity-Book.otf";

///////////////////////////////////////////////////////////////////////////////////////////////////
/* update() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void LogWindow::update() {
  sf::Event event;
  while (this->window->pollEvent(event)) {
    switch (event.type) {
      case sf::Event::Resized: {
        sf::View view = this->window->getView();
        view.reset(sf::FloatRect(0, 0, (float)this->window->getSize().x,
                                 (float)this->window->getSize().y));
        this->window->setView(view);
        break;
      }

      case sf::Event::Closed:
        this->window->close();
        break;

      default:
        break;
    }
  }

  this->window->clear(sf::Color::White);

  std::string text_string;
  for (std::string line : this->text_lines) {
    text_string.append(line + "\n");
  }
  this->text_lines.clear();
  this->text.setString(text_string);
  this->window->draw(this->text);
  this->window->display();
}
