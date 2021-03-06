/**
 * @file coin.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Coin class
 * @version 0.1
 * @date 2019-12-12
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "coin.h"

namespace Collectables {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Static Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string Coin::TEXTURE_FILE = "Ressources/Images/coin.png";
const std::string Coin::SOUND_FILE = "Ressources/Sounds/coin.wav";
const sf::Vector2i Coin::SIZE = sf::Vector2i(16, 16);

sf::Texture Coin::texture;
sf::SoundBuffer Coin::sound_buffer;

///////////////////////////////////////////////////////////////////////////////////////////////////
/* toggleAnimation() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Coin::toggleAnimation() {
  sf::IntRect rect = sf::IntRect(0, 0, SIZE.x, SIZE.y);
  rect.left += 16 * animation_counter;
  setTextureRect(rect);
  this->animation_counter = (this->animation_counter + 1) % 8;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* collect() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Coin::collect() {
  Collectable::collect();
  this->sound.play();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* canBeDelted() */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool Coin::canBeDeleted() {
  return (this->sound.getStatus() != sf::Sound::Status::Playing);
}
}  // namespace Collectables