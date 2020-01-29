/**
 * @file buyable_helper.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for helper functions in Buyables namespace.
 * @version 0.1
 * @date 2019-12-16
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "buyable_helper.h"

namespace Buyables {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* typetoName() */
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string Helper::typeToName(Type t) {
  switch (t) {
    case Type::JumpBoost:
      return "jump_boost";
      break;

    case Type::Immortality:
      return "immortality";
      break;

    default:
      std::cerr << "Error: Type has no name: " << std::to_string((uint8_t)t)
                << std::endl;
      return "unknown";
      break;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* nameToType() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Type Helper::nameToType(std::string name) {
  if (name == "jump_boost")
    return Type::JumpBoost;
  else if (name == "immortality")
    return Type::Immortality;
  else {
    std::cerr << "Error: Name has no type: " << name << std::endl;
    return Type::Unknown;
  }
}
}  // namespace Buyables