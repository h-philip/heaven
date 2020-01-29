/**
 * @file network_button.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for NetworkButton class.
 * @version 0.1
 * @date 2019-12-01
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "button.h"

namespace Grounds {
/** @class NetworkButton
 * @brief Button to open game to network as host or connect to other host.
 */
class NetworkButton : public Button {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  static const std::string
      TEXTURE_FILE; /**< File with textures for all network buttons */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 private:
  static sf::Texture texture; /**< Texture for all network buttons */
  static bool is_pressed; /**< Determines whether this button is currently being
                             pressed (Enter pressed while colliding) */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Network Button object.
   *
   * @param pos Position of the button
   */
  NetworkButton(const sf::Vector2f& pos) : Button(pos) {
    if (texture.getSize().x == 0)
      if (!texture.loadFromFile(TEXTURE_FILE)) exit(1);

    this->sprites.push_back(
        new sf::Sprite(texture, sf::IntRect(0, 0, 100, 100)));
    this->sprites[0]->setPosition(pos);
  }

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  virtual ButtonType getType() const override;

  std::string name() const override;

  void doAction(Characters::Player* p, float factor) override;

  static bool isPressed();

  static void press(bool value);
};
}  // namespace Grounds