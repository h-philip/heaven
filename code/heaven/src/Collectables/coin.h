/**
 * @file coin.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Coin class
 * @version 0.1
 * @date 2019-12-12
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "collectable.h"

namespace Collectables {

/** \class Coin
 *  @brief Class for collectible coins in the world.
 *
 *
 */
class Coin : public Collectable {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  static const std::string
      TEXTURE_FILE; /**< File with textures for all coins */
  static const sf::Vector2i
      SIZE; /**< size of one coin inside the sprite file */
  static const std::string SOUND_FILE; /**<  Sound file for collecting */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 private:
  static sf::Texture texture;          /**< Texture for all coins */
  static sf::SoundBuffer sound_buffer; /**< Sound buffer for all coins */

  sf::Sound sound; /**< Sound for this coin */

  uint8_t animation_counter = 0; /**< Counter for "rotating" coin */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Coin object.
   *
   * @param position The position of the coin
   */
  Coin(const sf::Vector2f& position) : Collectable(position) {
    if (texture.getSize().x == 0)
      if (!texture.loadFromFile(TEXTURE_FILE)) exit(1);

    if (this->sound_buffer.getDuration().asMilliseconds() == 0)
      if (!this->sound_buffer.loadFromFile(SOUND_FILE)) exit(1);
    this->sound.setBuffer(this->sound_buffer);
    this->sound.setPosition((position.x - 960) * .5f, position.y, 0);
    this->sound.setMinDistance(1000);
    this->sound.setAttenuation(3);
    this->sound.setVolume(80);

    setTexture(texture);
    setTextureRect(sf::IntRect(0, 0, SIZE.x, SIZE.y));
    setScale(2.5, 2.5);
  }

  /**
   * @brief Destroys the Coin object.
   *
   */
  ~Coin() {}

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  Type getType() override { return Type::Coin; }

  void toggleAnimation() override;

  void collect() override;

  bool canBeDeleted() override;
};
}  // namespace Collectables
