/**
 * @file log_window.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for LogWindow class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <math.h>

#include <SFML/Graphics.hpp>

#include "world.h"

/** \class LogWindow
 *  @brief Class for a new window for debug and log things.
 */
class LogWindow {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  static const std::string FONT_FILE; /**< Font file for text */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 private:
  sf::RenderWindow* window; /**< Window to use */
  World* world;             /**< Active world */
  sf::Font font;            /**< Font for text */
  sf::Text text;            /**< The log text */

  std::vector<std::string> text_lines; /**< Text to be displayed at next draw */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Log Window object.
   *
   * @param world Pointer to the world object.
   */
  LogWindow(World* world) : world(world) {
    this->window =
        new sf::RenderWindow(sf::VideoMode(900, 600), "Heaven - Log",
                             sf::Style::Default, sf::ContextSettings());

    this->font.loadFromFile(this->FONT_FILE);
    this->text.setFont(this->font);
    this->text.setCharacterSize(20);
    this->text.setPosition(0, 0);
    this->text.setFillColor(sf::Color::Black);
  }

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /**
   * @brief Updates this window.
   *
   */
  void update();

  /**
   * @brief Adds a line of text to be diplayed at next draw.
   * 
   * @param line The text line.
   */
  void addLine(std::string line) { this->text_lines.push_back(line); }

  void close() { this->window->close(); }

  bool isOpen() const { return this->window->isOpen(); }
};