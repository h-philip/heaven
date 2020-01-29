/**
 * @file main_helper.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for MainHelper namespace.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "main_helper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/* createWindow() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::RenderWindow* MainHelper::createWindow() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;

  sf::VideoMode video_mode(0, 0);
  // Check if 1920x1080 VideoMode is available
  const std::vector<sf::VideoMode, std::allocator<sf::VideoMode>>
      available_video_modes = sf::VideoMode::getFullscreenModes();
  for (uint8_t i = 0; i < available_video_modes.size(); i++) {
    if (available_video_modes[i].width == 1920 &&
        available_video_modes[i].height == 1080) {
      video_mode = available_video_modes[i];
      break;
    }
  }
  if (video_mode.width == 0 &&
      video_mode.height == 0) {  // 1920x1080 is not supported
    video_mode = available_video_modes[0];
  }

  sf::RenderWindow* window = new sf::RenderWindow(
      video_mode, "Heaven", sf::Style::Fullscreen, settings);
  window->setVerticalSyncEnabled(true);
  window->setMouseCursorVisible(false);
  return window;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* loadBackgroundTexture() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Texture MainHelper::loadBackgroundTexture() {
  sf::Texture background;
  srand((unsigned int)time(0));
  uint8_t random =
      rand() % ((sizeof(BACKGROUND_FILES) / sizeof(*BACKGROUND_FILES)));

  if (!background.loadFromFile(BACKGROUND_FILES[random])) exit(1);

  return background;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* parseArgs() */
///////////////////////////////////////////////////////////////////////////////////////////////////
std::map<std::string, char*> MainHelper::parseArgs(int argc, char** argv) {
  std::map<std::string, char*> args;
  for (uint8_t i = 1; i < argc; i++) {
    char* opt = argv[i];
    int strip_index = -1;
    // Check if '=' is in opt
    for (size_t k = 0; k < strlen(opt); k++) {
      if (opt[k] == '=') {
        strip_index = k;
        break;
      }
    }

    if (strip_index != -1) {
      // Replace '=' with '\0'
      opt[strip_index] = '\0';
    }

    // Store pair in map if = was found
    // Store pair of key, key in map if no = was found
    args[std::string(opt)] = opt + strip_index + 1;
  }
  return args;
}