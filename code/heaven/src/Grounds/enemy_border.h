/**
 * @file enemy_border.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for EnemyBorder class
 * @version 0.1
 * @date 2019-12-12
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "ground.h"

namespace Grounds {
/** \class EnemyBorder
 *  @brief Ground to act as a border that can't be passed by enemies but is
 * invisible and does not affect the player.
 */
class EnemyBorder : public Ground {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 private:
  sf::RectangleShape* editor_rect =
      nullptr; /**< Rectangle to be displayed while in edtior */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Enemy Border object.
   * 
   * @param rect Position and size of this ground
   * @param in_editor Whether this ground is currently in an editor
   */
  EnemyBorder(const sf::FloatRect& rect, bool in_editor = false)
      : Ground(rect) {
    if (in_editor) enableDraw();
  }

  /**
   * @brief Destroys the Enemy Border object.
   * 
   */
  ~EnemyBorder() {}

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  std::string name() const override { return "enemy_border"; }

  virtual void contact(Characters::Character* c, float factor){};

  virtual bool canCollide(const Characters::Character* c) const override {
    return c->getType() == Characters::Type::Player;
  }

  void draw(sf::RenderWindow* window) const override {
    if (editor_rect) window->draw(*this->editor_rect);
  }

  virtual void move(const sf::Vector2f& offset) override;

  void enableDraw();
};
}  // namespace Grounds