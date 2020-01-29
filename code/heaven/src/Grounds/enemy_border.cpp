/**
 * @file enemy_border.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for EnemyBorder class
 * @version 0.1
 * @date 2019-12-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "enemy_border.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/* move() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Grounds::EnemyBorder::move(const sf::Vector2f& offset) {
  Ground::move(offset);
  if (this->editor_rect) this->editor_rect->move(offset);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* enableDraw() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Grounds::EnemyBorder::enableDraw() {
  this->editor_rect = new sf::RectangleShape(
      sf::Vector2f(getGlobalBounds().width, getGlobalBounds().height));
  this->editor_rect->setPosition(getGlobalBounds().left, getGlobalBounds().top);
  this->editor_rect->setFillColor(sf::Color(200, 50, 50, 50));
}
