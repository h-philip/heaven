/**
 * @file buyable_factory.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Buyables::Factory class.
 * @version 0.1
 * @date 2019-12-16
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "immortality.h"
#include "jump_boost.h"

namespace Characters {
class Character;
class Player;
}  // namespace Characters

namespace Buyables {

/** \class Factory
 *  @brief Factory class for creating Buyables.
 *
 *
 */
class Factory {
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
  Factory() {}

  // ----------------------------------------- //
  /* Create Methods */
  // ----------------------------------------- //

  Buyable* createBuyable(Type t) const;

  /**
   * @brief Creates a Buyable object.
   *
   * @param t The type of the buyable to create.
   * @param character The character to whom the buyable is connected.
   * @return Buyable* Pointer to the new buyable.
   */
  Buyable* createBuyable(Type t, Characters::Character* character) const;

  /**
   * @brief Gets the price of the given buyable type.
   *
   * @param t Buyable type.
   * @return int Its price.
   */
  static int getPriceOf(Type t);

  /**
   * @brief Gets the texture of the fiven buyable type.
   *
   * @param t Buyable type.
   * @return sf::Texture& Its texture.
   */
  static sf::Texture& getTexture(Type t);

  /**
   * @brief Gets the next type.
   *
   * This method is used to switch between different types of buyables for the
   * shop. The editor uses this when pressing B.
   *
   * @param t Current buyable type.
   * @return Type Next buyable type.
   */
  static Type getNextType(Type t);
};
}  // namespace Buyables