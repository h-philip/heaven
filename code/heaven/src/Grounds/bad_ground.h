/**
 * @file bad_ground.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for BadGround class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "ground.h"

namespace Grounds {

/** \class BadGround
 * @brief Class for ground which will hurt the Player on contatct.
 */
class BadGround : public Ground {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:

  static sf::Texture base_texture; /**< Texture for all bad grounds */
  static const int
      DAMAGE; /**< The damage dealt to Characters on contact */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 protected:

  std::map<const Characters::Character*, sf::Clock>
      damage_clock; /**< Measures the time passed since last time damage was
                       dealt to player */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:

  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Bad Ground object.
   *
   * @param rect Position and size.
   */
  BadGround(const sf::FloatRect& rect) : Ground(rect) {
    if (this->base_texture.getSize().x == 0) {
      // Only load once to static
      if (!this->base_texture.loadFromFile(
              "Ressources/Images/Grounds/Spikes.png"))
        exit(1);
      this->base_texture.setRepeated(true);
    }

    // Base
    this->sprites.push_back(new 
        sf::Sprite(this->base_texture,
                   sf::IntRect(0, 0, (int)rect.width, (int)rect.height)));
    this->sprites[0]->setPosition(rect.left, rect.top);
  }

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /**
   * @brief Returns the name of this sub class.
   *
   * @return std::string bad_ground
   */
  std::string name() const override { return std::string("bad_ground"); }

  virtual bool canCollide(const Characters::Character* c) const override;

  virtual void contact(Characters::Character* c, float factor) override;
};
}  // namespace Grounds