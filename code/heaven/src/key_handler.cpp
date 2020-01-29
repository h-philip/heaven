/**
 * @file key_handler.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for KeyHandler class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "key_handler.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/* keyPressed() */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool KeyHandler::keyPressed(sf::Event& event, float factor) {
  switch (event.key.code) {
    case sf::Keyboard::Escape:
      // ----------------------------------------- //
      /* Close Window */
      // ----------------------------------------- //
      sf::sleep(sf::milliseconds(100));  // DO NOT DELETE THIS!
      // (Sonst geht alles kaputt.)

      this->window->close();
      return false;
      break;

    case sf::Keyboard::Space:
    case sf::Keyboard::Up:
    case sf::Keyboard::W:
      // ----------------------------------------- //
      /* Jump */
      // ----------------------------------------- //
      this->player->setJump(true);
      this->jump_pressed = true;
      break;

    case sf::Keyboard::Left:
    case sf::Keyboard::A:
      // ----------------------------------------- //
      /* Move Left */
      // ----------------------------------------- //
      this->player->setMoveDirLeft(factor);
      this->left_pressed = true;
      break;

    case sf::Keyboard::Right:
    case sf::Keyboard::D:
      // ----------------------------------------- //
      /* Move Right */
      // ----------------------------------------- //
      this->player->setMoveDirRight(factor);
      this->right_pressed = true;
      break;

    case sf::Keyboard::F6:  // Open editor
      std::cout << "Editor closed. Code: " << startEditor(nullptr) << std::endl;
      return false;
      break;

    case HelpOverlay::HELP_KEY:
      this->show_help = true;
      break;

    default:
      // ----------------------------------------- //
      /* Default */
      // ----------------------------------------- //
      break;
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* keyReleased() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void KeyHandler::keyReleased(sf::Event& event, float factor) {
  switch (event.key.code) {
    case sf::Keyboard::Space:
    case sf::Keyboard::Up:
    case sf::Keyboard::W:
      // ----------------------------------------- //
      /* Stop Try Jumping */
      // ----------------------------------------- //
      this->player->setJump(false);
      this->jump_pressed = false;
      break;

    case sf::Keyboard::Left:
    case sf::Keyboard::A:
      // ----------------------------------------- //
      /* Stop Moving Left */
      // ----------------------------------------- //
      this->left_pressed = false;
      this->player->setHorizontalMovement(0);
      break;

    case sf::Keyboard::Right:
    case sf::Keyboard::D:
      // ----------------------------------------- //
      /* Stop Moving Right */
      // ----------------------------------------- //
      this->right_pressed = false;
      this->player->setHorizontalMovement(0);
      break;

    case HelpOverlay::HELP_KEY:
      this->show_help = false;
      break;

    default:
      // ----------------------------------------- //
      /* Default */
      // ----------------------------------------- //
      break;
  }
  if (this->jump_pressed)
    this->player->setJump(true); 
  if (this->right_pressed) player->setMoveDirRight(factor);
  if (this->left_pressed) player->setMoveDirLeft(factor);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* startEditor() */
///////////////////////////////////////////////////////////////////////////////////////////////////
int KeyHandler::startEditor(char* scene) {
  delete this->world;
  this->window->setMouseCursorVisible(true);
  sf::sleep(sf::milliseconds(100));  // Same as always..
  this->window->close();
  Editor::Editor editor(sf::VideoMode(1920, 1080), scene);
  return (editor.run());
}