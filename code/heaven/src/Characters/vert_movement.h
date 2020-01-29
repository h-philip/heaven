/**
 * @file vert_movement.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for VertMovement class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <SFML/Window.hpp>

namespace Characters {

/** \class VertMovement
 * @brief This class contains all vertical movement related code.
 *
 * Each character to use this class for jumping and falling.
 * Note: The sign of the numbers is oriented by the display coordinates.
 */
class VertMovement {
 public:
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  const float GRAVITY = 14; /**< Force that pushes character down */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  float current_speed, /**< Current speed with direction */
      max_speed;       /**< The maximum vertical speed */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Vert Movement object.
   *
   * @param max_speed The maximum speed at which the associated Character can
   * fall.
   */
  VertMovement(float max_speed) : current_speed(0.0f), max_speed(max_speed){};

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /** @fn applyForce
   * Is used to apply a force (acceleration) two this movement instance.
   *
   * @param force The force to apply.
   * @return The new current_accel-value of this instance.
   */
  void applyForce(float force);

  /** @fn applyForce
   * Is used to apply a force (acceleration) two this movement instance.
   *
   * @param force The force to apply.
   * @param factor The factor which determining how much time (relative to a
   * second) has passed.
   * @return The new current_accel-value of this instance.
   */
  void applyForce(float force, float factor);

  /** @fn applyGravity
   * Apply a gravity force.
   *
   * @param factor The factor which determining how much time (relative to a
   * second) has passed.
   */
  void applyGravity(float factor);

  /** @fn move
   * Returns the movement which would currently happen with given factor.
   *
   * @param factor The factor which determines how much time has passed.
   * @return The movement distance (with sign)
   */
  float move(float factor) const;
};
}  // namespace Characters