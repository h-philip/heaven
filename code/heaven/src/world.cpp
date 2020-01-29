/**
 * @file world.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for World class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "world.h"

#include "Network/client.h"
#include "Network/host.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Static Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string World::SCENE_LIST_FILE = "Ressources/scene_list.txt",
                  World::SPAWN_SCENE_FILE = "Ressources/Scenes/spawn.xml";

///////////////////////////////////////////////////////////////////////////////////////////////////
/* World() */
///////////////////////////////////////////////////////////////////////////////////////////////////
World::World(sf::RenderWindow* window, Characters::Player* player, State state)
    : window(window),
      grounds(),
      scene_list(),
      state(state),
      character_factory(this->grounds, this->players, this->window),
      lower_border(
          new Grounds::SolidGround(sf::FloatRect(0, 1180, 1920, 500))) {
  addPlayer(player);
  addGround(this->lower_border);

  sf::Listener::setPosition(0, player->getPosition().y, 0);
  sf::Listener::setGlobalVolume(50);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/* World() */
///////////////////////////////////////////////////////////////////////////////////////////////////
World::World(sf::RenderWindow* window, Characters::Player* player, char* scenes,
             State state)
    : World(window, player, state) {
  this->scene_order = new std::vector<std::string>();
  size_t last_index = 0, len = strlen(scenes);
  for (size_t i = 0; i < len; i++) {
    if (scenes[i] == ',') {
      scenes[i] = '\0';
      this->scene_order->push_back("Ressources/Scenes/" +
                                   std::string(scenes + last_index));
      last_index = i + 1;
    }
  }
  this->scene_order->push_back("Ressources/Scenes/" +
                               std::string(scenes + last_index));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* update() */
///////////////////////////////////////////////////////////////////////////////////////////////////
float World::update(float factor) {
  if (this->state != State::Editor) {
    // Check if all players are higher then currently first loaded scene
    if (this->current_scenes.size() > 0) {
      bool scene_done = true;
      scene_t& first_scene = this->current_scenes[0];
      std::vector<Grounds::Ground*>& grounds = std::get<0>(first_scene);
      for (auto player : this->players) {
        if (grounds.size() == 0 ||
            player->getPosition().y + player->getGlobalBounds().height >=
                grounds[grounds.size() - 1]->getGlobalBounds().top) {
          scene_done = false;
          break;
        }
      }
      if (scene_done) {
        float top = grounds[grounds.size() - 1]->getGlobalBounds().top;
        removeScene(&first_scene);
        this->lower_border->setPosition(sf::Vector2f(0, top));
        // Change pos in network
        if (this->state == State::Host) {
          ((Network::Host*)Network::NetworkManager::getInstance())
              ->doGroundSetPos(this->lower_border);
        }
        this->current_scenes.erase(this->current_scenes.begin());
      }
    }

    // Check if any player is higher then lowest ground in last loaded scene
    if (this->current_scenes.size() > 0) {
      bool need_new_scene = false;
      scene_t& last_scene =
          this->current_scenes[this->current_scenes.size() - 1];
      std::vector<Grounds::Ground*>& grounds = std::get<0>(last_scene);
      for (auto player : this->players) {
        if (grounds.size() == 0 ||
            player->getPosition().y < grounds[0]->getGlobalBounds().top) {
          need_new_scene = true;
          break;
        }
      }
      if (need_new_scene) {
        loadNextScene();
      }
    }
  }

  if (this->state == State::Local || this->state == State::Host) {
    // Update Collectables
    for (auto collectable : this->collectables) collectable->update(factor);

    // Check collectables for colliding
    for (auto player : this->players) {
      // For each collectable
      for (size_t i = 0; i < this->collectables.size(); i++) {
        // If player intersects with it
        if (player->getGlobalBounds().intersects(
                this->collectables[i]->getGlobalBounds())) {
          // if player wants to collect it
          if (player->collect(this->collectables[i])) {
            // Apply to network
            if (this->state == State::Host) {
              ((Network::Host*)Network::NetworkManager::getInstance())
                  ->doCollectableCollected(this->collectables[i], player);
            }
            // Remove and change i
            this->collectables.erase(this->collectables.begin() + i);
            i--;
          }
        }
      }
    }
  }

  // Move enemies
  for (auto enemy : this->enemies) enemy->update(factor);
  if (this->state != State::Editor)
    if (this->enemies.size() > 0) moveEnemies(factor, this->enemies);

  // Move players
  for (auto player : this->players)
    player->update(factor);  // Update vertical speed of Player
  float return_value = movePlayers(factor, this->players);

  // "Contact" with each ground that is contacting a player
  for (auto c : this->colliding_grounds) {
    for (auto g : c.second) {
      g->contact(c.first, factor);
    }
  }

#ifdef DEBUG
  if (this->state != State::Editor)
    // Check if grounds are in correct order
    for (size_t i = 1; i < this->grounds.size(); i++) {
      if (this->grounds[i]->getGlobalBounds().top >
          this->grounds[i - 1]->getGlobalBounds().top)
        std::cerr << "Error in world: Grounds not in order! Index: " << i - 1
                  << " and " << i
                  << ". pos_x: " << this->grounds[i]->getGlobalBounds().top
                  << " and " << this->grounds[i - 1]->getGlobalBounds().top
                  << std::endl;
    }
#endif

  // Editor: set hp to max
  if (this->state == State::Editor) {
    for (auto p : this->players) p->setHP(Characters::Player::MAX_HP);
  }

  // Host: Check if race has a winner
  else if (this->state == State::Host && this->race_distance > 0) {
    // Get highest player
    Characters::Player* highest_player = getPlayer(0);
    for (auto p : this->players) {
      if (p->getPosition().y > highest_player->getPosition().y)
        highest_player = p;
    }

    // Check if highest player won
    if (highest_player->getPosition().y < this->race_distance * -100) {
      ((Network::Host*)Network::NetworkManager::getInstance())
          ->doDeclareWinner(highest_player,
                            this->race_clock.getElapsedTime().asMilliseconds());
    }
  }

  return return_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* draw() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void World::draw(sf::RenderWindow* window) {
  // Grounds
  if (this->grounds.size() > 0)
    for (size_t i = 0; i < this->grounds.size(); i++) {
      this->grounds[i]->draw(window);
    }

  if (this->enemies.size() > 0)
    for (size_t i = 0; i < this->enemies.size(); i++) {
      this->enemies[i]->draw(window);
    }

  for (auto player : this->players) {
    player->draw(window);
  }

  for (auto collectable : this->collectables) {
    collectable->draw(window);
  }

  // Draw race destination if set
  if (this->race_distance > 0) {
    window->draw(this->race_destination);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* addEnemy() */
///////////////////////////////////////////////////////////////////////////////////////////////////
int World::addEnemy(Characters::Enemy* c) {
  int index = (int)this->enemies.size();
  for (Characters::Enemy* existing_char :
       this->enemies)  // Check for already existing in vector
    if (existing_char == c) return false;

  this->enemies.push_back(c);

  // Add to network
  if (Network::NetworkManager::getInstance() &&
      Network::NetworkManager::getInstance()->isHost()) {
    ((Network::Host*)Network::NetworkManager::getInstance())->doAddEnemy(c);
  }

  if (c->canMove()) c->setMoveDirRight(0);

  return index;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* addPlayer() */
///////////////////////////////////////////////////////////////////////////////////////////////////
int World::addPlayer(Characters::Player* p) {
  int index = (int)this->players.size();
  for (Characters::Player* existing_char :
       this->players)  // Check for already existing in vector
    if (existing_char == p) return false;

  this->players.push_back(p);
  return index;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* addGround() */
///////////////////////////////////////////////////////////////////////////////////////////////////
int World::addGround(Grounds::Ground* sprite) {
  int index = (int)this->grounds.size();
  this->grounds.push_back(sprite);

  // Add to network
  if (Network::NetworkManager::getInstance() &&
      Network::NetworkManager::getInstance()->isHost()) {
    ((Network::Host*)Network::NetworkManager::getInstance())
        ->doAddGround(sprite);
  }
  return index;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* moveCharacters() */
///////////////////////////////////////////////////////////////////////////////////////////////////
float World::moveCharacters(float factor,
                            std::vector<Characters::Character*> characters) {
  float return_value = 0;
  for (auto c : characters) {
    // Skip if character can't move
    if (!c->canMove()) continue;

    bool colliding_grounds_was_reset = false;

    // ----------------------------------------- //
    /* Horizontal movement */
    // ----------------------------------------- //

    sf::FloatRect prev_bounds = c->getGlobalBounds(),
                  new_bounds = c->getHorizontalMove(factor);

    // Absolute move distance (horizontal)
    float abs_move_distance = prev_bounds.left - new_bounds.left;
    if (abs_move_distance < 0.f) abs_move_distance *= -1.f;
    std::vector<Grounds::Ground*> colliding;  // Stores colliding grounds

    // For loop lowering factor
    for (float i = 1; i < abs_move_distance; i++) {
      // Get global bounds for this factor
      new_bounds = c->getHorizontalMove(factor / i);

      // (Re-)set
      colliding = std::vector<Grounds::Ground*>();

      // Check if still inside window
      if (new_bounds.left > 0 &&
          new_bounds.left + new_bounds.width < this->window->getSize().x) {
        if (c->canMoveTo(new_bounds, this->grounds, &colliding, nullptr)) {
          c->horizontalMove(factor / i);
          // Colliding grounds
          if (this->colliding_grounds.count(c) == 0)
            colliding_grounds[c] = std::vector<Grounds::Ground*>();
          this->colliding_grounds[c] =
              colliding;  // Overwrite (reset) colliding_grounds
          colliding_grounds_was_reset = true;

          break;
        }
      }
    }

    // ----------------------------------------- //
    /* Vertical movement */
    // ----------------------------------------- //

    // Save position of character.
    // Call moveVertical to move the character.
    // Check if collision detected
    //  If yes -> revert changes and try again with smaller factor
    //  If no -> continue
    // c->move()

    prev_bounds = c->getGlobalBounds();
    new_bounds = c->getVerticalMove(factor);

    abs_move_distance =
        prev_bounds.top - new_bounds.top;  // Absolute move distance (vertical)
    if (abs_move_distance < 0.f) abs_move_distance *= -1.f;

    bool* collided_head = new bool;

    for (float i = 1; i < abs_move_distance; i++) {
      // Get global bounds for this factor
      new_bounds = c->getVerticalMove(factor / i);

      // (Re-)set
      colliding = std::vector<Grounds::Ground*>();

      if (c->canMoveTo(new_bounds, this->grounds, &colliding, collided_head)) {
        float vert_move_dist = c->verticalMove(factor / i);

        // Colliding grounds
        if (this->colliding_grounds.count(c) == 0)
          this->colliding_grounds[c] = std::vector<Grounds::Ground*>();
        if (!colliding_grounds_was_reset)  // If collding_grounds was not
                                           // reset yet
          this->colliding_grounds[c] = colliding;  // Overwrite it
        else  // Else add the new elements to it
          this->colliding_grounds[c].insert(this->colliding_grounds[c].end(),
                                            colliding.begin(), colliding.end());

        if (c == this->players[0]) {  // Current Character is main Player
          return_value = vert_move_dist;
        }
        if (c->onGround()) {  // Character didn't started jumping but was on
                              // ground before -> Is falling from something
          c->setOnGround(false);
          c->canKeepJumping(false);
        }
        break;
      } else {
        // Check if collision was on head or feet of Character
        if (*collided_head)
          c->touchCeiling();
        else
          c->touchGround();
      }
    }
    delete collided_head;
  }
  return return_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* removeScene */
///////////////////////////////////////////////////////////////////////////////////////////////////
void World::removeScene(const scene_t* scene) {
  const std::vector<Grounds::Ground*>& grounds = std::get<0>(*scene);
  const std::vector<Characters::Enemy*>& enemies = std::get<1>(*scene);
  const std::vector<Collectables::Collectable*>& collectables =
      std::get<2>(*scene);

  for (auto ground : grounds) {
    for (size_t i = 0; i < this->grounds.size(); i++) {
      if (ground == this->grounds[i]) {
        // Remove from network
        if (Network::NetworkManager::getInstance() &&
            Network::NetworkManager::getInstance()->isHost()) {
          ((Network::Host*)Network::NetworkManager::getInstance())
              ->doRemoveGround(ground);
        }
        // Remove from  global grounds
        this->grounds.erase(this->grounds.begin() + i);
        delete ground;
        break;
      }
    }
  }

  for (auto enemy : enemies) {
    for (size_t i = 0; i < this->enemies.size(); i++) {
      if (enemy == this->enemies[i]) {
        // Remove from network
        if (Network::NetworkManager::getInstance() &&
            Network::NetworkManager::getInstance()->isHost()) {
          ((Network::Host*)Network::NetworkManager::getInstance())
              ->doRemoveEnemy(enemy);
        }
        // Remove from global enemies
        this->enemies.erase(this->enemies.begin() + i);
        delete enemy;
        break;
      }
    }
  }

  for (auto collec : collectables) {
    for (size_t i = 0; i < this->collectables.size(); i++) {
      if (collec == this->collectables[i]) {
        // Remove from network
        if (this->state == State::Host) {
          ((Network::Host*)Network::NetworkManager::getInstance())
              ->doRemoveCollectable(collec);
        }
        // Remove from global collectables
        this->collectables.erase(this->collectables.begin() + i);
        delete collec;
        break;
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* init() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void World::init() {
  loadSceneList();

  scene_t scene = loadScene(World::SPAWN_SCENE_FILE, 0);
  this->current_scenes.push_back(scene);

  for (auto ground : std::get<0>(scene)) {
    addGround(ground);
  }
  for (auto e : std::get<1>(scene)) {
    this->addEnemy(e);
  }
  for (auto collectable : std::get<2>(scene)) {
    this->collectables.push_back(collectable);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* loadSceneList() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void World::loadSceneList() {
  std::fstream scene_file;
  scene_file.open(this->SCENE_LIST_FILE, std::ios::in);
  if (!scene_file.is_open()) {
    std::cerr << "Could not open file: " << this->SCENE_LIST_FILE << std::endl;
    exit(1);
  }

  std::vector<std::string> files;
  std::string line;
  while (std::getline(scene_file, line))
    if (line.size() > 0 && line[0] != '#') files.push_back(line);

  this->scene_list = files;

  // Check for empty scene_list file
  if (files.size() == 0) {
    std::cerr
        << "Error: The provided scene_list file doesn't contain any scenes!"
        << std::endl;
    exit(2);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* loadScene() */
///////////////////////////////////////////////////////////////////////////////////////////////////
World::scene_t World::loadScene(std::string file, float offset) {
  sf::Vector2u window_size = this->window->getSize();
#ifdef DEBUG
  std::cout << "New scene file: " << file << std::endl;
  if (file == "Ressources/Scenes/2019_11_21_23_7_52.xml") std::cout << "";
#endif
  std::vector<Grounds::Ground*> sprites;
  std::vector<Characters::Enemy*> enemies;
  std::vector<Collectables::Collectable*> collectables;
  using namespace tinyxml2;
  XMLDocument doc;
  if (doc.LoadFile(file.c_str()) != XML_SUCCESS) {
    std::cerr << "Could not load xml file: " << file << std::endl;
    exit(1);
  }
  XMLElement *prev_box, *next_box;

  // ----------------------------------------- //
  /* Grounds */
  // ----------------------------------------- //

  next_box = doc.FirstChildElement("scene");
  if (next_box == nullptr) {
    std::cerr << "No element \"scene\" in file: " << file << std::endl;
    exit(1);
  }
  next_box = next_box->FirstChildElement("grounds");
  if (next_box == nullptr) {
    std::cerr << "No element \"grounds\" in file: " << file << std::endl;
    exit(1);
  }
  next_box = next_box->FirstChildElement();
  if (next_box) do {
      sf::FloatRect rect;
      // -------- x-Position --------------------- //
      // Try get x pos in px
      if (next_box->QueryFloatAttribute("pos_x_px", &rect.left) !=
          XML_SUCCESS) {
        // No success
        // Get x pos in window sizes
        rect.left = next_box->FloatAttribute("pos_x_rel") * window_size.x;
      }
      // -------- y-Position -------------------- //
      // Try get y pos in px
      if (next_box->QueryFloatAttribute("pos_y_px", &rect.top) != XML_SUCCESS) {
        // No success
        // Get y pos in window sizes
        rect.top = next_box->FloatAttribute("pos_y_rel") * window_size.y;
      }
      // -------- x-Size ------------------------- //
      // Try get x size in px
      if (next_box->QueryFloatAttribute("size_x_px", &rect.width) !=
          XML_SUCCESS) {
        // No success
        // Get x size in window sizes
        rect.width = next_box->FloatAttribute("size_x_rel") * window_size.x;
      }
      // -------- y-Size ------------------------- //
      // Try get y size in px
      if (next_box->QueryFloatAttribute("size_y_px", &rect.height) !=
          XML_SUCCESS) {
        // No success
        // Get y size in window sizes
        rect.height = next_box->FloatAttribute("size_y_rel") * window_size.y;
      }
      // ----------------------------------------- //

      rect.top += offset;

      Grounds::Ground* new_ground;

      // Get ground type
      const char* ground_type = next_box->Attribute("type");
      if (Grounds::ButtonHelper::isButton(std::string(ground_type))) {
        new_ground = this->buttons_factory.createButton(
            Grounds::ButtonHelper::nameToType(std::string(ground_type)),
            sf::Vector2f(rect.left, rect.top));
        if (!new_ground) {
          if (strcmp(ground_type, "buyable_button") == 0) {
            const char* buyable_type = next_box->Attribute("buyable_type");
            new_ground = new Grounds::BuyableButton(
                sf::Vector2f(rect.left, rect.top),
                Buyables::Helper::nameToType(std::string(buyable_type)));
          }
        }
      } else if (strcmp(ground_type, "bad_ground") == 0) {
        new_ground = new Grounds::BadGround(rect);
      } else if (strcmp(ground_type, "solid_ground") == 0) {
        new_ground = new Grounds::SolidGround(rect);
      } else if (strcmp(ground_type, "portal") == 0) {
        // Get teleport destination
        sf::Vector2f destination;
        if (next_box->QueryFloatAttribute("dest_x", &destination.x) !=
            XML_SUCCESS)
          std::cerr
              << "Error while loading scene: Portal ground: No dest_x found!"
              << std::endl;
        if (next_box->QueryFloatAttribute("dest_y", &destination.y) !=
            XML_SUCCESS)
          std::cerr
              << "Error while loading scene: Portal ground: No dest_x found!"
              << std::endl;

        new_ground = new Grounds::Portal(rect, destination);
      } else if (strcmp(ground_type, "enemy_border") == 0) {
        new_ground = new Grounds::EnemyBorder(rect);
      } else {
        std::cerr << "Error while loading scene. Unknown ground type: "
                  << ground_type;
        exit(1);
      }

      // Set new_ground as start ground (gets removed by button) if attriute set
      //   and true
      bool start_ground = false;
      if (next_box->QueryBoolAttribute("start_ground", &start_ground) ==
          XMLError::XML_SUCCESS)
        if (start_ground) {
          Grounds::StartButton::setStartGround(this, new_ground);
        }

      if (rect.top > 0 && file != SPAWN_SCENE_FILE) {  // Not allowed
#ifdef DEBUG
        std::cerr << "Error while loading scene: y-Pos of ground > 0. Skipping."
                  << std::endl;
#endif
        delete new_ground;
      } else {
        // Insert at correct position
        uint16_t i;
        for (i = 0; i < sprites.size(); i++) {
          if (sprites[i]->getGlobalBounds().top <
              new_ground->getGlobalBounds().top)
            break;
        }
        sprites.insert(sprites.begin() + i, new_ground);
      }
      prev_box = next_box;
    } while ((next_box = prev_box->NextSiblingElement()) != nullptr);

  // ----------------------------------------- //
  /* Enemies */
  // ----------------------------------------- //

  next_box = doc.FirstChildElement("scene");
  if (next_box == nullptr) {
    std::cerr << "No element \"scene\" in file: " << file << std::endl;
    exit(1);
  }
  next_box = next_box->FirstChildElement("enemies");
  if (next_box == nullptr) {
    std::cerr << "No element \"enemies\" in file: " << file << std::endl;
    exit(1);
  }
  next_box = next_box->FirstChildElement();
  if (next_box) do {
      sf::Vector2f pos;
      // -------- x-Position --------------------- //
      // Try get x pos in px
      if (next_box->QueryFloatAttribute("pos_x_px", &pos.x) != XML_SUCCESS) {
        // No success
        // Get x pos in window sizes
        pos.x = next_box->FloatAttribute("pos_x_rel") * window_size.x;
      }
      // -------- y-Position -------------------- //
      // Try get y pos in px
      if (next_box->QueryFloatAttribute("pos_y_px", &pos.y) != XML_SUCCESS) {
        // No success
        // Get y pos in window sizes
        pos.y = next_box->FloatAttribute("pos_y_rel") * window_size.y;
      }
      // ----------------------------------------- //

      pos.y += offset;

      Characters::Enemy* new_enemy;

      // Get enemy type
      const char* enemy_type = next_box->Attribute("type");
      new_enemy = this->character_factory.createEnemy(
          Characters::Helper::nameToType(std::string(enemy_type)), pos);

      // Insert at correct position
      uint16_t i;
      for (i = 0; i < enemies.size(); i++) {
        if (enemies[i]->getGlobalBounds().top <
            new_enemy->getGlobalBounds().top)
          break;
      }
      enemies.insert(enemies.begin() + i, new_enemy);

      prev_box = next_box;
    } while ((next_box = prev_box->NextSiblingElement()) != nullptr);

  // ----------------------------------------- //
  /* Collectables */
  // ----------------------------------------- //

  next_box = doc.FirstChildElement("scene");
  if (next_box == nullptr) {
    std::cerr << "No element \"scene\" in file: " << file << std::endl;
    exit(1);
  }
  next_box = next_box->FirstChildElement("collectables");
  if (next_box == nullptr) {
    std::cerr << "No element \"collectables\" in file: " << file << std::endl;
    exit(1);
  }
  next_box = next_box->FirstChildElement();
  if (next_box) do {
      sf::Vector2f pos;
      // -------- x-Position --------------------- //
      // Try get x pos in px
      if (next_box->QueryFloatAttribute("pos_x_px", &pos.x) != XML_SUCCESS) {
        // No success
        // Get x pos in window sizes
        pos.x = next_box->FloatAttribute("pos_x_rel") * window_size.x;
      }
      // -------- y-Position -------------------- //
      // Try get y pos in px
      if (next_box->QueryFloatAttribute("pos_y_px", &pos.y) != XML_SUCCESS) {
        // No success
        // Get y pos in window sizes
        pos.y = next_box->FloatAttribute("pos_y_rel") * window_size.y;
      }
      // ----------------------------------------- //

      pos.y += offset;

      Collectables::Collectable* new_collectable;

      // Get collectable type
      const char* collectable_type = next_box->Attribute("type");
      new_collectable = this->collectables_factory.createCollectable(
          Collectables::Helper::nameToType(collectable_type), pos);

      // Insert at correct position
      uint16_t i;
      for (i = 0; i < collectables.size(); i++) {
        if (collectables[i]->getGlobalBounds().top <
            new_collectable->getGlobalBounds().top)
          break;
      }
      collectables.insert(collectables.begin() + i, new_collectable);

      prev_box = next_box;
    } while ((next_box = prev_box->NextSiblingElement()) != nullptr);

  return scene_t(sprites, enemies, collectables);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* loadNextScene() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void World::loadNextScene() {
  float offset = this->grounds[this->grounds.size() - 1]->getGlobalBounds().top;

  if (this->scene_order) {
    // Predefined order
    this->current_scenes.push_back(
        loadScene(this->scene_order->at(this->scene_index), offset));
    this->scene_index = (this->scene_index + 1) % this->scene_order->size();
  } else {
    // Random order
    srand((unsigned int)time(0));
    int random = rand() % (this->scene_list.size());

    this->current_scenes.push_back(loadScene(this->scene_list[random], offset));
  }

  scene_t& scene = this->current_scenes[this->current_scenes.size() - 1];

  for (auto ground : std::get<0>(scene)) addGround(ground);

  for (auto enemy : std::get<1>(scene)) addEnemy(enemy);

  for (auto collectable : std::get<2>(scene)) addCollectable(collectable);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* addCollectable() */
///////////////////////////////////////////////////////////////////////////////////////////////////
int World::addCollectable(Collectables::Collectable* collectable) {
  int index = (int)this->collectables.size();
  this->collectables.push_back(collectable);
  // Add to network
  if (Network::NetworkManager::getInstance() &&
      Network::NetworkManager::getInstance()->isHost()) {
    ((Network::Host*)Network::NetworkManager::getInstance())
        ->doAddCollectable(collectable);
  }
  return index;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* toggleAnimation() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void World::toggleAnimation() {
  for (auto e : this->enemies) {
    e->toggleAnimation();
  }
  for (auto p : this->players) {
    p->toggleAnimation();
  }
  for (auto c : this->collectables) {
    c->toggleAnimation();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getLogText() */
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> World::getLogText() {
  std::vector<std::string> all_logs;
  all_logs.push_back("Speed: " +
                     std::to_string((int)this->players[0]->getVerticalSpeed()));
  all_logs.push_back("y-Position: " +
                     std::to_string((int)this->players[0]->getPosition().y));
  for (auto player : this->players) {
    std::vector<std::string> logs = player->getLogText();
    for (std::string& log : logs) all_logs.push_back(log);
  }

  return all_logs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* reset() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void World::reset(State state) {
  for (auto enemy : this->enemies) delete enemy;
  this->enemies.clear();

  for (auto ground : this->grounds) delete ground;
  this->grounds.clear();

  for (auto collectable : this->collectables) delete collectable;
  this->collectables.clear();

  this->current_scenes.clear();

  sf::Vector2f pos = Characters::Player::START_POSITION;
  pos.x -= 32;
  pos.y -= 32;
  this->getPlayer(0)->setPosition(pos);

  this->state = state;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* removeCollectable() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void World::removeCollectable(const Collectables::Collectable* collectable) {
  for (size_t i = 0; i < this->collectables.size(); i++)
    if (this->collectables[i] == collectable) {
      this->collectables.erase(this->collectables.begin() + i);
      return;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* removeGround() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void World::removeGround(const Grounds::Ground* ground) {
  for (size_t i = 0; i < this->grounds.size(); i++)
    if (this->grounds[i] == ground) {
      this->grounds.erase(this->grounds.begin() + i);
      return;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* removeEnemy() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void World::removeEnemy(const Characters::Enemy* enemy) {
  for (size_t i = 0; i < this->enemies.size(); i++)
    if (this->enemies[i] == enemy) {
      this->enemies.erase(this->enemies.begin() + i);
      return;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setRaceDistance() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void World::setRaceDistance(uint16_t distance) {
  this->race_distance = distance;
  if (distance > 0) {
    this->race_destination = sf::RectangleShape(
        sf::Vector2f((float)this->window->getSize().x, 10.f));
    this->race_destination.setFillColor(sf::Color(255, 0, 0, 100));
    this->race_destination.setPosition(0, (float)distance * -100.f);
  } else {
    this->race_destination.setSize(sf::Vector2f(0, 0));
  }
}