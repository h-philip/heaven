/**
 * @file character_helper.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for helper functions in Characters namespace.
 * @version 0.1
 * @date 2019-12-08
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <iostream>
#include <string>

namespace Characters {

/**
 * @brief All types of character (incl. enemies).
 *
 */
enum class Type : uint8_t { Unknown, Player, Shooter, Walker };

/**
 * @brief All types of enemies (no other characters).
 *
 */
enum class EnemyType : uint8_t { Unknown, Shooter, Walker };

class Helper {
 public:
  /**
   * @brief Converts a type to a name.
   *
   * @param t The Type of the character.
   * @return std::string The type as string.
   */
  static std::string typeToName(Type t);

  /**
   * @brief Converts a name to a type.
   *
   * @param name The name of the type.
   * @return Type The type of the character.
   */
  static Type nameToType(std::string name);

  /**
   * @brief Returns whether the given type is an enemy type.
   *
   * @param t The type to check.
   * @return true When the type is an enemy type.
   * @return false Otherwise.
   */
  static bool isEnemy(Type t);
  /**
   * @brief Converts an Type to the corresponding EnemyType.
   *
   * @param t The Type.
   * @return EnemyType The EnemyType.
   */
  static EnemyType getEnemyType(Type t);
};
}  // namespace Characters