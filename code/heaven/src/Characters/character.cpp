/**
 * @file character.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Character class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "character.h"

#include <iostream>

#include "../Grounds/ground.h"
#include "../Network/client.h"
#include "../Network/host.h"

namespace Characters {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Constant Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////
const float Character::HORIZONTAL_MOVE_SPEED = 35.f,
            Character::MAX_ABS_FALL_SPEED = 300.f,
            Character::JUMP_ACCEL = -27.f,
            Character::HIGH_JUMP_MULTIPLIER = .4f,
            Character::MAX_JUMP_HIGHER_TIME_MS = 1000.f,
            Character::DEFAULT_WIDTH = 18.f, Character::DEFAULT_HEIGHT = 50.f;
const uint8_t Character::MAX_HP = 5;
const sf::Color Character::DAMAGE_COLOR = sf::Color(255, 100, 100, 255);

const std::string Character::FOOTSTEPS_FILE = "Ressources/Sounds/footsteps.wav",
                  Character::JUMP_FILE = "Ressources/Sounds/jump.wav",
                  Character::LAND_FILE = "Ressources/Sounds/land.wav";

sf::SoundBuffer Character::footsteps_buffer, Character::jump_buffer,
    Character::land_buffer;

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Characer() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Character::Character(sf::Vector2f position)
    : movement(this->MAX_ABS_FALL_SPEED),
      default_texture(new sf::Texture()),
      hp(MAX_HP) {
  if (this->footsteps_buffer.getDuration().asMicroseconds() == 0)
    if (!this->footsteps_buffer.loadFromFile(FOOTSTEPS_FILE)) exit(1);
  this->footsteps.setBuffer(this->footsteps_buffer);
  this->footsteps.setLoop(true);

  if (this->jump_buffer.getDuration().asMicroseconds() == 0)
    if (!this->jump_buffer.loadFromFile(JUMP_FILE)) exit(1);
  this->jump.setBuffer(this->jump_buffer);
  this->jump.setVolume(70);

  if (this->land_buffer.getDuration().asMicroseconds() == 0)
    if (!this->land_buffer.loadFromFile(LAND_FILE)) exit(1);
  this->land.setBuffer(this->land_buffer);
  this->land.setVolume(70);

  this->footsteps.setPosition((position.x - 960) * .5f, position.y, 0);
  this->footsteps.setMinDistance(400);
  this->footsteps.setAttenuation(4);
  this->jump.setPosition((position.x - 960) * .5f, position.y, 0);
  this->jump.setMinDistance(400);
  this->jump.setAttenuation(4);
  this->land.setPosition((position.x - 960) * .5f, position.y, 0);
  this->land.setMinDistance(400);
  this->land.setAttenuation(4);

  setPosition(position);
  setTexture(*this->default_texture);
  setTextureRect(sf::IntRect(0, 0, 50, 50));

  setMoveSpeed(HORIZONTAL_MOVE_SPEED);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* ~Character() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Character::~Character() {
  delete this->default_texture;
  for (auto b : this->buyables) delete b;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setJump() */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool Character::setJump(bool value) {
  if (this->hp <= 0) return false;
  this->keeps_jumping = value;
  if (this->is_local) setState(State::Jump);
  if (value && this->on_ground) {
    movement.current_speed = 0.f;
    movement.applyForce(this->jump_power);
    this->on_ground = false;
    this->jump_clock.restart();
    this->jump.play();
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setMoveDirRight() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::setMoveDirRight(float factor) {
  if (this->hp <= 0) return;
  if (this->on_ground) setState(State::Walk);
  setHorizontalMovement(1);
  if (getScale().x > 0) return;
  scale(-1, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setMoveDirLeft() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::setMoveDirLeft(float factor) {
  if (this->hp <= 0) return;
  if (this->on_ground) setState(State::Walk);
  setHorizontalMovement(-1);
  if (getScale().x < 0) return;
  scale(-1, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* update() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::update(float factor) {
  // Damage color
  if (this->damage_colored &&
      this->damage_clock.getElapsedTime().asMilliseconds() > 250) {
    setColor(sf::Color(255, 255, 255, 255));
    this->damage_colored = false;
  }

  if (this->state == State::Walk) {
    if (this->footsteps.getStatus() != sf::Sound::Status::Playing)
      this->footsteps.play();
  } else if (this->footsteps.getStatus() == sf::Sound::Status::Playing)
    this->footsteps.stop();

  if (this->is_local) updateMovement(factor);

  for (size_t i = 0; i < this->buyables.size(); i++) {
    this->buyables[i]->update(factor);
    if (this->buyables[i]->isDone()) {
      delete this->buyables[i];
      this->buyables.erase(this->buyables.begin() + i);
      i--;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* draw() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::draw(sf::RenderWindow* window) const {
  window->draw(*this);
  for (size_t i = 0; i < this->buyables.size(); i++) {
    this->buyables[i]->draw(
        window, window->mapPixelToCoords(getBuyableIconPos((int)i)));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* updateMovement() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::updateMovement(float factor) {
  // Vertical movement
  this->movement.applyGravity(factor);
  if (this->keeps_jumping && this->can_keep_jumping) {
    // The following statement calculates the force which should push the
    // Character in the air because it is currently jumping and wants to jump
    // higher. Depending on the time passed since it started jumping, the force
    // will be lower (linear i think).
    float force = JUMP_ACCEL * HIGH_JUMP_MULTIPLIER *
                  (1 - (this->jump_clock.getElapsedTime().asMilliseconds() /
                        this->MAX_JUMP_HIGHER_TIME_MS));
    this->movement.applyForce(force, factor);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* touchGround() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::touchGround() {
  if (!this->on_ground) {
    if (this->land.getStatus() != sf::Sound::Status::Playing) this->land.play();
    if (this->is_local && this->hp > 0) {
      if (this->horizontal_movement == 0)
        setState(State::Idle);
      else
        setState(State::Walk);
    }
  }
  this->on_ground = true;
  this->movement.current_speed = 0;
  this->can_keep_jumping = true;
  if (this->keeps_jumping) {
    this->last_animation = 1;  // To animate the jump start when keep jumping
    setJump(true);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* touchCeiling() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::touchCeiling() {
  this->can_keep_jumping = false;
  this->movement.current_speed = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* horizontalMove() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::horizontalMove(float factor) {
  move(this->fixed_move.x, 0);
  this->fixed_move.x = 0;
  if (this->horizontal_movement != 0) {
    move(sf::Vector2f(this->horizontal_movement * this->speed * factor, 0.f));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getHorizontalMove() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::FloatRect Character::getHorizontalMove(float factor) const {
  sf::FloatRect new_global_bounds = this->getGlobalBounds();
  new_global_bounds.left += this->fixed_move.x;
  new_global_bounds.left += this->horizontal_movement * this->speed * factor;
  return new_global_bounds;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* verticalMove() */
///////////////////////////////////////////////////////////////////////////////////////////////////
float Character::verticalMove(float factor) {
  float prev_pos = getGlobalBounds().top;
  move(0, this->fixed_move.y);
  this->fixed_move.y = 0;
  move(sf::Vector2f(0.f, this->movement.move(factor)));
  return prev_pos - getGlobalBounds().top;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getVerticalMove() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::FloatRect Character::getVerticalMove(float factor) const {
  sf::FloatRect new_global_bounds = this->getGlobalBounds();
  new_global_bounds.top += this->fixed_move.y;
  new_global_bounds.top += this->movement.move(factor);
  return new_global_bounds;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* canMoveTo() */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool Character::canMoveTo(
    sf::FloatRect new_global_bounds,
    std::vector<Grounds::Ground*> possible_colliding_grounds,
    std::vector<Grounds::Ground*>* collided_grounds,
    bool* collided_head) const {
  for (Grounds::Ground* g : possible_colliding_grounds) {
    if (g->getGlobalBounds().intersects(new_global_bounds)) {  // Intersecting
      if (!g->canCollide((const Character*)this)) {  // Not allowed to collide

        // Check if head collided
        sf::FloatRect head = new_global_bounds;
        head.height = 1;

        if (collided_head) {
          if (head.intersects(g->getGlobalBounds()))  // Collision on head
            *collided_head = true;
          else
            *collided_head = false;
        }
        return false;
      } else if (collided_grounds) {
        collided_grounds->push_back(g);
      }
    }
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setPosition() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::setPosition(const sf::Vector2f& position) {
  sf::Sprite::setPosition(position);

  sf::Vector3f audio_pos((getPosition().x - 960.f) * .5f, getPosition().y, 0);
  this->footsteps.setPosition(audio_pos);
  this->jump.setPosition(audio_pos);
  this->land.setPosition(audio_pos);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* move() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::move(const sf::Vector2f& offset) {
  sf::Sprite::move(offset);

  sf::Vector3f audio_pos((getPosition().x - 960.f) * .5f, getPosition().y, 0);
  this->footsteps.setPosition(audio_pos);
  this->jump.setPosition(audio_pos);
  this->land.setPosition(audio_pos);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setHP() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::setHP(int hp) { this->hp = hp; }

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setHorizontalMovement() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::setHorizontalMovement(int value) {
  if (this->hp <= 0) {
    this->horizontal_movement = 0;
  } else {
    this->horizontal_movement = value;
    if (value == 0 && this->on_ground) setState(State::Idle);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setOnGround() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::setOnGround(bool value) {
  this->on_ground = value;
  if (this->hp <= 0) return;
  if (!value)
    setState(State::Jump);
  else
    setState(State::Idle);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setState() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::setState(State state) {
  if (!this->is_local) {
    if (this->state == State::Jump &&
        (state == State::Walk || state == State::Idle)) {
      touchGround();
    } else if (this->state == State::Idle || this->state == State::Walk) {
      this->on_ground = true;
      if (state == State::Jump) {
        setJump(true);
      }
    }
  }
  this->state = state;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* takeDamage() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::takeDamage(int damage) {
  this->hp -= damage;
  if (this->hp <= 0)
    die();
  else if (this->hp > Player::MAX_HP)
    this->hp = Player::MAX_HP;

  this->damage_clock.restart();
  this->damage_colored = true;
  setColor(this->DAMAGE_COLOR);
  this->movement.current_speed = 0.f;
  this->movement.applyForce(-15.f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* die() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::die() {
  this->hp = 0;
  setState(State::Die);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getGlobalBounds() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::FloatRect Character::getGlobalBounds() const {
  sf::FloatRect bounds = sf::Sprite::getGlobalBounds();
  transformCollisionOn(bounds);
  return bounds;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getDistanceTo() */
///////////////////////////////////////////////////////////////////////////////////////////////////
float Character::getDistanceTo(sf::Vector2f position) const {
  return sqrtf(pow((position.x - getPosition().x), 2) +
               pow((position.y - getPosition().y), 2));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getLogText() */
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> Character::getLogText() {
  return std::vector<std::string>();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* addBuyable() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Character::addBuyable(Buyables::Buyable* buyable) {
  bool contained = false;
  for (auto b : this->buyables)
    if (b == buyable) {
      contained = true;
      break;
    }
  if (!contained) this->buyables.push_back(buyable);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getBuyableIconPos() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Vector2i Character::getBuyableIconPos(int index) const {
  sf::Vector2i p;
  p.x = 1850;
  p.y = 200 + index * 70;
  return p;
}
}  // namespace Characters