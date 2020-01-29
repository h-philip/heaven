/**
 * @file editor.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Editor class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <stdio.h>
#include <time.h>

#include <SFML/Graphics.hpp>
#include <fstream>

#include "../../tinyxml2/tinyxml2.h"
#include "../help_overlay.h"
#include "../key_handler.h"
#include "../world.h"

namespace Editor {

/** \class Editor
 *  @brief Class for creating simple creating of new scenes.
 */
class Editor {
 private:
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  static const std::string SCENE_DIR; /**< Standard directory for scene files */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  sf::RenderWindow* window = nullptr;      /**< Editor window */
  std::vector<Grounds::Ground*> grounds;   /**< Vector for all grounds */
  std::vector<Characters::Enemy*> enemies; /**< Vector for all enemies */
  std::vector<Collectables::Collectable*>
      collectables;                        /**< Vector for all collectables */
  sf::RectangleShape* blueprint = nullptr; /**< Blueprint for mouse etc. */
  bool mouse_pressed = false, /**< Mouse is pressed at the moment *and*
                                   blueprint is being printed. */
      portal_mode = false; /**< Whether a destination for a portal is currently
                                being set */
  sf::Vector2f start_pos; /**< Original "position" of the blueprint (where mouse
                               was pressed down first) */
  uint8_t ground_type = 1; /**< Currently used Ground type. 1 = solid_ground,
                                2 = bad_ground, 3 = portal, 4 = enemy_border,
                                5 = buyable_button */
  uint8_t enemy_type =
      0; /**< Currently used Enemy type. 0 = shooter, 1 = walker */
  uint8_t collectable_type =
      0; /**< Currently used Collectable type. 0 = coin */
  bool
      select_key_pressed =
          false, /**< Whether the key to select grounds or enemies is pressed */
      move_key_pressed =
          false, /**< Whether the key to move selection is pressed */
      show_next_scene =
          false, /**< Whether the start of the next scene should be displayed */
      show_help = false; /**< Whether the help overlay should be displayed */
  std::vector<Grounds::Ground*>
      selected_grounds, /**< A vector with all currently selected grounds */
      next_scene;       /**< A vector with the grounds of the next scene */
  std::vector<Characters::Enemy*>
      selected_enemies; /**< A vector with all currently selected enemies */
  std::vector<Collectables::Collectable*>
      selected_collectables;   /**< A vector with all currently selected enemies
                                */
  sf::Vector2f last_mouse_pos; /**< Last position of the mouse pointer to
                                  calculate difference */
  std::vector<uint8_t> last_change;  /**< The order and type of last change. 0 =
                                        Ground, 1 = Enemy, 2 = Collectable */
  Grounds::Portal* portal = nullptr; /**< Ponter to last portal */
  Grounds::BuyableButton* buyable_button =
      nullptr; /**< Pointer to last buyable_button */

  World* world = nullptr;              /**< World for this editor */
  HelpOverlay* help_overlay = nullptr; /**< The overlay with keybindings */

  Characters::Factory* character_factory;     /**< Factory for characters */
  Collectables::Factory* collectable_factory; /**< Factory for collectables */
  Grounds::ButtonFactory* button_factory;     /**< Factor for buttons */

  char* load_scene; /**< The scene to load at start. nullptr if no scene to load
                     */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //
 public:
  /**
   * @brief Constructs a new Editor object.
   *
   */
  Editor(sf::VideoMode video_mode, char* scene) : load_scene(scene) {
    this->window = new sf::RenderWindow(video_mode, "Heaven - Editor",
                                        sf::Style::Fullscreen);
    this->blueprint = new sf::RectangleShape();

    this->next_scene.push_back(
        new Grounds::SolidGround(sf::FloatRect(100, -170, 300, 70)));
    this->next_scene.push_back(
        new Grounds::SolidGround(sf::FloatRect(1520, -170, 300, 70)));
    for (auto ground : this->next_scene) {
      for (auto sprite : ground->getSprites()) {
        sprite->setColor(sf::Color(255, 255, 255, 100));
      }
    }

    sf::View view = this->window->getView();
    view.move(sf::Vector2f(0, -1080));
    this->window->setView(view);

    this->world =
        new World(this->window, new Characters::Player(sf::Vector2f(0, 0)),
                  World::State::Editor);
    this->character_factory = this->world->getCharacterFactory();
    this->collectable_factory = this->world->getCollectablesFactory();
    this->button_factory = new Grounds::ButtonFactory();

    this->help_overlay = new HelpOverlay(this->window, true);
  }

  /**
   * @brief Destroys the Editor object.
   *
   */
  ~Editor() {
    delete this->help_overlay;
    delete this->world;
  }

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //
  /** @fn run
   * @brief The "main" method for the editor.
   *
   * This method will run until the editor is "finished".
   */
  int run();

 private:
  /* @fn mouseDown
   * @brief Changes current Ground type or "creates" new blueprint.
   *
   * @param event The mouse down event.
   *
   * @return Status code. 0 = OK, Everything else: end editor
   */
  int mouseDown(sf::Event& event);

  /** @fn mouseUp
   * @brief Saves a new Ground to grounds with the values from blueprint.
   *
   * @param event The mouse up event.
   *
   * @return Status code. 0 = OK, Everything else: end editor
   */
  int mouseUp(sf::Event& event);

  /** @fn mouseMove
   * @brief Changes the size and if needed the position of blueprint.
   *
   * @param event The mouse move event.
   *
   * @return Status code. 0 = OK, Everything else: end editor
   */
  int mouseMove(sf::Event& event);

  /** @fn save
   * @brief Saves the current grounds and enemies to a xml file named after the
   * current time.
   *
   * The file name will be printed to cout.
   *
   * @return true On success
   * @return false On error
   */
  bool save();

  /** @fn load
   * @brief Loads a scene from xml file.
   *
   * The file name must be give through standard input or as argument.
   */
  void load(char* scene_name);

  /** @fn keyPressed
   * @brief Switches between different keys for event handling.
   *
   * @param event The event.
   *
   * @return Status code. 0 = OK, Everything else: end editor
   */
  int keyPressed(sf::Event& event);

  /** @fn keyReleased
   * @brief Switches between different keys for event handling.
   *
   * @param event The event.
   *
   * @return Status code. 0 = OK, Everything else: end editor
   */
  int keyReleased(sf::Event& event);

  /**
   * @brief Moves the screen when scrolling.
   *
   * @param event The event.
   *
   * @return Status code. 0 = OK, Everything else: end editor
   */
  int mouseScroll(sf::Event& event);

  /** @fn getGround
   * @brief Returns a pointer to the ground at the world position.
   *
   * @param x World position x.
   * @param y World position y.
   * @return Pointer to ground at position or nullptr.
   */
  Grounds::Ground* getGround(float x, float y);

  /**
   * @brief Returns a pointer to the enemy at the world position.
   *
   * @param x Screen position x.
   * @param y Screen position y.
   * @return Pointer to enemy at position or nullptr.
   */
  Characters::Enemy* getEnemy(float x, float y);

  /**
   * @brief Returns a pointer to the collectable at the world position.
   *
   * @param x Screen position x.
   * @param y Screen position y.
   * @return Pointer to collectable at position or nullptr.
   */
  Collectables::Collectable* getCollectable(float x, float y);

  /**
   * @brief Calculates and sets the new y position for the next_scene grounds.
   */
  void setNextSceneGrounds();

  /**
   * @brief Clears all memory for this Editor.
   *
   * @param code The exit or return code. Default = 0
   * @param call_exit Whether 'exit(code)' should be called.
   */
  void end(int code = 0, bool call_exit = true);

  /**
   * @brief Starts the current world as a test.
   *
   * @param start_position The position to place the player at.
   */
  void testRun(const sf::Vector2f& start_position);
};
}  // namespace Editor