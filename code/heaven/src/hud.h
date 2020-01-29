/**
 * @file hud.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for HUD class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "SFML/Graphics.hpp"

/**
 * @brief Class for displaying hud.
 *
 * Points and lives.
 */
class HUD {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  const std::string
      HEART_FILE =
          "Ressources/Images/Characters/Player/hearts.png", /**< File with
                                                               hearts */
      FONT_FILE = "Ressources/PressStart2P.ttf", /**< Font file for text */
      COINS_FILE = "Ressources/Images/coin.png"; /**< font with coins */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 private:
  sf::Font point_font,       /**< Font for points */
      coins_font;            /**< Font for coin counter */
  sf::Text points_text,      /**< Text for points */
      coins_text;            /**< Text for coin conter */
  sf::RenderWindow* window;  /**< Current window */
  sf::Texture heart_texture, /**< Texture for hearts */
      coin_texture;          /**< Texture for coins */
  sf::Sprite heart,          /**< Sprite for displayed heart */
      coin;                  /**< Sprite for displayed coin */

  float max_points, /**< Max points measured */
      max_hp;       /**< Max hp possible */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new HUD object.
   *
   * @param window Pointer to the window.
   * @param max_hp The max lives to display.
   */
  HUD(sf::RenderWindow* window, float max_hp)
      : window(window), max_points(200), max_hp(max_hp) {
    // Points
    if (!this->point_font.loadFromFile(this->FONT_FILE)) exit(1);

    points_text.setFont(this->point_font);
    points_text.setCharacterSize(30);
    points_text.setFillColor(sf::Color::White);
    this->points_text.setPosition(window->getSize().x * .92f,
                                  window->getSize().y * .03f);

    // HP
    if (!this->heart_texture.loadFromFile(this->HEART_FILE)) exit(1);

    this->heart = sf::Sprite(heart_texture, sf::IntRect(0, 0, 36, 30));
    this->heart.setPosition(window->getSize().x * .02f,
                            window->getSize().y * .02f);

    // Coins
    if (!this->coin_texture.loadFromFile(this->COINS_FILE)) exit(1);

    this->coin = sf::Sprite(coin_texture, sf::IntRect(0, 0, 16, 16));
    this->coin.setScale(2, 2);
    this->coin.setColor(sf::Color(220, 220, 220, 255));
    this->coin.setPosition(window->getSize().x * .177f,
                           window->getSize().y * .02f);

    if (!this->coins_font.loadFromFile(this->FONT_FILE)) exit(1);

    coins_text.setFont(this->coins_font);
    coins_text.setCharacterSize(25);
    coins_text.setFillColor(sf::Color::Black);
    coins_text.setFillColor(sf::Color::White);
    this->coins_text.setPosition(window->getSize().x * .195f,
                                 window->getSize().y * .025f);
    this->coins_text.setString("x0");
  }

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /**
   * @brief Redraws the points.
   *
   * @param vertical_movement The vertical movement of the view.
   * @param height The height of the player.
   */
  void drawPoints(float vertical_movement, float height);

  /**
   * @brief Redraws the lives.
   *
   * @param vertical_movement The vertical movement of the view.
   * @param hearts Lives to draw.
   */
  void drawHP(float vertical_movement, int hearts);

  /**
   * @brief Redraws the collected coins.
   *
   * @param vertical_movement The vertical movement of the view.
   * @param coins Amount of coins to draw.
   */
  void drawCoins(float vertical_movement, int coins);
};