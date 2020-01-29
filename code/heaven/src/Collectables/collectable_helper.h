/**
 * @file collectable_helper.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Collectables::Helper class
 * @version 0.1
 * @date 2019-12-08
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <iostream>
#include <string>

namespace Collectables {

/**
 * @brief All types of collectables.
 *
 */
enum class Type : uint8_t { Unknown, Coin, Heart };

/**
 * @brief Class with helper methods for Collectables.
 * 
 */
class Helper {
 public:
  /**
   * @brief Converts a type to its name.
   *
   * @param t The type to convert.
   * @return std::string The corresponding name.
   */
  static std::string typeToName(Type t);

  /**
   * @brief Converts a name to its type.
   *
   * @param name The name to convert.
   * @return Type The corresponding type.
   */
  static Type nameToType(std::string name);
};
}  // namespace Collectables