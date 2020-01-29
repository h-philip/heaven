/**
 * @file character_factory.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Characters::Factory class.
 * @version 0.1
 * @date 2019-12-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#pragma once

#include "shooter.h"
#include "walker.h"

namespace Characters {

/** \class Factory
 *  @brief Factory class for creating Characters.
 *
 *
 */
class Factory {

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 private:
  const std::vector<Grounds::Ground*>& grounds; /**< Reference to vector with all grounds */
  std::vector<Player*>& players; /**< Reference to vector with all players */
  const sf::RenderWindow* window; /**< Pointer to the window */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Factory object.
   * 
   * @param grounds All grounds in the world.
   * @param players All players in the world.
   * @param window The used window.
   */
  Factory(const std::vector<Grounds::Ground*>& grounds, std::vector<Player*>& players,
          const sf::RenderWindow* window)
      : grounds(grounds), players(players), window(window) {}

  // ----------------------------------------- //
  /* Create Methods */
  // ----------------------------------------- //

  /**
   * @brief Creates a Character object.
   * 
   * @param t The type of the character to create.
   * @param position The position to start at.
   * @return Character* Pointer to the new character.
   */
  Character* createCharacter(Type t, const sf::Vector2f& position) const;

  /**
   * @brief Creates a Enemy object.
   * 
   * @param t The type of the enemy to create,
   * @param position The position to start at.
   * @return Enemy* Pointer to the new enemy.
   */
  Enemy* createEnemy(EnemyType t, const sf::Vector2f& position) const;

  /**
   * @brief Creates a Enemy object.
   * 
   * If the type is not an enemy, nullptr will be returned.
   * 
   * @param t The type of the enemy to create (as Type).
   * @param position The position to start at.
   * @return Enemy* Pointer to the new enemy.
   */
  Enemy* createEnemy(Type t, const sf::Vector2f& position) const;
};
}  // namespace Characters