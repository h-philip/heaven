/**
 * @file immortality.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Immortality class
 * @version 0.1
 * @date 2020-01-15
 *
 *
 */
#include "immortality.h"

#include "../Characters/character.h"
#include "../Characters/player.h"

namespace Buyables {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Static Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string Immortality::ICON_TEXTURE =
    "Ressources/Images/Buyables/immortality.png";
const sf::Vector2f Immortality::ICON_SIZE(80, 80), Immortality::OBJECT_SIZE(50,
                                                                            50);

sf::Texture Immortality::texture;

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Immortality() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Immortality::Immortality(Characters::Character* character, uint16_t run_time)
    : Immortality(run_time) {
  assignTo(character);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* JumpBoost() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Immortality::Immortality(uint16_t run_time)
    : Buyable(getTexture()), time(run_time) {
  this->sprite.setScale(OBJECT_SIZE.x / 50, OBJECT_SIZE.y / 50);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getType() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Type Immortality::getType() const { return Type::Immortality; }

///////////////////////////////////////////////////////////////////////////////////////////////////
/* update() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Immortality::update(float factor) {
  if (!this->is_active) return;  // Do nothing if inactive
  if (this->clock.getElapsedTime().asSeconds() >= this->time)
    this->is_active = false;

  // Check if hp of character was increased and overwrite otherwise
  if (this->character->getHP() > this->base_hp)
    this->base_hp = this->character->getHP();
  else
    this->character->setHP(this->base_hp);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* assignTo() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Immortality::assignTo(Characters::Character* character) {
  Buyable::assignTo(character);
  character->addBuyable(this);
  this->is_active = true;

  this->base_hp = character->getHP();

  // Change drawing stuff
  this->sprite.setScale(ICON_SIZE.x / 50, ICON_SIZE.y / 50);

  this->clock.restart();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getTexture() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Texture& Immortality::getTexture() {
  if (texture.getSize().x == 0)
    if (!texture.loadFromFile(ICON_TEXTURE)) exit(1);
  return texture;
}
}  // namespace Buyables