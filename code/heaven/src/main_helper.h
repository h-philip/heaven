/**
 * @file main_helper.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for MainHelper namespace.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <cstdlib>
#include <stdio.h>
#include <string.h>


#include "SFML/Graphics.hpp"

/**
 * @brief Contains constants and functions directly for the main file and
 * KeyHandler class.
 *
 */
namespace MainHelper {
///////////////////////////////////////////////////////////////////////////////////////////////////
/* Constant Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////
const std::string BACKGROUND_FILES[] = {
    "Ressources/Images/Backgrounds/sky1.png",
}; /**< Array with files
             that can be used as
             backgrounds */

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Methods */
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Creates a Window object
 *
 * @return sf::RenderWindow* The pointer to the new window created.
 */
sf::RenderWindow* createWindow();

/**
 * @brief Loads a background texture from all available images.
 *
 * @return sf::Texture Background texture.
 */
sf::Texture loadBackgroundTexture();

/**
 * @brief Parses the command line args into a map.
 *
 * @param argc The original argc from main.
 * @param argv The original argv from main.
 * @return std::map<std::string, char*> A map with key option name and
 * value option value.
 */
std::map<std::string, char*> parseArgs(int argc, char** argv);
}  // namespace MainHelper