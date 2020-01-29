/**
 * @file bullet.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Bullet class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "bullet.h"

namespace Bullets {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Static Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string Bullet::TEXTURE_FILE =
    "Ressources/Images/Characters/Bullets/bullet.png";
const float Bullet::MAX_MOVE_DISTANCE = 500.f;

sf::Texture Bullet::texture;

///////////////////////////////////////////////////////////////////////////////////////////////////
/* update() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Bullet::update(float factor) {
  sf::Vector2f factor_movement = factor * this->movement;
  move(factor_movement);

  // Add to moved distance
  float dist = sqrtf(pow(factor_movement.x, 2) + pow(factor_movement.y, 2));
  this->moved_distance += dist;

  if (this->moved_distance > this->MAX_MOVE_DISTANCE) {
    this->destroyed = true;
    return;
  }

  // Check collision with players
  for (auto p : this->all_player) {
    if (p->getHP() > 0 && p->getGlobalBounds().intersects(getGlobalBounds())) {
      p->takeDamage(this->DAMAGE);
      this->destroyed = true;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* draw() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Bullet::draw(sf::RenderWindow* window) { window->draw(*this); }

}  // namespace Bullets