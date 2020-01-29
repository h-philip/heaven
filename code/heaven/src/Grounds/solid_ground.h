/**
 * @file solid_ground.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for SolidGround class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "ground.h"

namespace Grounds {

/** \class SolidGround
 * @brief Class for ground which won't hurt the Player on contatct.
 */
class SolidGround : public Ground {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 protected:
  static sf::Texture base_texture, /**< Background texture for all grounds */
      extra_texture;               /**< Grass texture for all grounds */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Solid Ground object.
   *
   * @param rect FloatRect with position and size of ground.
   */
  SolidGround(const sf::FloatRect& rect) : Ground(rect) {
    if (this->base_texture.getSize().x == 0) {
      // Only load once to static
      if (!this->base_texture.loadFromFile(
              "Ressources/Images/Grounds/Base.png"))
        exit(1);

      this->base_texture.setRepeated(true);
      if (!this->extra_texture.loadFromFile(
              "Ressources/Images/Grounds/Grass.png"))
        exit(1);
      this->extra_texture.setRepeated(true);
    }

    // Base (dirt)
    this->sprites.push_back(
        new sf::Sprite(this->base_texture,
                       sf::IntRect(0, 0, (int)rect.width, (int)rect.height)));
    this->sprites[0]->setPosition(rect.left, rect.top);

    // Top (grass)
    this->sprites.push_back(new sf::Sprite(
        this->extra_texture,
        sf::IntRect(0, 0, (int)rect.width,
                    rect.height < 50.f ? (int)rect.height : 50)));
    this->sprites[1]->setPosition(rect.left, rect.top);
  }

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /**
   * @brief Method to run when contacting with Character.
   *
   * @param c Pointer to the Character
   * @param factor The time factor.
   */
  void contact(Characters::Character* c, float factor) {}

  /**
   * @brief Returns the name of this sub class.
   *
   * @return std::string solid_ground
   */
  std::string name() const override { return std::string("solid_ground"); }
};
}  // namespace Grounds