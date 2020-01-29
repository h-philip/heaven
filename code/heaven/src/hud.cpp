/**
 * @file hud.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for HUD class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "hud.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/* drawPoints() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void HUD::drawPoints(float vertical_movement, float height) {
  this->points_text.move(0, vertical_movement * -1);
  if (height > this->max_points) this->max_points = height;
  std::string string = std::to_string((int)max_points / 100);
  uint8_t length = (uint8_t)string.length();
  for (uint8_t i = 4; i > length; i--) {
    string = std::string("0").append(string);
  }
  this->points_text.setString(string);
  this->window->draw(this->points_text);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* drawHP() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void HUD::drawHP(float vertical_movement, int hearts) {
  this->heart.move(0, vertical_movement * -1);
  sf::Vector2f base_position = this->heart.getPosition();

  // HP left
  uint8_t i;
  for (i = 0; i < hearts; i++) {
    this->heart.move(40, 0);
    this->window->draw(heart);
  }

  // HP already lost
  if (i < this->max_hp) {
    // Set to empty heart
    this->heart.setTextureRect(sf::IntRect(72, 0, 36, 30));
    for (; i < this->max_hp; i++) {
      this->heart.move(40, 0);
      this->window->draw(heart);
    }

    // Reset to full heart
    this->heart.setTextureRect(sf::IntRect(0, 0, 36, 30));
  }

  this->heart.setPosition(base_position);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* drawCoins() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void HUD::drawCoins(float vertical_movement, int coins) {
  this->coins_text.move(0, vertical_movement * -1);
  this->coins_text.setString("x" + std::to_string(coins));
  this->coin.move(0, vertical_movement * -1);
  this->window->draw(coin);
  this->window->draw(this->coins_text);
}
