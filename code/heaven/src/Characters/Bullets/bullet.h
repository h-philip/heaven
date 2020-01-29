/**
 * @file bullet.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Bullet class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

#include "../player.h"

/**
 * @brief Namespace for all bullets that can be shot.
 *
 */
namespace Bullets {

/** \class Bullet
 *  @brief Base class for all bullet types.
 */
class Bullet : public sf::Sprite {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  static const std::string
      TEXTURE_FILE; /**< The path to the file of the texture for this bullet */
  static const float
      MAX_MOVE_DISTANCE; /**< The maximum distance this bullet can move */
  static const uint8_t DAMAGE =
      1; /**< The damage that this Bullet deals per hit */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 protected:
  sf::Vector2f movement; /**< The movement direction and speed */
  float moved_distance;  /**< The distance moved for destroying
                         after specified distance */
  std::vector<Characters::Player*>&
      all_player;         /**< A reference to a vector
                          with all players that can be hit */
  bool destroyed = false; /**< Set this to true to tell the shooter that this
                             Bullet should be deleted */

  static sf::Texture texture; /**< The texture for this bullet */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Bullet object.
   *
   * @param start The start position.
   * @param movement The movement direction and speed.
   * @param players A vector with all Players which can be shot.
   */
  Bullet(sf::Vector2f start, sf::Vector2f movement,
         std::vector<Characters::Player*>& players)
      : movement(movement), moved_distance(0), all_player(players) {
    if (this->texture.getSize().x == 0)
      if (!this->texture.loadFromFile(TEXTURE_FILE)) {
        std::cerr << "Could not load file: " << TEXTURE_FILE << std::endl;
        exit(1);
      }
    setTexture(this->texture);
    setTextureRect(sf::IntRect(0, 0, 31, 31));

    // Set origin to middle of texture
    sf::Vector2f origin;
    origin.x = (float)this->texture.getSize().x * .5f;
    origin.y = (float)this->texture.getSize().y * .5f;
    setOrigin(origin);

    setPosition(start);
  }

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /** @fn update
   * Updates everything needed.
   *
   * @param factor The factor which determining how much time (relative to a
   * second) has passed
   */
  virtual void update(float factor);

  /**
   * @brief Draws this bullet.
   *
   * @param window A pointer to the window.
   */
  virtual void draw(sf::RenderWindow* window);

  // ----------------------------------------- //
  /* Getter / Setter */
  // ----------------------------------------- //

  virtual bool isDestroyed() const { return this->destroyed; }
};
}  // namespace Bullets