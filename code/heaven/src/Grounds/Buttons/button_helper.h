/**
 * @file button_helper.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for ButtonHelper class.
 * @version 0.1
 * @date 2020-01-26
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once

#include <iostream>
#include <string>

namespace Grounds {

/**
 * @brief All types of buttons.
 *
 */
enum class ButtonType : uint8_t { Unknown, NetworkButton, BuyableButton, StartButton };

/** \class ButtonHelper
 *  @brief Helper class for buttons.
 *
 *
 */
class ButtonHelper {
 public:
  /**
   * @brief Converts a button type to its name.
   *
   * @param t The button type to convert.
   * @return std::string The corresponding name.
   */
  static std::string typeToName(ButtonType t);

  /**
   * @brief Converts a name to its button type.
   *
   * @param name The name to convert.
   * @return Type The corresponding button type.
   */
  static ButtonType nameToType(std::string name);

  /**
   * @brief Tells whether a name is the name of a button.
   *
   * @param name The name to check.
   * @return true When it's a buttons name.
   * @return false When it's not a name of a button.
   */
  static bool isButton(std::string name);
};
}  // namespace Grounds