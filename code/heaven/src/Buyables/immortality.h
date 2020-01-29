/**
 * @file immortality.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Immortality class
 * @version 0.1
 * @date 2020-01-15
 *
 *
 */
#pragma once

#include "buyable.h"

namespace Characters {
class Character;
class Player;
}  // namespace Characters

namespace Buyables {

/**
 *  @brief Class for immortality buyable.
 *
 *
 */
class Immortality : public Buyable {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  static const uint16_t RUN_SECONDS =
      30; /**< The time in seconds until this boost expires */
  static const int PRICE = 15; /**< The price in coins for buying this */

  static const std::string
      ICON_TEXTURE; /**< The file with the texture for this icon */
  static const sf::Vector2f
      ICON_SIZE,   /**< The size when drawn as icon in hud */
      OBJECT_SIZE; /**< The size when drawn as button or collectable in hud */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 private:
  sf::Clock clock;     /**< Clock for setting inactive after time */
  const uint16_t time; /**< Stores the time to run for current instance */
  static sf::Texture texture; /**< The texture for all immortality buyables */

  uint8_t base_hp = 0; /**< The hp of the character when assigned;
                       will be increased when hp of character increases */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Immortality object with a character to assign to.
   *
   * @param character The character to assign to.
   * @param run_time The time to run. Default: RUN_SECONDS
   */
  Immortality(Characters::Character* character,
              uint16_t run_time = RUN_SECONDS);

  /**
   * @brief Constructs a new Immortality object.
   *
   * @param run_time The time to run. Default: RUN_SECONDS
   */
  Immortality(uint16_t run_time = RUN_SECONDS);

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  Type getType() const override;

  void update(float factor) override;

  /**
   * @brief Gets the price of an immortality buyable.
   *
   * @return int the price.
   */
  static int getPrice() { return PRICE; }

  void assignTo(Characters::Character* character) override;

  /**
   * @brief Gets the texture of all immortality buyables.
   *
   * @return sf::Texture& The texture.
   */
  static sf::Texture& getTexture();
};
}  // namespace Buyables
