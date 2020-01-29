/**
 * @file shooter.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Shooter class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "Bullets/bullet.h"
#include "enemy.h"

namespace Characters {

/** \class Shooter
 *  @brief Class for an Enemy that can't move but can shoot at the player.
 */
class Shooter : public Characters::Enemy {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  static const float
      MAX_SHOOT_DISTANCE, /**< The maximum distance at which a player will be
                             shot (start of bullet) */
      SHOOT_SPEED;        /**< The speed of the shot bullet */
  static const sf::Int32 SHOOT_DELAY =
      1000;                             /**< Min delay in ms between to shots */
  static const std::string SPRITE_FILE; /**< File with sprites for this enemy */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 protected:
  sf::Clock shoot_clock; /**< Clock to measure time since last shot */
  std::vector<Bullets::Bullet*>
      fired_bullets; /**< All fired (and currently existent) bullets */

  static sf::Texture texture; /**< Texture for the shooter */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Shooter object.
   *
   * @param position The new position.
   * @param players A vector to all Players at which this Shooter can shoot.
   */
  Shooter(sf::Vector2f position, std::vector<Player*>& players)
      : Enemy(position, players) {
    this->can_move = false;
    if (texture.getSize().x == 0) {
      if (!texture.loadFromFile(this->SPRITE_FILE)) exit(1);
    }
    setTexture(texture);
    setTextureRect(sf::IntRect(0, 0, 31, 31));

    setScale(1.5f, 1.5f);
    setOrigin(15, 15);
  };

  /**
   * @brief Destroys the Shooter object.
   * 
   */
  ~Shooter() {
    for (size_t i = 0; i < this->fired_bullets.size(); i++) {
      delete this->fired_bullets[i];
    }
  }

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /**
   * @brief Returns the type of this sub class.
   *
   * @return Type Shooter
   */
  Type getType() const override { return Type::Shooter; }

  virtual void update(float factor) override;

  virtual void draw(sf::RenderWindow* window) const override;

  /**
   * @brief Shoots a new bullet at the target.
   *
   * @param character The target.
   */
  virtual void shootAt(Character* character);

  void die() override;

  // ----------------------------------------- //
  /* Animation Methods */
  // ----------------------------------------- //

  void toggleAnimation() override;

 private:
  void animateIdle();

  void animateDie();

  void animateShoot();
};

}  // namespace Characters