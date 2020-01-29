#pragma once

#include "buyable.h"

namespace Characters {
class Character;
class Player;
}  // namespace Characters

namespace Buyables {

/**
 *  @brief Class for jump boost buyable.
 *
 *
 */
class JumpBoost : public Buyable {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  static const uint16_t RUN_SECONDS =
      15; /**< The time in seconds until this boost expires */
  static const float JUMP_MODIFY; /**< The value to add to the characters
                                     jump_power while active */
  static const int PRICE = 3;     /**< The price in coins for buying this */

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
  const uint16_t time; /**< The time to run for the current jump boost */

  static sf::Texture texture; /**< Texture for all jump boost buyables */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new JumpBoost object with a character to be assigned.
   *
   * @param character The character.
   * @param run_time The time to run. Default: RUN_SECONDS
   */
  JumpBoost(Characters::Character* character, uint16_t run_time = RUN_SECONDS);

  /**
   * @brief Constructs a new Jump Boost object.
   *
   * @param run_time The time to run. Default: RUN_SECONDS
   */
  JumpBoost(uint16_t run_time = RUN_SECONDS);

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
