/**
 * @file buyable_factory.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Buyables::Factory class.
 * @version 0.1
 * @date 2019-12-16
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "buyable_factory.h"

#include "../Characters/character.h"

namespace Buyables {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* createBuyables() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Buyable* Factory::createBuyable(Type t) const {
  switch (t) {
    case Type::JumpBoost:
      return new JumpBoost();
      break;

    case Type::Immortality:
      return new Immortality();
      break;

    default:
      return nullptr;
      break;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* createBuyables() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Buyable* Factory::createBuyable(Type t,
                                Characters::Character* character) const {
  Buyable* b = createBuyable(t);
  if (character) b->assignTo(character);
  return b;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getPriceOf() */
///////////////////////////////////////////////////////////////////////////////////////////////////
int Factory::getPriceOf(Type t) {
  switch (t) {
    case Type::JumpBoost:
      return JumpBoost::getPrice();
      break;

    case Type::Immortality:
      return Immortality::getPrice();
      break;

    default:
      std::cerr << "Error while getting price of buyable: "
                << "Unknown buyable type: " << std::to_string((uint8_t)t)
                << std::endl;
      return 0;
      break;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getTexture() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Texture& Factory::getTexture(Type t) {
  switch (t) {
    case Type::JumpBoost:
      return JumpBoost::getTexture();
      break;

    case Type::Immortality:
      return Immortality::getTexture();
      break;

    case Type::Unknown:
    default:
      std::cerr << "Error while getting texture of buyable: "
                << "Unknown buyable type: " << std::to_string((uint8_t)t)
                << std::endl;
      exit(1);
      break;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getNextType() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Type Factory::getNextType(Type t) {
  switch (t) {
    case Type::JumpBoost:
      return Type::Immortality;
      break;

    case Type::Immortality:

    default:
      return Type::JumpBoost;
      break;
  }
}
}  // namespace Buyables