/**
 * @file start_button.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for StartButton class
 * @version 0.1
 * @date 2020-01-09
 *
 * @copyright Copyright (c) 2020
 *
 */
#include "start_button.h"

#include "../../Network/host.h"
#include "../../world.h"

namespace Grounds {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Static Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string StartButton::TEXTURE_FILE =
    "Ressources/Images/Grounds/Buttons/start_button.png";

Ground* StartButton::start_ground = nullptr;
World* StartButton::world = nullptr;

///////////////////////////////////////////////////////////////////////////////////////////////////
/* StartButton() */
///////////////////////////////////////////////////////////////////////////////////////////////////
StartButton::StartButton(const sf::Vector2f& pos) : Button(pos) {
  if (!texture.loadFromFile(TEXTURE_FILE)) exit(1);

  this->sprites.push_back(new sf::Sprite(texture, sf::IntRect(0, 0, 100, 100)));
  this->sprites[0]->setPosition(pos);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getType() */
///////////////////////////////////////////////////////////////////////////////////////////////////
ButtonType StartButton::getType() const { return ButtonType::StartButton; }

///////////////////////////////////////////////////////////////////////////////////////////////////
/* name() */
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string StartButton::name() const {
  return ButtonHelper::typeToName(getType());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* contact() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void StartButton::contact(Characters::Character* c, float factor) {
  // Check if
  // - key pressed
  // - if this is network -> i am host
  // - character is player
  // - character is a local one
  if (sf::Keyboard::isKeyPressed(Button::INTERACT_KEY) &&
      ((Network::NetworkManager::getInstance() &&
        Network::NetworkManager::getInstance()->isHost()) ||
       !Network::NetworkManager::getInstance()) &&
      c->getType() == Characters::Type::Player && c->isLocal())
    doAction((Characters::Player*)c, factor);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doAction() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void StartButton::doAction(Characters::Player* p, float factor) {
  if (!this->start_ground)
    return;  // Do nothing if ground isn't set or already removed

  this->world->removeGround(StartButton::start_ground);

  if (Network::NetworkManager::getInstance() &&
      Network::NetworkManager::getInstance()->isHost()) {
    ((Network::Host*)Network::NetworkManager::getInstance())
        ->doRemoveGround(StartButton::start_ground);
  }

  this->world->startRace();

  StartButton::start_ground = nullptr;

#ifdef INFO
  std::cout << "Info: StartButton::doAction -> done." << std::endl;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setStartGround() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void StartButton::setStartGround(World* world, Ground* start_ground) {
  if (!world || !start_ground) return;
  StartButton::world = world;
  StartButton::start_ground = start_ground;
#ifdef INFO
  std::cout
      << "Info: StartButton::setStartGround -> world and start_ground were set."
      << std::endl;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getStartGround() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Ground* StartButton::getStartGround() { return StartButton::start_ground; }
}  // namespace Grounds