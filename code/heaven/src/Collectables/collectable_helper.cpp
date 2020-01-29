/**
 * @file collectable_helper.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Collectables::Helper class
 * @version 0.1
 * @date 2019-12-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "collectable_helper.h"

namespace Collectables {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* typeToName() */
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string Helper::typeToName(Type t) {
  switch (t) {
    case Type::Coin:
      return "coin";
      break;

    case Type::Heart:
      return "heart";
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
  if (name == "coin")
    return Type::Coin;
  else if (name == "heart")
    return Type::Heart;
    else {
      std::cerr << "Error: Name has no type: " << name << std::endl;
      return Type::Unknown;
    }
}
}  // namespace Collectables