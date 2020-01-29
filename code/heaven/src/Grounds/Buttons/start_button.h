/**
 * @file start_burron.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for StartButton class
 * @version 0.1
 * @date 2020-01-09
 *
 * @copyright Copyright (c) 2020
 *
 */

#pragma once

#include "button.h"

class World;

namespace Grounds {
/** @class StartButton
 * @brief Button to let all players start the run in a game when in multiplayer
 * as host.
 */
class StartButton : public Button {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  static const std::string
      TEXTURE_FILE; /**< File with textures for all start buttons */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 private:
  sf::Texture texture; /**< Texture */

  static Ground* start_ground; /**< The ground being controlled */
  static World* world;         /**< The world which manages the start ground */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new StartButton object.
   *
   * @param pos Position of the button
   */
  StartButton(const sf::Vector2f& pos);

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  virtual ButtonType getType() const override;

  std::string name() const override;

  virtual void contact(Characters::Character* c, float factor) override;

  void doAction(Characters::Player* p, float factor) override;

  // ----------------------------------------- //
  /* Getter / Setter */
  // ----------------------------------------- //

  /**
   * @brief Sets the ground which is controlled by the start button.
   *
   * @param world A pointer to the world containing the start ground
   * @param start_ground The ground that needs to be removed to start the game.
   */
  static void setStartGround(World* world, Ground* start_ground);

  /**
   * @brief Gets the start_ground.
   * 
   * @return Ground* The start_ground
   */
  static Ground* getStartGround();
};
}  // namespace Grounds