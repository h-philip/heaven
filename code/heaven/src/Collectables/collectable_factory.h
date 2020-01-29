/**
 * @file collectable_factory.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Collectables::Factory class.
 * @version 0.1
 * @date 2019-12-08
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "coin.h"
#include "heart.h"

namespace Collectables {

/** \class Factory
 *  @brief Factory class for creating Collectables.
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
   */
  Factory() {}

  // ----------------------------------------- //
  /* Create Methods */
  // ----------------------------------------- //

  /**
   * @brief Creates a Collectable object.
   *
   * @param t The type of the collectable to create.
   * @param position The collectables position.
   * @return Collectable* Pointer to the new collectable.
   */
  Collectable* createCollectable(Type t, const sf::Vector2f& position) const;
};
}  // namespace Collectables