/**
 * @file world.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for the World class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cstdlib>
#include <fstream>

#include "../tinyxml2/tinyxml2.h"
#include "Buyables/buyable_factory.h"
#include "Characters/character.h"
#include "Characters/character_factory.h"
#include "Characters/player.h"
#include "Collectables/collectable_factory.h"
#include "Grounds/Buttons/button_factory.h"
#include "Grounds/bad_ground.h"
#include "Grounds/enemy_border.h"
#include "Grounds/portal.h"
#include "Grounds/solid_ground.h"

namespace Network {
class NetworkManager;
class Host;
class Client;
}  // namespace Network

/** \class World
 * @brief This class handles everything world-related.
 *
 * This includes loading and creating the world from files, setting up the
 * buttons and so on.
 */
class World {
 public:
  typedef std::tuple<std::vector<Grounds::Ground*>,
                     std::vector<Characters::Enemy*>,
                     std::vector<Collectables::Collectable*>>
      scene_t;

  enum class State : uint8_t {
    Local,  /**< Singleplayer */
    Host,   /**< Multiplayer as host */
    Client, /**< Multiplayer as client */
    Editor  /**< Used in editor */
  };

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  static const std::string
      SCENE_LIST_FILE,  /**< The file path of the scene list file. */
      SPAWN_SCENE_FILE; /**< Scene file for spawn */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 private:
  sf::RenderWindow* window = nullptr; /**< A pointer to the window */

  std::vector<Characters::Enemy*> enemies;  /**< All Enemies of this World.
                                                 Sorted after  y-Pos  */
  std::vector<Characters::Player*> players; /**< The Player of this World */
  std::vector<Grounds::Ground*> grounds;    /**< All grounds in this world.
                                                 Sorted after y-Pos */

  std::vector<std::string> scene_list; /**< List of files with scenes that can
                                            be used for random loading */
  std::map<Characters::Character*, std::vector<Grounds::Ground*>>
      colliding_grounds; /**< Map for storing the grounds which are currently
                              colliding with the the Players */
  std::vector<Collectables::Collectable*>
      collectables; /**< Vector with all collectables */

  State state; /**< Current state */

  Characters::Factory character_factory;      /**< Factory for characters */
  Collectables::Factory collectables_factory; /**<  Factory for collectables */
  Grounds::ButtonFactory buttons_factory;     /**< Factor for buttons */

  std::vector<scene_t> current_scenes; /**< The scenes that are currently
                                          loaded in the right order */

  Grounds::SolidGround*
      lower_border; /**< Ground that acts as the a lower border for the world.
                         It will be moved when players move up */

  bool started = false; /**< Determines whether the game started when in
                           multiplayer as host */

  std::vector<std::string>* scene_order = nullptr;
  size_t scene_index = 0;

  uint16_t race_distance = 0;
  sf::Clock race_clock;
  sf::RectangleShape race_destination;

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /** @fn World
   * @brief The default constructor.
   *
   * @param window A pointer to the window.
   * @param player A pointer to the (main) player. The Player-object will be
   * deleted when deleting this World-object.
   */
  World(sf::RenderWindow* window, Characters::Player* player,
        State state = State::Local);

  World(sf::RenderWindow* window, Characters::Player* player, char* scenes, State state = State::Local);

  /**
   * @brief Destroys the World object.
   *
   */
  ~World() {
    for (size_t i = 0; i < this->enemies.size(); i++) {
      delete this->enemies[i];
    }
    for (size_t i = 0; i < this->grounds.size(); i++) {
      delete this->grounds[i];
    }
    for (size_t i = 0; i < this->players.size(); i++) {
      delete this->players[i];
    }
    for (size_t i = 0; i < this->collectables.size(); i++) {
      delete this->collectables[i];
    }

    delete this->scene_order;
  }

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  /** @fn update
   * @brief Updates everything in this world.
   *
   * @param factor The time factor.
   * @return The vertical movement of the Player.
   */
  float update(float factor);

  /** @fn draw
   * @brief Draws this World with all Drawables belonging to it.
   *
   * @param window The window to draw in.
   */
  void draw(sf::RenderWindow* window);

  /** @fn init
   * @brief Initializes the World.
   *
   * This means loading all scenes and other objects needed for the start.
   */
  void init();

  /** @fn loadScene
   * @brief Loads a scene from file.
   *
   * @param file The file from where to load the scene.
   * @param offset Vertical offset for positions.
   * @return A pair with a vector with all loaded grounds and a vector with all
   * loaded enemies.
   */
  scene_t loadScene(std::string file, float offset);

  /** @fn addEnemy
   * @brief Adds a new Enemy to this world.
   *
   * @param e Pointer to the Enemy to add. The Enemy object will be
   * deleted when deleting this World-object.
   *
   * @return The index of the new enemy in enemies or -1 on failure.
   */
  int addEnemy(Characters::Enemy* e);

  /** @fn addPlayer
   * @brief Adds a new Player to this world.
   *
   * @param p Pointer to the Player to add.
   *
   * @return The index of the new player in players or -1 on failure.
   */
  int addPlayer(Characters::Player* p);

  /** @fn addGround
   * @brief Adds a new ground shape to this World.
   *
   * WARNING: This will not check the order of the grounds!
   *          The new ground will simply be pushed back on grounds!
   *
   * @param p Pointer to the Ground to add.
   *
   * @return The index of the new ground in grounds or -1 on failure.
   */
  int addGround(Grounds::Ground* sprite);

  /**
   * @brief Adds a new collectable to this World.
   *
   * @param p Pointer to the Collectable to add.
   *
   * @return The index of the new collectable in collectables or -1 on failure.
   */
  int addCollectable(Collectables::Collectable* collectable);

  /**
   * @brief Toggles animations of objects in this world.
   *
   */
  void toggleAnimation();

  /**
   * @brief Returns a vector with strings to add to the log window.
   *
   * This contains not only World-class logs but also all logs from every
   * Character, Ground, etc.
   *
   * @return Vector with strings to log
   */
  std::vector<std::string> getLogText();

  /**
   * @brief Resets this world.
   *
   * This is used to prepare the world for receiving Packets when acting as a
   * client.
   *
   * @param state The new state
   */
  void reset(State state);

  /**
   * @brief Removes a collectable.
   *
   * This is intendet for use when the collectable is collected
   * or the scene was removed.
   *
   * @param collectable The collectable to remove
   */
  void removeCollectable(const Collectables::Collectable* collectable);

  /**
   * @brief Removes a ground.
   *
   * @param ground The ground to remove
   */
  void removeGround(const Grounds::Ground* ground);

  /**
   * @brief Removes an enemy.
   *
   * @param enemy The enemy to remove
   */
  void removeEnemy(const Characters::Enemy* enemy);

  /**
   * @brief Removes the player p from this world.
   *
   * Note: This won't free the memory of p.
   *
   * @param p Pointer to the player.
   */
  void deletePlayer(Characters::Player* p) {
    int16_t index = -1;
    for (uint8_t i = 0; i < this->players.size(); i++)
      if (this->players[i] == p) index = i;
    if (index == -1) return;
    this->players.erase(this->players.begin() + index);
  }

  /**
   * @brief Restarts the clock for a race.
   */
  void startRace() {
    this->race_clock.restart();
    this->started = true;
  }

  // ----------------------------------------- //
  /* Private Methods */
  // ----------------------------------------- //
 private:
  /** @fn loadSchemeList
   * @brief Reads the SCENE_LIST_FILE and saves all entries into scene_list.
   */
  void loadSceneList();

  /** @fn loadNextScene
   * @brief Loads a random scene from the scene list into the next spot.
   */
  void loadNextScene();

  /** @fn moveCharacters
   * @brief Handles moving for all Characters in this world.
   *
   * @param factor The time factor.
   * @return The vertical movement of the Player.
   */
  float moveCharacters(float factor,
                       std::vector<Characters::Character*> characters);

  /**
   * @brief Wrapper for moveCharacters with enemies.
   *
   * @param factor The time factor.
   * @param enemies The vector of enemies to move.
   */
  void moveEnemies(float factor, std::vector<Characters::Enemy*> enemies) {
    std::vector<Characters::Character*> characters;
    for (auto e : enemies) characters.push_back(e);
    moveCharacters(factor, characters);
  }

  /**
   * @brief Wrapper for moveCharacters with players.
   *
   * @param factor The time factor
   * @param players The vector of players to move
   * @return float The value of vertical movement of the main player or 0 if not
   * in players.
   */
  float movePlayers(float factor, std::vector<Characters::Player*> players) {
    std::vector<Characters::Character*> characters;
    for (auto e : players)
      if (e->isLocal()) characters.push_back(e);
    return moveCharacters(factor, characters);
  }

  /**
   * @brief Removes all grounds, enemies and collectables of this scene from
   * global variables.
   *
   * The scene will not modified. No pointers will be deleted.
   *
   * @param scene Pointer to the scene.
   */
  void removeScene(const scene_t* scene);

  // ----------------------------------------- //
  /* Getter / Setter */
  // ----------------------------------------- //
 public:
  std::vector<Characters::Player*>& getPlayers() { return this->players; }

  Characters::Player* getPlayer(size_t index) { return this->players[index]; }

  std::vector<Grounds::Ground*>& getGrounds() { return this->grounds; }

  std::vector<Characters::Enemy*>& getEnemies() { return this->enemies; }

  std::vector<Collectables::Collectable*>& getCollectables() {
    return this->collectables;
  }

  void setGrounds(std::vector<Grounds::Ground*> grounds) {
    this->grounds = grounds;
  }

  void setEnemies(std::vector<Characters::Enemy*> enemies) {
    this->enemies = enemies;
  }

  void setCollectables(std::vector<Collectables::Collectable*> collectables) {
    this->collectables = collectables;
  }

  void setPlayers(std::vector<Characters::Player*> players) {
    this->players = players;
  }

  sf::RenderWindow* getWindow() { return this->window; }

  Characters::Factory* getCharacterFactory() {
    return &this->character_factory;
  }

  Collectables::Factory* getCollectablesFactory() {
    return &this->collectables_factory;
  }

  bool isStarted() const { return this->started; }

  void setState(State state) { this->state = state; }

  void setRaceDistance(uint16_t distance);
};
