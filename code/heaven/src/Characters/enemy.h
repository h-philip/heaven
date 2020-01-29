/**
 * @file enemy.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Enemy class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "character.h"
#include "player.h"

namespace Characters {

/** \class Enemy
 *  @brief This is the base class for all types of enemys.
 */
class Enemy : public Character {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 protected:
  std::vector<Player*>&
      all_player; /**< A reference to a vector with all players. */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //
 public:
  /**
   * @brief Constructs a new Enemy object.
   *
   * @param position The new position.
   * @param players A vector with all player which can be attacked by this
   *                Enemy.
   */
  Enemy(sf::Vector2f position, std::vector<Player*>& players);

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /** @fn getNearestPlayer
   * Return a pointer to the nearest player.
   *
   * @return The nearest player.
   */
  Player* getNearestPlayer();

  void setState(State state) override;

  void setHorizontalMovement(int value) override;

  virtual void die() override;
};
}  // namespace Characters