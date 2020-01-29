/**
 * @file portal.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Poral class.
 * @version 0.1
 * @date 2019-11-22
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "portal.h"

namespace Grounds {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Static Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string Portal::SOUND_FILE = "Ressources/Sounds/teleport.wav";
sf::Texture Portal::base_texture;
sf::SoundBuffer Portal::sound_buffer;

///////////////////////////////////////////////////////////////////////////////////////////////////
/* contact() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Portal::contact(Characters::Character* c, float factor) {
  sf::Int32 time = this->timeout_clock.getElapsedTime().asMilliseconds();
  if (time < 100) return;  // If less than 100 ms since last contact -> skip

  this->timeout_clock.restart();
  c->setFixedMove(sf::Vector2f(global_bounds.left, global_bounds.top) -
                  c->getPosition() + this->destination);
  this->sound.play();
#ifdef PORTAL_DEBUG
  std::cout << "Portal Debug: Portal::contact() was called!" << std::endl;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* draw() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Portal::draw(sf::RenderWindow* window) const {
  Ground::draw(window);
  if (this->draw_dest) window->draw(this->dest_line, 2, sf::Lines);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* move() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Portal::move(const sf::Vector2f& offset) {
  Ground::move(offset);
  this->destination.x += offset.x;
  this->destination.y += offset.y;
  this->dest_line[0].position += offset;
  this->dest_line[1].position += offset;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setPosition() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Portal::setPosition(const sf::Vector2f& position) {
  sf::Vector2f prev_pos;
  prev_pos.x = getGlobalBounds().left;
  prev_pos.y = getGlobalBounds().top;

  sf::Vector2f offset = position - prev_pos;
  move(offset);
}

}  // namespace Grounds