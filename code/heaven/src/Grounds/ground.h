/**
 * @file ground.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for the Ground class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <SFML/Graphics.hpp>

#include "../Characters/character.h"
#include "../main_helper.h"

/**
 * @brief Namespace for all grounds.
 *
 */
namespace Grounds {

/** \class Ground
 * Base class for grounds in World.
 */
class Ground {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 protected:
  std::vector<sf::Sprite*> sprites; /**< All sprites of this ground (to be displayed) */

  sf::FloatRect global_bounds; /**< Global bounds of this ground */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Ground object.
   *
   * @param rect The position and size.
   */
  Ground(const sf::FloatRect& rect) : global_bounds(rect) {}

  /**
   * @brief Destroys the Ground object.
   *
   */
  virtual ~Ground() {
    for (auto sprite : this->sprites) delete sprite;
    this->sprites.clear();
  }

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /** @fn name
   * Returns the name of the derived class.
   */
  virtual std::string name() const = 0;

  /** @fn draw
   * Draws this Ground with all Drawables belonging to it.
   *
   * @param window The window to draw in.
   */
  virtual void draw(sf::RenderWindow* window) const;

  /** @fn contact
   * Should be called when a Character collides with this Ground.
   *
   * @param c The Character which collided.
   * @param factor The time factor.
   */
  virtual void contact(Characters::Character* c, float factor) = 0;

  /**
   * @brief Returns whether the given Character can pass through this ground.
   *
   * @param c Pointer to the Character.
   * @return true If it can pass through.
   * @return false Otherwise.
   */
  virtual bool canCollide(const Characters::Character* c) const {
    return false;
  }

  /**
   * @brief Moves this ground.
   * @param offset The offset to move this ground.
   */
  virtual void move(const sf::Vector2f& offset);

  /**
   * @brief Gets the Global Bounds object.
   *
   * @return sf::FloatRect The global bounds.
   */
  virtual sf::FloatRect getGlobalBounds() const { return this->global_bounds; }

  /**
   * @brief Sets the position of this ground.
   *
   * @param position The new position.
   */
  virtual void setPosition(const sf::Vector2f& position);

  // ----------------------------------------- //
  /* Getter / Setter */
  // ----------------------------------------- //

  virtual std::vector<sf::Sprite*>& getSprites() { return this->sprites; 
  }
};
}  // namespace Grounds