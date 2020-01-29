/**
 * @file key_handler.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for KeyHandler class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "Characters/player.h"
#include "Editor/editor.h"
#include "SFML/Window.hpp"
#include "help_overlay.h"
#include "world.h"

/** \class KeyHandler
 *  @brief Class for handling KeyPressed and KeyReleased events.
 */
class KeyHandler {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 private:
  // Set to true when key is pressed down, set to false when
  // key is release => All can be true at the same time.
  // Note: When Space is pressed down and then W is released, jump_pressed will
  // be set to false.
  bool right_pressed = false, left_pressed = false, jump_pressed = false;

  bool show_help = false;

  sf::RenderWindow* window;   /**< The window used */
  Characters::Player* player; /**< The player being controlled */
  World* world;               /**< The world being controlled */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Key Handler object.
   *
   * @param window Pointer to the window.
   * @param player Pointer to the player of this window.
   */
  KeyHandler(sf::RenderWindow* window, Characters::Player* player, World* world)
      : window(window), player(player), world(world) {}

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /**
   * @brief Handles actions to do when pressing any key down.
   *
   * @param event Reference to the key event.
   * @param factor The time factor.
   * @return true Signals the caller to do nothing.
   * @return false Signals the caller to end this process.
   */
  bool keyPressed(sf::Event& event, float factor);

  /**
   * @brief Handles actions to do when releasing any key.
   *
   * @param event Reference to the key event.
   * @param factor The time factor.
   */
  void keyReleased(sf::Event& event, float factor);

  void setPlayer(Characters::Player* player) { this->player = player; }

  bool showHelp() { return this->show_help; }

  /**
   * @brief Ends this window and starts the editor.
   *
   * @return The return code of the editor.
   */
  int startEditor(char* scene = nullptr);
};
