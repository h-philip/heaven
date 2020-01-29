/**
 * @file button_factory.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for ButtonFactory class.
 * @version 0.1
 * @date 2020-01-26
 *
 * @copyright Copyright (c) 2020
 *
 */
#pragma once

#include <SFML/Graphics.hpp>

#include "buyable_button.h"
#include "network_button.h"
#include "start_button.h"

namespace Grounds {

/** \class ButtonFactory
 *  @brief Factory for buttons.
 *
 *
 */
class ButtonFactory {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Button Factory object.
   * 
   */
  ButtonFactory() {}

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /**
   * @brief Creates a new button if the type is not BuyableButton.
   *
   * @param t The type of the new button.
   * @param position The position to place.
   * @return Button* The new button.
   */
  Button* createButton(ButtonType t, const sf::Vector2f& position);
};
}  // namespace Grounds