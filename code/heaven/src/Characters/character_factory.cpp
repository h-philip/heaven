/**
 * @file character_factory.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Characters::Factory class.
 * @version 0.1
 * @date 2019-12-08
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "character_factory.h"

namespace Characters {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* createCharacter() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Character* Factory::createCharacter(Type t,
                                    const sf::Vector2f& position) const {
  if (Helper::isEnemy(t))
    return createEnemy(t, position);
  else
    switch (t) {
      case Type::Player:
        return new Player(position);
        break;

      default:
        return nullptr;
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* createEnemy() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Enemy* Factory::createEnemy(EnemyType t, const sf::Vector2f& position) const {
  switch (t) {
    case EnemyType::Walker:
      return new Walker(position, this->players, this->grounds, this->window);
      break;

    case EnemyType::Shooter:
      return new Shooter(position, this->players);
      break;

    default:
      return nullptr;
      break;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* createEnemy() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Enemy* Factory::createEnemy(Type t, const sf::Vector2f& position) const {
  if (!Helper::isEnemy(t))
    return nullptr;
  else
    return createEnemy(Helper::getEnemyType(t), position);
}
}  // namespace Characters