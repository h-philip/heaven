/**
 * @file bad_ground.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief source file for BadGround class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "bad_ground.h"

namespace Grounds {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Static Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////

sf::Texture BadGround::base_texture;

///////////////////////////////////////////////////////////////////////////////////////////////////
/* canCollide() */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool BadGround::canCollide(const Characters::Character* c) const {
  return c->getType() == Characters::Type::Player;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* contact() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void BadGround::contact(Characters::Character* c, float factor) {
  if (c->getType() == Characters::Type::Player && c->getHP() > 0) {
    c->canKeepJumping(false);
    if (this->damage_clock.count(c) == 0) {
      this->damage_clock[c] = sf::Clock();
      c->takeDamage();
    } else if (this->damage_clock[c].getElapsedTime().asMilliseconds() > 500) {
      this->damage_clock[c].restart();
      c->takeDamage();
    }
  }
}
}  // namespace Grounds