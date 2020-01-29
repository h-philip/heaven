/**
 * @file helper.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for helper functions in Character namespace.
 * @version 0.1
 * @date 2019-12-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "character_helper.h"

namespace Characters {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* typetoName() */
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string Helper::typeToName(Type t) {
  switch (t) {
    case Type::Player:
      return "player";
      break;

    case Type::Shooter:
      return "shooter";
      break;

    case Type::Walker:
      return "walker";
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
  if (name == "player")
    return Type::Player;
  else if (name == "shooter")
    return Type::Shooter;
  else if (name == "walker")
    return Type::Walker;
  else {
    std::cerr << "Error: Name has no type: " << name << std::endl;
    return Type::Unknown;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* isEnemy() */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool Helper::isEnemy(Type t) {
  return (t == Type::Shooter || t == Type::Walker);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getEnemyType() */
///////////////////////////////////////////////////////////////////////////////////////////////////
EnemyType Helper::getEnemyType(Type t) {
  switch (t) {
    case Type::Shooter:
      return EnemyType::Shooter;
      break;

    case Type::Walker:
      return EnemyType::Walker;
      break;

    default:
      return EnemyType::Unknown;
      break;
  }
}
}  // namespace Characters