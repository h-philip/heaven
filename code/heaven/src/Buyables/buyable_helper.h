/**
 * @file buyable_helper.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for helper functions in Buyables namespace.
 * @version 0.1
 * @date 2019-12-16
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <iostream>
#include <string>

namespace Buyables {

/**
 * @brief All types of buyables.
 *
 */
enum class Type : uint8_t { Unknown, JumpBoost, Immortality };

/**
 * @brief Class with helper methods for buyables.
 * 
 */
class Helper {
 public:
  /**
   * @brief Converts a type to a name.
   *
   * @param t The Type of the buyable.
   * @return std::string The type as string.
   */
  static std::string typeToName(Type t);

  /**
   * @brief Converts a name to a type.
   *
   * @param name The name of the type.
   * @return Type The type of the buyable.
   */
  static Type nameToType(std::string name);
};
}  // namespace Buyables