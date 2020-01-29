/**
 * @file buyable_button.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for BuyableButton class.
 * @version 0.1
 * @date 2019-12-16
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "../../Buyables/buyable_factory.h"
#include "button.h"

namespace Grounds {
/** @class BuyableButton
 * @brief Button to buy a Buyable.
 *
 * The type of buyable is defined at creation.
 */
class BuyableButton : public Button {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  static const std::string TEXTURE_FILE; /**< File with textures for all buyable
                                            button "backgrounds" */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 private:
  static sf::Texture
      texture;         /**< Texture for all buyable button "backgrounds" */
  Buyables::Type type; /**< The type of the contained buyable */
  const Buyables::Factory buyables_factory; /**< Factory for buyables */

  Buyables::Buyable* buyable; /**< The buyable contained in this button */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new BuyableButton object.
   *
   * @param pos Position of the button
   */
  BuyableButton(const sf::Vector2f& pos, Buyables::Type type);

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  virtual ButtonType getType() const override;

  virtual void doAction(Characters::Player* p, float factor) override;

  Buyables::Type getBuyableType() const { return this->type; }

  virtual void draw(sf::RenderWindow* window) const override;

  virtual void setBuyableType(Buyables::Type t);
};
}  // namespace Grounds