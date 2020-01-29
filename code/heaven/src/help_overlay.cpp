/**
 * @file help_overlay.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Immortality class
 * @version 0.1
 * @date 2020-01-15
 *
 *
 */
#include "help_overlay.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Static Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string HelpOverlay::EDITOR_HELP_FILE = "Ressources/editor_help.txt",
                  HelpOverlay::INGAME_HELP_FILE = "Ressources/ingame_help.txt",
                  HelpOverlay::FONT_FILE = "Ressources/monofonto.ttf";

///////////////////////////////////////////////////////////////////////////////////////////////////
/* HelpOverlay() */
///////////////////////////////////////////////////////////////////////////////////////////////////
HelpOverlay::HelpOverlay(sf::RenderWindow* window, bool editor)
    : window(window), editor(editor) {
  if (editor) {
    this->text.setString(loadHelpText(HelpOverlay::EDITOR_HELP_FILE));
    this->text.setFillColor(sf::Color(255, 255, 255, 255));
  } else {
    this->text.setString(loadHelpText(HelpOverlay::INGAME_HELP_FILE));
    this->text.setFillColor(sf::Color(0, 0, 0, 255));
  }

  this->text.setCharacterSize((int)(this->window->getSize().y * .04f));
  if (!this->font.loadFromFile(HelpOverlay::FONT_FILE)) exit(2);
  this->text.setFont(this->font);

  this->text.setOrigin(this->text.getGlobalBounds().width * .5f,
                       this->text.getGlobalBounds().height * .5f);

  this->default_position.x = (int)(this->window->getSize().x * .5f);
  this->default_position.y = (int)(this->window->getSize().y * .5f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* update() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void HelpOverlay::draw() {
  // Show help
  this->text.setPosition(
      this->window->mapPixelToCoords(this->default_position));
  this->window->draw(this->text);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* loadHelpText */
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string HelpOverlay::loadHelpText(std::string file) {
  // Load help text from file
  sf::FileInputStream stream;
  if (!stream.open(file)) exit(2);
  sf::Int64 size = stream.getSize();
  char* text_array = new char[size + 1];
  memset(text_array, ' ', size);
  text_array[size] = '\0';
  if (stream.read(text_array, size) != size) {
    std::cerr << "Error while reading file \"" << file
              << "\". Size not matching!" << std::endl;
  }
  sf::String text(text_array);
  return text.toAnsiString();
}
