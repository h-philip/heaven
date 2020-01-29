/**
 * @file collectable.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Collectable class
 * @version 0.1
 * @date 2019-12-12
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "collectable.h"

namespace Collectables {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* draw() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Collectable::draw(sf::RenderWindow* window) { window->draw(*this); }

///////////////////////////////////////////////////////////////////////////////////////////////////
/* collect() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Collectable::collect() { this->is_collected = true; }
}  // namespace Collectables