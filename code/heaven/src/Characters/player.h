/**
 * @file player.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Player class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <iostream>
#include <queue>

#include "../Collectables/coin.h"
#include "character.h"

namespace Characters {

/** \class Player
 *  @brief This is the class for the player character.
 */
class Player : public Character {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  static const sf::Vector2f
      START_POSITION, /**< The default start position for players */
      NAME_OFFSET;    /**< Relative position of name string */
  static const std::string SPRITE_FILE, /**< The sprite file for texture */
      FONT_FILE;                        /**< The font file for names */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 protected:
  static sf::Font name_font;  /**< The font used for the name above
                                   the player */
  static sf::Texture texture; /**< The texture for this player */
  uint16_t coin_counter = 0;  /**< Counter for collected coins */
  sf::Text name; /**< Name displayed above the player in multiplayer */

  std::queue<Collectables::Collectable*>
      to_delete_collectables; /**< Collectables that need to be delted.
                              This is used to allow collectables to play
                              sound after they were collected. */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Player object.
   *
   * @param position The position to start at.
   * @param is_local Whether this player is a local one.
   */
  Player(sf::Vector2f position, bool is_local = true);

  /**
   * @brief Destroys the Player object.
   *
   */
  ~Player() {}

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /**
   * @brief The name of this sub class.
   *
   * @return Type Player
   */
  Type getType() const override { return Type::Player; }

  void update(float factor) override;

  void transformCollisionOn(sf::FloatRect& rect) const override;

  void die() override;

  /**
   * @brief Check if a collectable should be collected and does so if yes.
   *
   * @param collectable The collectable which may be collected.
   *
   * @return true If the collectable is collected.
   * @return false If it isn't.
   */
  virtual bool collect(Collectables::Collectable* collectable);

  const uint16_t getCoins() const { return this->coin_counter; }

  virtual void setPosition(const sf::Vector2f& position) override;

  virtual void setPosition(float x, float y) override {
    setPosition(sf::Vector2f(x, y));
  }

  virtual void draw(sf::RenderWindow* window) const override;

  virtual void move(const sf::Vector2f& offset) override;

  virtual void move(float x, float y) override { move(sf::Vector2f(x, y)); }

  virtual void takeDamage(int damage = 1) override;

  /**
   * @brief Sets the hp to new_hp and sends the info to all other players (in
   * network).
   *
   * Note: This method does the same as setHp() except it also sends the info
   * over network!
   *
   * @param new_hp The new hp for this player.
   */
  virtual void heal(uint8_t new_hp);

  // ----------------------------------------- //
  /* Getter / Setter */
  // ----------------------------------------- //

/**
 * @brief Sets the name for multiplayer.
 * 
 * It will be displayed above the player.
 * 
 * @param n The name.
 */
  void setName(std::string n) { this->name.setString(n); }

  void setState(State state) override;

  void setHorizontalMovement(int value) override;

  void addCoins(int value);

  // ----------------------------------------- //
  /* Animation Methods */
  // ----------------------------------------- //

  void toggleAnimation() override;

 private:
  void animateIdle();

  void animateWalk();

  void animateJump();

  void animateDie();

  void animateShoot();
};
}  // namespace Characters
