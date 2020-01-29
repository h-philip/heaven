/**
 * @file walker.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Walker class
 * @version 0.1
 * @date 2019-12-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "walker.h"

namespace Characters {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Static Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string Walker::SPRITE_FILE =
    "Ressources/Images/Characters/spritesheet_jumper.png";
const float Walker::HORIZONTAL_MOVE_SPEED = 20.f;

sf::Texture Walker::texture;

///////////////////////////////////////////////////////////////////////////////////////////////////
/* update() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Walker::update(float factor) {
  Enemy::update(factor);
  for (auto p : this->all_player) {
    if (p->getGlobalBounds().intersects(getGlobalBounds())) {
      if (this->damage_clocks.count(p) == 0) {
        this->damage_clocks[p] = sf::Clock();
        p->takeDamage();
      } else if (this->damage_clocks[p].getElapsedTime().asMilliseconds() >
                 500) {
        this->damage_clocks[p].restart();
        p->takeDamage();
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* horizontalMove() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Walker::horizontalMove(float factor) {
  Enemy::horizontalMove(factor);
  std::vector<Grounds::Ground*> colliding;
  sf::FloatRect new_rect = getHorizontalMove(factor);
  if (new_rect.left < 0)
    setMoveDirRight(0);
  else if (new_rect.left + new_rect.width > this->window->getSize().x)
    setMoveDirLeft(0);
  else if (canMoveTo(new_rect, this->all_grounds, &colliding, nullptr))
    return;
  else {
    if (this->horizontal_movement > 0)
      setMoveDirLeft(0);
    else if (this->horizontal_movement < 0)
      setMoveDirRight(0);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* toggleAnimation() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Walker::toggleAnimation() {
  /* last_animation:
     0 = idle
     1 = walk
   */

  // Walk
  if (this->last_animation == 1) animateWalk();
  // Idle
  else
    animateIdle();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* animateIdle() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Walker::animateIdle() { setTextureRect(sf::IntRect(814, 1417, 90, 155)); }

///////////////////////////////////////////////////////////////////////////////////////////////////
/* animateWalk */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Walker::animateWalk() {
  switch (this->animation_counter) {
    case 0:
      setTextureRect(sf::IntRect(704, 1256, 120, 159));
      setOrigin(60, 77);
      break;

    case 1:
      setTextureRect(sf::IntRect(812, 296, 90, 155));
      setOrigin(45, 77);
      break;
  }
  this->animation_counter = (this->animation_counter + 1) % 2;
}
}  // namespace Characters