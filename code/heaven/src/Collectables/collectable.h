/**
 * @file collectable.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Collectable class
 * @version 0.1
 * @date 2019-12-12
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "collectable_helper.h"

/**
 * @brief Namespace for all collectable things
 *
 */
namespace Collectables {

/** \class Collectable
 *  @brief Base class for all collectables in the world.
 *
 *
 */
class Collectable : public sf::Sprite {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 protected:
  bool is_collected =
      false; /**< Determines whether this collectable is collected currently */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Collectable object.
   *
   * @param position Position of the collectable
   */
  Collectable(const sf::Vector2f& position) { setPosition(position); }

  /**
   * @brief Destroys the Collectable object.
   *
   */
  virtual ~Collectable() {}

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /**
   * @brief Returns the type of the subclass.
   *
   * @return Type The type
   */
  virtual Type getType() = 0;

  /**
   * @brief Update method for collectable.
   *
   * @param factor The time factor
   */
  virtual void update(float factor){};

  /**
   * @brief Draws this collectable and everything related.
   *
   * @param window The current window
   */
  virtual void draw(sf::RenderWindow* window);

  /**
   * @brief Tells the collectable that the animation time passed.
   *
   */
  virtual void toggleAnimation(){};

  /**
   * @brief Collects this collectable
   *
   */
  virtual void collect();

  /**
   * @brief Whether this collectable can be deleted.
   * 
   * @return true When it can (and should) be deleted.
   * @return false Otherwise.
   */
  virtual bool canBeDeleted() = 0;
};
}  // namespace Collectables
