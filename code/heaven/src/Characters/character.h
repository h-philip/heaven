/**
 * @file character.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Character class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>

#include "../Buyables/buyable.h"
#include "../Collectables/collectable.h"
#include "character_helper.h"
#include "vert_movement.h"

namespace Grounds {
class Ground;
}  // namespace Grounds

namespace Network {
class Host;
class Client;
}  // namespace Network

/**
 * @brief Namespace for all Characters.
 *
 */
namespace Characters {

/** \class Character
 *  @brief This is the base class for all enemys and players.
 */
class Character : public sf::Sprite {
 public:
  enum class State : uint8_t { Idle, Walk, Jump, Shoot, Die, Dead };
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  static const float
      HORIZONTAL_MOVE_SPEED,   /**< Horizontal movement speed for characters */
      MAX_ABS_FALL_SPEED,      /**< Maximum falling speed for characters */
      JUMP_ACCEL,              /**< Acceleration at which characters move
                               upwards when jumping */
      HIGH_JUMP_MULTIPLIER,    /**< The multiplier by which the JUMP_ACCEL
                               will be applyed when trying to jump higher while in
                               the air */
      MAX_JUMP_HIGHER_TIME_MS, /**< The time for which the Character can jump
                               higher (see HIGH_JUMP_MULTIPLIER). The applied
                               force will lower with time to zero when time is
                               over. */
      DEFAULT_WIDTH,           /**< The default width for Characters (shape) */
      DEFAULT_HEIGHT;          /**< The default height for Characters (shape) */
  static const uint8_t
      MAX_HP; /**< The maximum health points for this Character */
  static const sf::Color
      DAMAGE_COLOR; /**< Color for "animating" taking damage */

  static const std::string
      FOOTSTEPS_FILE, /**< Path to the file with sound for footsteps */
      JUMP_FILE,      /**< Path to the file with sound for jumping up */
      LAND_FILE; /**< Path to the file wit sound for landing after jumping */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 protected:
  VertMovement movement; /**< VertMovement object */
  sf::Clock jump_clock,  /**< The Clock for jump timing */
      damage_clock;      /**< The Clock for "animating" taking damage */
  sf::Texture* default_texture =
      nullptr; /**< The Texture used when no other one is specified*/
  sf::Vector2f fixed_move; /**< Can be set to move the Character in a fixed
                              direction (before other movement). */

  bool on_ground = false,      /**< Determines whether this Character
                               is currently standing on the ground */
      keeps_jumping = false,   /**< Determines whether this Character
                               currently wants to jump higher */
      can_keep_jumping = true, /**< Whether the Character can try to jump higher
                               (false when character jumps
                               against ceiling) */
      damage_colored = false, /**< Indicates whether this Character is currently
                              colored differently because
                              of taking damage */
      is_local = true;        /**< Determines whether this Character is a local.
                              This is used for network multiplayer and only applied to
                              players. */
  int8_t horizontal_movement = 0, /**< Movement value between -1 and +1
                                  etermining the movement
                                  direction with display dimensions */
      animation_counter = -1,     /**< Counter for loading the correct sprite */
      last_animation = 0;         /**< Code for the last animation in use. */
  int hp = MAX_HP;                /**< The current hp */

  float speed = 0,             /**< The horizontal movement speed */
      jump_power = JUMP_ACCEL; /**< The force to apply when start jumping */

  bool can_move = true; /**< Whether this character can move */

  State state = State::Idle; /**< Current state */

  static sf::SoundBuffer footsteps_buffer, /**< Sound buffer for footsteps */
      jump_buffer,                         /**< Sound buffer for jump */
      land_buffer;                         /**< Sound buffer for landing */
  sf::Sound footsteps,                     /**< Sound for footsteps */
      jump,                                /**< Sound for jumping */
      land;                                /**< Sound for landing */
  std::vector<Buyables::Buyable*>
      buyables; /**< All active buyables of this character */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Character object.
   *
   * @param position The position to start at.
   */
  Character(sf::Vector2f position);

  /**
   * @brief Destroys the Character object.
   *
   */
  virtual ~Character();

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /**
   * Returns the type of the derived class.
   */
  virtual Type getType() const = 0;

  /** @fn update
   * Updates everything for this Character.
   *
   * @param factor The factor which determining how much time (relative to a
   * second) has passed
   */
  virtual void update(float factor);

  /** @fn draw
   * Draws this Character and all Drawables belonging to this Character.
   *
   * @param window The window to draw in.
   */
  virtual void draw(sf::RenderWindow* window) const;

  /** @fn updateMovement
   * Updates the vertical movement for this Character.
   *
   * @param factor The factor which determining how much time (relative to a
   * second) has passed
   */
  virtual void updateMovement(float factor);

  /** @fn transformCollisionOn
   * Applies transformations needed for collision detection on the given
   * FloatRect.
   *
   * @param rect The FloatRect to transform.
   */
  virtual void transformCollisionOn(sf::FloatRect& rect) const {};

  /**
   * @brief Lets this Character take damage.
   *
   * @param damage The amount of damage.
   */
  virtual void takeDamage(int damage = 1);

  /**
   * @brief Lets this Character die.
   *
   */
  virtual void die();

  /**
   * @brief Gets the Global Bounds object.
   *
   * @return sf::FloatRect The global bounds.
   */
  virtual sf::FloatRect getGlobalBounds() const;

  /**
   * @brief Gets the distance to a position.
   *
   * @param position The position.
   * @return float The distance.
   */
  float getDistanceTo(sf::Vector2f position) const;

  /**
   * @brief Returns a vector with strings to add to the log window.
   *
   * @return Vector with strings to log
   */
  std::vector<std::string> getLogText();

  /**
   * @brief Adds a buyable to this character.
   *
   * @brief This will do nothing if the same buyable is already inside the
   * vector.
   *
   * @param buyable The buyable to add.
   */
  void addBuyable(Buyables::Buyable* buyable);

  /**
   * @brief Calculates and returns the position where to draw a buyable icon.
   *
   * @param index The index of the buyable.
   *
   * @return The position in pixel coordinates where to draw.
   */
  sf::Vector2i getBuyableIconPos(int index) const;

  // ----------------------------------------- //
  /* Movement Methods */
  // ----------------------------------------- //

  /** @fn setJump
   * @brief Starts jumping if the character is currently standing on ground.
   *
   * Applys a small force if character is currently in the air (to control
   * jumping higher). Stops keep jumping if value = false.
   *
   * @param value Whether to start or "stop" jumping.
   * @return true If Character started jumping; otherwise false
   */
  virtual bool setJump(bool value);

  /** @fn setMoveDirRight
   * Sets variables to signalize that this Character currently wants to move
   * right.
   */
  virtual void setMoveDirRight(float factor);

  /** @fn setMoveDirLeft
   * Sets variables to signalize that this Character currently wants to move
   * left.
   */
  virtual void setMoveDirLeft(float factor);

  /** @fn touchGround
   * Sets variables to signalize that this Character is back on the floor.
   *
   * If the Character wants to jump again, the setJump method will be called and
   * the correct animation will be set.
   */
  virtual void touchGround();

  /** @fn touchCeiling
   * Resets variable keeping jumping and resets the speed to directly start
   * falling again.
   */
  virtual void touchCeiling();

  /** @fn horizontalMove
   * Moves this character horizontal.
   *
   * @param factor The factor which determines how much time (relative to a
   * second) has passed.
   */
  virtual void horizontalMove(float factor);

  /** @fn getHorizontalMove
   * Returns the global bounds for the next horizontal movement.
   *
   * This method won't actually move this Character.
   */
  virtual sf::FloatRect getHorizontalMove(float factor) const;

  /** @fn verticalMove
   * Moves this character vertical.
   *
   * @param factor The factor which determines how much time (relative to a
   *        second) has passed.
   * @return The moved distance.
   */
  virtual float verticalMove(float factor);

  /** @fn getVerticalMove
   * Returns the global bounds for the next vertical movement.
   *
   * This method won't actually move this Character.
   */
  virtual sf::FloatRect getVerticalMove(float factor) const;

  /** @fn canMoveTo
   * Checks if this Character can move to a new position.
   *
   * @param new_pos The new global_bounds.
   * @param possible_colliding_grounds A vector with all Grounds that could be
   *        colliding with this Character.
   * @param[out] collided_head Pointer to bool for writing whether this
   *             Characters head collided.
   * @param[out] collided_grounds Apointer to write that the Character is
   *             colliding with.
   * @return True if Character can move there; false otherwise.
   */
  virtual bool canMoveTo(
      sf::FloatRect new_global_bounds,
      std::vector<Grounds::Ground*> possible_colliding_grounds,
      std::vector<Grounds::Ground*>* collided_grounds,
      bool* collided_head) const;

  virtual void setPosition(const sf::Vector2f& position);

  virtual void setPosition(float x, float y) {
    setPosition(sf::Vector2f(x, y));
  }

  virtual void move(const sf::Vector2f& offset);

  virtual void move(float x, float y) { move(sf::Vector2f(x, y)); }

  // ----------------------------------------- //
  /* Animation Methods */
  // ----------------------------------------- //

  /**
   * @brief Toggles the animation for this Character.
   *
   */
  virtual void toggleAnimation(){};

  // ----------------------------------------- //
  /* Getter / Setter */
  // ----------------------------------------- //

  virtual int getHP() const { return this->hp; };

  virtual void setHP(int hp);

  virtual bool canKeepJumping() const { return this->can_keep_jumping; }

  virtual void canKeepJumping(bool value) { this->can_keep_jumping = value; }

  /**
   * @brief Sets the fixed move to apply at next movement.
   *
   * This is used for portals to apply a special movement on a character so it
   * will be moved there.
   *
   * @param move The move offset.
   */
  virtual void setFixedMove(const sf::Vector2f& move) {
    this->fixed_move = move;
  }

  /**
   * @brief Sets the horizontal movement.
   *
   * This also changes the state if needed and checks for dead character first.
   *
   * @param value The new horizontal movement.
   */
  virtual void setHorizontalMovement(int value);

  /**
   * @brief Whether a character is able to move at all.
   *
   * @return true Is able to move.
   * @return false Is not able to move at all.
   */
  virtual bool canMove() const { return this->can_move; }

  virtual float getVerticalSpeed() const {
    return this->movement.current_speed;
  }

  virtual bool onGround() const { return this->on_ground; }

  /**
   * @brief Sets whether this characer is on ground.
   *
   * This also sets the state if needed.
   *
   * @param value Whether this character is on ground.
   */
  virtual void setOnGround(bool value);

  /**
   * @brief Returns whether this character is a local one.
   *
   * In multiplayer this on applies to the one which is conntrolled by the
   * player.
   *
   * @return true If local.
   * @return false If not local (remote).
   */
  bool isLocal() const { return this->is_local; }

  /**
   * @brief Sets the state
   *
   * This also calls touchGround() or setJump() for non-local characters
   * if needed.
   *
   * @param state The new state.
   */
  virtual void setState(State state);

  State getState() const { return this->state; }

  int8_t getDirection() const { return this->horizontal_movement; }

  void setMoveSpeed(float speed) { this->speed = speed; }

  /**
   * @brief Changes the jump power.
   * 
   * This is used to apply the JumpBoost buyable on a character.
   * 
   * @param value The value to change.
   */
  void modifyJumpPower(float value) { this->jump_power += value; }
};
}  // namespace Characters