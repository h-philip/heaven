/**
 * @file button_helper.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for ButtonHelper class.
 * @version 0.1
 * @date 2020-01-26
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "button_helper.h"

namespace Grounds {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* typeToName() */
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string Grounds::ButtonHelper::typeToName(ButtonType t) {
  switch (t) {
    case ButtonType::NetworkButton:
      return "network_button";
      break;

    case ButtonType::BuyableButton:
      return "buyable_button";
      break;

    case ButtonType::StartButton:
      return "start_button";
      break;

    default:
      std::cerr << "Error: ButtonType has no name: "
                << std::to_string((uint8_t)t) << std::endl;
      return "unknown";
      break;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* nameToType() */
///////////////////////////////////////////////////////////////////////////////////////////////////
ButtonType Grounds::ButtonHelper::nameToType(std::string name) {
  if (name == "network_button")
    return ButtonType::NetworkButton;
  else if (name == "buyable_button")
    return ButtonType::BuyableButton;
  else if (name == "start_button")
    return ButtonType::StartButton;
  else if (name == "solid_ground")
    return ButtonType::Unknown;
  else if (name == "bad_ground")
    return ButtonType::Unknown;
  else if (name == "enemy_border")
    return ButtonType::Unknown;
  else if (name == "portal")
    return ButtonType::Unknown;
  else {
    std::cerr << "Error: Unknown button type: " << name << std::endl;
    return ButtonType::Unknown;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* isButton() */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ButtonHelper::isButton(std::string name) {
  return nameToType(name) != ButtonType::Unknown;
}
}  // namespace Grounds