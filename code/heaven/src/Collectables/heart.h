/**
 * @file heart.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Heart class
 * @version 0.1
 * @date 2019-12-12
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "collectable.h"

namespace Collectables {

/** \class Heart
 *  @brief Class for collectible heart in the world.
 *
 *
 */
class Heart : public Collectable {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  static const std::string TEXTURE_FILE; /**< File with texture for  hearts */
  static const sf::Vector2i
      SIZE; /**< Size of one heart inside the sprite file */
  static const std::string SOUND_FILE; /**< File with sound for hearts */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 private:
  static sf::Texture texture;          /**< Texture for all hearts */
  static sf::SoundBuffer sound_buffer; /**< Sound buffer for all hearts */

  sf::Sound sound; /**< Sound for this heart */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Heart object.
   *
   * @param position Position of the heart
   */
  Heart(const sf::Vector2f& position) : Collectable(position) {
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
    setScale(1.4f, 1.4f);
  }

  /**
   * @brief Destroys the Heart object.
   *
   */
  ~Heart() {}

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  Type getType() override { return Type::Heart; }

  void collect() override;

  bool canBeDeleted() override;
};
}  // namespace Collectables
