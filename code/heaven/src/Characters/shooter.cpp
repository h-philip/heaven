/**
 * @file shooter.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Shooter class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "shooter.h"

namespace Characters {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Static Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////

const float Shooter::MAX_SHOOT_DISTANCE = 300.f;
const float Shooter::SHOOT_SPEED = 50.f;
const std::string Shooter::SPRITE_FILE =
    "Ressources/Images/Characters/Enemies/shooter_sprite.png";

sf::Texture Shooter::texture;

///////////////////////////////////////////////////////////////////////////////////////////////////
/* update() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Shooter::update(float factor) {
  Character::update(factor);

  // Shoot at player if near enough
  Player* nearest = getNearestPlayer();
  if (nearest)
    if (getDistanceTo(nearest->getPosition()) < this->MAX_SHOOT_DISTANCE) {
      // Check if shoot delay is ok
      if (this->shoot_clock.getElapsedTime().asMilliseconds() >=
          this->SHOOT_DELAY) {
        this->shoot_clock.restart();
        shootAt(nearest);
      }
    }

  // Bullets
  for (size_t i = 0; i < this->fired_bullets.size(); i++) {
    Bullets::Bullet* b = this->fired_bullets[i];
    b->update(factor);

    // Check if bullet should be removed
    if (b->isDestroyed()) {
      delete b;
      this->fired_bullets.erase(this->fired_bullets.begin() + i);
      i--;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* draw() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Shooter::draw(sf::RenderWindow* window) const {
  Characters::Character::draw(window);
  for (auto b : this->fired_bullets) b->draw(window);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* shootAt() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Shooter::shootAt(Character* character) {
  sf::Vector2f movement = character->getPosition() - getPosition();

  // Normalize
  float d = sqrtf(pow(movement.x, 2) + pow(movement.y, 2));
  movement /= d;
  movement *= this->SHOOT_SPEED;

  this->fired_bullets.push_back(
      new Bullets::Bullet(getPosition(), movement, this->all_player));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* die() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Shooter::die() {
  Character::die();
  this->last_animation = 5;
  this->animation_counter = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* toggleAnimation() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Shooter::toggleAnimation() {
  /* last_animation:
     0 = idle
     1 = standard left
     2 = standard right
     3 = shoot left
     4 = shoot right
     5 = die
   */
  Player* nearest = getNearestPlayer();

  // Die
  if (this->last_animation == 5) animateDie();

  // Player near
  else if (nearest &&
           getDistanceTo(nearest->getPosition()) <= this->MAX_SHOOT_DISTANCE) {
    sf::Int32 clock_time = this->shoot_clock.getElapsedTime().asMilliseconds();
    // Shoot
    if (clock_time >= this->SHOOT_DELAY - 100 || clock_time < 100) {
      // Left
      if (nearest->getPosition().x < getPosition().x) this->last_animation = 3;
      // Right
      else
        this->last_animation = 4;

      animateShoot();
    }

    // Don't shoot
    else {
      // Left
      if (nearest->getPosition().x < getPosition().x) this->last_animation = 1;
      // Right
      else
        this->last_animation = 2;

      animateIdle();
    }
  }

  // Idle
  else {
    this->last_animation = 0;
    animateIdle();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* animateIdle() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Shooter::animateIdle() {
  switch (this->last_animation) {
    // Middle
    case 0:
      setTextureRect(sf::IntRect(0, 0, 31, 31));
      break;

    // Left
    case 1:
      setTextureRect(sf::IntRect(31, 0, 31, 31));
      break;

    // Right
    case 2:
      setTextureRect(sf::IntRect(62, 0, 31, 31));
      break;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* animateDie() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Shooter::animateDie() {
  if (this->animation_counter != 1) {
    setTextureRect(sf::IntRect(155, 0, 31, 31));
    this->animation_counter = 1;
  } else {
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* animateShoot */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Shooter::animateShoot() {
  switch (this->last_animation) {
    // Left
    case 3:
      setTextureRect(sf::IntRect(93, 0, 31, 31));
      break;

    // Right
    case 4:
      setTextureRect(sf::IntRect(124, 0, 31, 31));
      break;
  }
}
}  // namespace Characters