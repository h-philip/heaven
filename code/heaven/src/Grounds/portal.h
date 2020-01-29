/**
 * @file portal.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Portal class.
 * @version 0.1
 * @date 2019-11-22
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "ground.h"

namespace Grounds {

/** \class Portal
 * @brief Ground type that can teleport the player to another position.
 *
 */
class Portal : public Ground {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  static sf::Texture base_texture;     /**< Texture for all portals */
  static const std::string SOUND_FILE; /**< File with sound for all portals */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 protected:
  static sf::SoundBuffer sound_buffer; /**< Sound buffer for all portals */
  sf::Sound sound;                     /**< Sound for this portal */

  sf::Vector2f destination; /**< The offset where to teleport the player to. */
  bool draw_dest = false;   /**< Whether the destination should be drawn (for
                               editor and debugging) */
  sf::Vertex dest_line[2];  /**< Line to show destination in editor */

  sf::Clock timeout_clock; /**< Clock for timeout between multiple contacts with
                              the portal */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Portal object.
   *
   * @param rect Position and Size of portal
   * @param destination Destination of teleportation
   */
  Portal(const sf::FloatRect& rect, const sf::Vector2f& destination)
      : Ground(rect), destination(destination) {
    if (this->base_texture.getSize().x == 0)
      if (!this->base_texture.loadFromFile(
              "Ressources/Images/Grounds/portal.png"))
        exit(1);

    if (this->sound_buffer.getDuration().asMicroseconds() == 0)
      if (!this->sound_buffer.loadFromFile(SOUND_FILE)) exit(1);
    this->sound.setBuffer(this->sound_buffer);
    this->sound.setPosition((rect.left - 960) * .5f, rect.top, 0);
    this->sound.setMinDistance(500);
    this->sound.setAttenuation(4);

    // Base
    this->sprites.push_back(
        new sf::Sprite(this->base_texture, sf::IntRect(0, 0, 100, 100)));
    this->sprites[0]->setPosition(rect.left, rect.top);
    this->sprites[0]->setScale(rect.width / 100.f, rect.height / 100.f);

    // Dest line
    this->dest_line[0] = sf::Vertex(sf::Vector2f(rect.left + rect.width * .5f,
                                                 rect.top + rect.height * .5f),
                                    sf::Color::Red);
    this->dest_line[1] = sf::Vertex(
        sf::Vector2f(rect.left, rect.top) + destination, sf::Color::Red);
  }

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  std::string name() const override { return "portal"; }

  void contact(Characters::Character* c, float factor);

  bool canCollide(const Characters::Character* c) const override {
    return true;
  }

  void draw(sf::RenderWindow* window) const override;

  void move(const sf::Vector2f& offset) override;

  void setPosition(const sf::Vector2f& position) override;

  void moveDestination(const sf::Vector2f& offset) {
    this->dest_line[1].position += offset;
    this->destination += offset;
  }

  // ----------------------------------------- //
  /* Getter / Setter */
  // ----------------------------------------- //

  void enableDrawDest() { this->draw_dest = true; }

  sf::Vector2f getDestination() const { return this->destination; }
};
}  // namespace Grounds
