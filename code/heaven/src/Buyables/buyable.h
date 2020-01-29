/**
 * @file buyable.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for source class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */
#pragma once

#include <SFML/Graphics.hpp>

#include "buyable_helper.h"

namespace Characters {
class Character;
class Player;
}  // namespace Characters

/**
 * @brief Namespace for all buyables and its factory and helpers.
 * 
 */
namespace Buyables {
/** \class Buyable
 *  @brief This class represents all buyable objects.
 *
 *  The player can buy objects with coins.
 *  This class is used to represent the objects behind buttons in the world.
 *  Buyables aren't directly accessable in the world but can be bought through
 * the buttons they are contained in.
 */
class Buyable {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 protected:
  Characters::Character* character = nullptr;
  bool is_active =
      false; /**< Determines whether this buyable is still active */
  sf::Sprite sprite;

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //
  Buyable(const sf::Texture& texture);

  virtual ~Buyable() {}

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  virtual Type getType() const = 0;

  virtual void update(float factor) {}

  virtual bool isDone() { return !this->is_active; }

  virtual void draw(sf::RenderWindow* window);

  virtual void draw(sf::RenderWindow* window, const sf::Vector2f& position);

  virtual void assignTo(Characters::Character* character);

  // ----------------------------------------- //
  /* Getter / Setter */
  // ----------------------------------------- //
  sf::Sprite getSprite() { return this->sprite; }
};
}  // namespace Buyables