/**
 * @file button_factory.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for ButtonFactory class.
 * @version 0.1
 * @date 2020-01-26
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "button_factory.h"

namespace Grounds {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* createButton() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Button* Grounds::ButtonFactory::createButton(ButtonType t,
                                             const sf::Vector2f& position) {
  switch (t) {
    case ButtonType::NetworkButton:
      return new NetworkButton(position);
      break;

    case ButtonType::StartButton:
      return new StartButton(position);
      break;

    case ButtonType::BuyableButton:  // nullptr
    default:
      return nullptr;
      break;
  }
}
}  // namespace Grounds