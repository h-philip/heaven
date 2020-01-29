/**
 * @file button.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Button class.
 * @version 0.1
 * @date 2019-11-29
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "button.h"

namespace Grounds {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* name() */
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string Button::name() const { return ButtonHelper::typeToName(getType()); }

///////////////////////////////////////////////////////////////////////////////////////////////////
/* contact() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Button::contact(Characters::Character* c, float factor) {
  if (c->getType() == Characters::Type::Player &&
      sf::Keyboard::isKeyPressed(Button::INTERACT_KEY))
    doAction((Characters::Player*)c, factor);
}
}  // namespace Grounds