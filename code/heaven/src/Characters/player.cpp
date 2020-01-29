/**
 * @file player.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief The source file for Player class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "player.h"

#include "../Network/client.h"
#include "../Network/host.h"

namespace Characters {
///////////////////////////////////////////////////////////////////////////////////////////////////
/* Statics */
///////////////////////////////////////////////////////////////////////////////////////////////////
const sf::Vector2f Player::START_POSITION = sf::Vector2f(1080, 700),
                   Player::NAME_OFFSET = sf::Vector2f(10, 50);
const std::string Player::SPRITE_FILE =
    "Ressources/Images/Characters/Player/sprite_base_addon_2012_12_14.png",
                  Player::FONT_FILE = "Ressources/Gravity-Book.otf";

sf::Texture Player::texture;
sf::Font Player::name_font;

///////////////////////////////////////////////////////////////////////////////////////////////////
/* update() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::update(float factor) {
  Character::update(factor);
  while (this->to_delete_collectables.size() > 0) {
    Collectables::Collectable* c = this->to_delete_collectables.front();
    if (c->canBeDeleted()) {
      delete c;
      this->to_delete_collectables.pop();
    } else
      break;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Player() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Player::Player(sf::Vector2f position, bool is_local) : Character(position) {
  if (texture.getSize().x == 0) {
    // First time a player is created
    if (!texture.loadFromFile(this->SPRITE_FILE)) {
      exit(1);
    }
    if (!name_font.loadFromFile(FONT_FILE)) exit(1);
  }
  setTexture(texture);
  this->is_local = is_local;
  setOrigin(32.f, 32.f);
  setTextureRect(sf::IntRect(0, 0, 64, 64));
  setScale(1.8f, 1.8f);

  this->name.setFont(this->name_font);
  this->name.setPosition(position - NAME_OFFSET);
  this->name.setString(" ");
  this->name.setFillColor(sf::Color::Black);

// Set 100 coins if in debug
#ifdef DEBUG
  this->coin_counter = 100;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* transformCollisionOn() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::transformCollisionOn(sf::FloatRect& rect) const {
  sf::Transform t;
  t.translate(45.f, 45.f);
  rect = t.transformRect(rect);
  rect.height *= .45f;
  rect.width *= .21f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* die() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::die() {
  Character::die();

  this->last_animation = 5;
  this->animation_counter = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* collect() */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool Player::collect(Collectables::Collectable* collectable) {
  if (collectable->getType() == Collectables::Type::Coin) {
    collectable->collect();
    this->coin_counter++;
    this->to_delete_collectables.push(collectable);
    return true;
  } else if (collectable->getType() == Collectables::Type::Heart) {
    collectable->collect();
    if (this->hp < MAX_HP) this->hp++;
    this->to_delete_collectables.push(collectable);
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setPosition */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::setPosition(const sf::Vector2f& position) {
  Character::setPosition(position);
  this->name.setPosition(position - NAME_OFFSET);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* draw() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::draw(sf::RenderWindow* window) const {
  Character::draw(window);
  if (name.getString() != " ") window->draw(name);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* move() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::move(const sf::Vector2f& offset) {
  Character::move(offset);
  this->name.move(offset);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* takeDamage() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::takeDamage(int damage) {
  Character::takeDamage(damage);

  // Apply to network
  if (this->is_local && Network::NetworkManager::getInstance()) {
    if (Network::NetworkManager::getInstance()->isHost()) {
      ((Network::Host*)Network::NetworkManager::getInstance())
          ->doPlayerTakeDamage(this, damage);
    } else {
      ((Network::Client*)Network::NetworkManager::getInstance())
          ->doPlayerTakeDamage(this, damage);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* heal() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::heal(uint8_t new_hp) {
  setHP(new_hp);

  if (Network::NetworkManager::getInstance()) {
    if (Network::NetworkManager::getInstance()->isHost())
      ((Network::Host*)Network::NetworkManager::getInstance())
          ->doPlayerHeal(this, new_hp);
    else
      ((Network::Client*)Network::NetworkManager::getInstance())
          ->doPlayerHeal(this, new_hp);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setState() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::setState(State state) {
  Character::setState(state);

  // Apply to network
  if (this->is_local && Network::NetworkManager::getInstance()) {
    if (Network::NetworkManager::getInstance()->isHost()) {
      ((Network::Host*)Network::NetworkManager::getInstance())
          ->doPlayerChangeState(this);
    } else {
      ((Network::Client*)Network::NetworkManager::getInstance())
          ->doPlayerChangeState(this);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setHorizontalMovement() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::setHorizontalMovement(int value) {
  Character::setHorizontalMovement(value);

  if (this->is_local && Network::NetworkManager::getInstance()) {
    if (Network::NetworkManager::getInstance()->isHost()) {
      ((Network::Host*)Network::NetworkManager::getInstance())
          ->doPlayerHorizontalDir(this);
    } else {
      ((Network::Client*)Network::NetworkManager::getInstance())
          ->doPlayerHorizontalDir(this);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* addCoins() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::addCoins(int value) { this->coin_counter += value; }

///////////////////////////////////////////////////////////////////////////////////////////////////
/* toggleAnimation() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::toggleAnimation() {
  switch (this->state) {
    case State::Dead:
      return;
      break;

    case State::Die:
      animateDie();
      break;

    case State::Idle:
      animateIdle();
      break;

    case State::Jump:
      animateJump();
      break;

    case State::Shoot:
      animateShoot();
      break;

    case State::Walk:
      animateWalk();
      break;
  }
  // animateShoot();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* animateIdle() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::animateIdle() {
  if (this->last_animation != 0) {
    this->last_animation = 0;
    this->animation_counter = 0;
  } else
    this->animation_counter %= 4;
  setTextureRect(sf::IntRect(this->animation_counter * 64, 0, 64, 64));
  this->animation_counter++;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* animateWalk() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::animateWalk() {
  if (this->last_animation != 1) {
    this->last_animation = 1;
    this->animation_counter = 0;
  } else
    this->animation_counter %= 7;
  setTextureRect(sf::IntRect(this->animation_counter * 64, 64, 64, 64));
  this->animation_counter++;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* animateJump() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::animateJump() {
  this->animation_counter %= 8;
  if (this->last_animation != 2 && this->last_animation != 3 &&
      this->last_animation != 4) {  // Not jumping before
    this->animation_counter = 0;
    this->last_animation = 2;
  } else if (this->animation_counter == 4) {  // Last keep jumping frame
    this->animation_counter = 2;
    this->last_animation = 3;
  }
  setTextureRect(sf::IntRect(this->animation_counter * 64, 128, 64, 64));
  this->animation_counter++;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* animateDie() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::animateDie() {
  setTextureRect(sf::IntRect(this->animation_counter * 64, 256, 64, 64));
  this->animation_counter++;
  if (this->animation_counter == 7) setState(State::Dead);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* animateShoot() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Player::animateShoot() {
  this->animation_counter %= 9;
  setTextureRect(sf::IntRect(this->animation_counter * 64, 256, 64, 64));
  this->animation_counter++;
}
}  // namespace Characters