/**
 * @file walker.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Walker class
 * @version 0.1
 * @date 2019-12-12
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "enemy.h"

namespace Characters {

/** \class Walker
 *  @brief Class for an Enemy that can only walk.
 */
class Walker : public Characters::Enemy {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  static const std::string
      SPRITE_FILE; /**< File with sprites for all walkers */
  static const float HORIZONTAL_MOVE_SPEED; /**< Speed at which this walker can
                                               move horizontally */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 protected:
  const std::vector<Grounds::Ground*>& all_grounds; /**< Vector with all grounds to check colliding */
  const sf::RenderWindow* window; /**< Current window */
  static sf::Texture texture; /**< Texture for this walker */
  std::map<const Characters::Player*, sf::Clock>
      damage_clocks; /**< Measures the time passed since last time damage was
                       dealt to player */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Walker object.
   *
   * @param position The new position.
   * @param players A vector to all Players which can take damage by this
   * walker.
   */
  Walker(sf::Vector2f position, std::vector<Player*>& players,
         const std::vector<Grounds::Ground*>& grounds,
         const sf::RenderWindow* window)
      : Enemy(position, players), all_grounds(grounds), window(window) {
    if (texture.getSize().x == 0) {
      if (!texture.loadFromFile(this->SPRITE_FILE)) exit(1);
    }
    setTexture(texture);
    setTextureRect(sf::IntRect(704, 1256, 120, 159));

    setOrigin(60, 79);
    setScale(.5f, .5f);

    setMoveSpeed(HORIZONTAL_MOVE_SPEED);
  };

  /**
   * @brief Destroys the Walker object.
   * 
   */
  ~Walker() {}

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /**
   * @brief Returns the type of this sub class.
   *
   * @return Type Walker
   */
  Type getType() const override { return Type::Walker; }

  void update(float factor) override;

  sf::FloatRect getGlobalBounds() const override {
    sf::FloatRect rect = sf::Sprite::getGlobalBounds();
    rect.height = 77.5f;
    return rect;
  }

  virtual void horizontalMove(float factor) override;

  virtual void setMoveDirRight(float factor) override {
    Enemy::setMoveDirRight(factor);
    this->last_animation = 1;
  }

  virtual void setMoveDirLeft(float factor) override {
    Enemy::setMoveDirLeft(factor);
    this->last_animation = 1;
  }

  // ----------------------------------------- //
  /* Animation Methods */
  // ----------------------------------------- //

  void toggleAnimation() override;

 private:
  void animateIdle();

  void animateWalk();
};

}  // namespace Characters