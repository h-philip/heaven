/**
 * @file collectable_factory.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Collectables::Factory class.
 * @version 0.1
 * @date 2019-12-08
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "collectable_factory.h"

namespace Collectables {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* createCollectable() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Collectable* Factory::createCollectable(Type t,
                                        const sf::Vector2f& position) const {
  switch (t) {
    case Type::Coin:
      return new Coin(position);
      break;

    case Type::Heart:
      return new Heart(position);
      break;

    default:
      return nullptr;
      break;
  }
}
}  // namespace Collectables