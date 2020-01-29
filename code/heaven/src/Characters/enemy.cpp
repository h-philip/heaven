/**
 * @file enemy.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Enemy class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "enemy.h"

#include "../Network/host.h"

namespace Characters {
///////////////////////////////////////////////////////////////////////////////////////////////////
/* Enemy() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Enemy::Enemy(sf::Vector2f position, std::vector<Player*>& players)
    : Character(position), all_player(players) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getNearestPlayer() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Player* Enemy::getNearestPlayer() {
  Player* nearest = nullptr;
  for (Player* p : this->all_player) {
    // Skip dead
    if (p->getHP() == 0) continue;

    // If nearest is nullptr set directly
    if (!nearest) {
      nearest = p;
      continue;
    }

    // Check if more close to this
    if (getDistanceTo(p->getPosition()) <
        getDistanceTo(nearest->getPosition())) {
      nearest = p;
    }
  }

  return nearest;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setState() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Enemy::setState(State state) {
  Character::setState(state);

  // Apply to network
  if (this->is_local && Network::NetworkManager::getInstance()) {
    if (Network::NetworkManager::getInstance()->isHost()) {
      ((Network::Host*)Network::NetworkManager::getInstance())
          ->doEnemyChangeState(this);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setHorizontalMovement() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Enemy::setHorizontalMovement(int value) {
  Character::setHorizontalMovement(value);

  // Apply to network
  if (this->is_local && Network::NetworkManager::getInstance()) {
    if (Network::NetworkManager::getInstance()->isHost()) {
      ((Network::Host*)Network::NetworkManager::getInstance())
          ->doEnemyHorizontalDir(this);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* die() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Enemy::die() {
  Character::die();
  // Apply to network
  if (this->is_local && Network::NetworkManager::getInstance()) {
    if (Network::NetworkManager::getInstance()->isHost()) {
      ((Network::Host*)Network::NetworkManager::getInstance())
          ->doEnemyDie(this);
    }
  }
}
}  // namespace Characters
