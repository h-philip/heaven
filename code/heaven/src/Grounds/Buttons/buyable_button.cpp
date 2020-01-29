/**
 * @file buyable_button.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for BuyableButton class.
 * @version 0.1
 * @date 2019-12-16
 *
 * @copyright Copyright (c) 2019
 *
 */
#include "buyable_button.h"

namespace Grounds {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Static Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string BuyableButton::TEXTURE_FILE =
    "Ressources/Images/Grounds/Buttons/buyable_button.png";

sf::Texture BuyableButton::texture;

///////////////////////////////////////////////////////////////////////////////////////////////////
/* BuyableButton() */
///////////////////////////////////////////////////////////////////////////////////////////////////
BuyableButton::BuyableButton(const sf::Vector2f& pos, Buyables::Type type)
    : Button(pos), type(type), buyables_factory() {
  if (texture.getSize().x == 0)
    if (!texture.loadFromFile(TEXTURE_FILE)) exit(1);

  this->sprites.push_back(new sf::Sprite(texture, sf::IntRect(0, 0, 100, 100)));
  this->sprites[0]->setPosition(pos);

  this->buyable = buyables_factory.createBuyable(type);

  if (!this->buyable) {  // Should not happen
    std::cerr << "Error while creating new BuyableButton: createBuyable "
                 "returned nullptr. type is: "
              << std::to_string((uint8_t)type) << std::endl;
    exit(1);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getType() */
///////////////////////////////////////////////////////////////////////////////////////////////////
ButtonType BuyableButton::getType() const { return ButtonType::BuyableButton; }

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doAction() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void BuyableButton::doAction(Characters::Player* p, float factor) {
  // Return if timeout not reached
  if (this->timeout_clock.getElapsedTime().asMilliseconds() < CLICK_TIMEOUT)
    return;
  else
    this->timeout_clock.restart();

  if (p->getCoins() >= Buyables::Factory::getPriceOf(this->type)) {
    p->addCoins(-1 * Buyables::Factory::getPriceOf(this->type));
    this->buyable->assignTo(p);
    this->buyable = buyables_factory.createBuyable(this->type, nullptr);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* draw() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void BuyableButton::draw(sf::RenderWindow* window) const {
  Button::draw(window);
  this->buyable->draw(window,
                      this->sprites[0]->getPosition() + sf::Vector2f(30, 20));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setBuyableType() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void BuyableButton::setBuyableType(Buyables::Type t) {
  this->type = t;
  delete this->buyable;
  this->buyable = buyables_factory.createBuyable(t);
}
}  // namespace Grounds