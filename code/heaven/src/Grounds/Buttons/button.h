/**
 * @file button.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Button class.
 * @version 0.1
 * @date 2019-11-29
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "button_helper.h"
#include "../../Characters/player.h"
#include "../ground.h"

namespace Grounds {
/** \class Button
 *  @brief Base class for all buttons
 */
class Button : public Ground {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  static const sf::Keyboard::Key INTERACT_KEY = sf::Keyboard::Key::Enter; /**< The key used to press the button */
  static const sf::Int32 CLICK_TIMEOUT = 100; /**< Timeout between multiple button presses */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 protected:
  sf::Clock timeout_clock; /**< Clock to prevent excessive button clicking */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Button object.
   *
   * @param pos Position of the button
   */
  Button(const sf::Vector2f& pos)
      : Ground(sf::FloatRect(pos.x, pos.y, 100, 100)) {}

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /**
   * @brief Gets the type of this button.
   * 
   * @return ButtonType The type.
   */
  virtual ButtonType getType() const = 0;

  virtual std::string name() const override;

  virtual bool canCollide(const Characters::Character* c) const override {
    return true;
  };

  virtual void contact(Characters::Character* c, float factor) override;

  /**
   * @brief The action to be performed when a player interacts with this button.
   *
   * @param p The player who interacted.
   * @param factor The time factor.
   */
  virtual void doAction(Characters::Player* p, float factor) = 0;
};
}  // namespace Grounds