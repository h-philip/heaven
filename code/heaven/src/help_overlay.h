/**
 * @file help_overlay.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for HelpOverlay class
 * @version 0.1
 * @date 2020-01-15
 *
 *
 */
#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string.h>

/** \class HelpOverlay
 *  @brief Class for displaying an overlay which explains keybindings ingame.
 */
class HelpOverlay {
 public:
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  static const sf::Keyboard::Key HELP_KEY =
      sf::Keyboard::Key::F1; /**< The key which shows the help */

 private:
  static const std::string
      EDITOR_HELP_FILE, /**< Path to the file containing the help for editor
                           keybindings */
      INGAME_HELP_FILE, /**< Path to the file containing the help for ingame
                           keybindings */
      FONT_FILE;        /**< Path to the font file */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 private:
  sf::RenderWindow* window; /**< Pointer to the window to draw in */

  bool editor;   /**< Whether the help is for the editor. else: normal game */
  sf::Text text; /**< The help text to be displayed */
  sf::Font font;
  sf::Vector2i default_position; /**< The position of the text without view
                                    transformation */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Creates a new Help Overlay object.
   *
   * @param window The window in which the overlay will be displayed
   * @param editor True if help for the editor should be shown
   *               False if help for default game should be shown
   */
  HelpOverlay(sf::RenderWindow* window, bool editor);

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /**
   * @brief Draws the help overlay.
   *
   */
  void draw();

 private:
  /**
   * @brief Loads text from a file.
   *
   * @param file Path to the file to be loaded from
   * @return std::string All the text from the file
   */
  std::string loadHelpText(std::string file);
};
