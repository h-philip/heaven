#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>

#include "src/Network/client.h"
#include "src/Network/host.h"
#include "src/hud.h"
#include "src/key_handler.h"
#include "src/log_window.h"
#include "src/main_helper.h"
#include "src/world.h"

int main(int argc, char** argv) {
  // ----------------------------------------- //
  /* Window */
  // ----------------------------------------- //

  sf::RenderWindow* window = MainHelper::createWindow();
  sf::Texture background_texture = MainHelper::loadBackgroundTexture();
  sf::Sprite background;
  background.setTexture(background_texture, true);

  HelpOverlay help_overlay(window, false);

  // ----------------------------------------- //
  /* Args */
  // ----------------------------------------- //

  std::map<std::string, char*> args = MainHelper::parseArgs(argc, argv);

  if (args.count("ip") > 0) {
    Grounds::NetworkButton::press(true);
  }

  bool predefined_scenes = false;
  if (args.count("scenes") > 0) predefined_scenes = true;

#ifdef DEBUG
  std::cout << "Args: " << std::endl;
  for (auto c : args) std::cout << c.first << ":" << c.second << std::endl;
  std::cout << "---" << std::endl;
#endif

  // ----------------------------------------- //
  /* Player & World */
  // ----------------------------------------- //

  Characters::Player* player =
      new Characters::Player(Characters::Player::START_POSITION, true);

  World* world = nullptr;
  if (predefined_scenes)
    world = new World(window, player, args["scenes"]);
  else
    world = new World(window, player);

  world->init();

  Network::NetworkManager* manager = nullptr;

  // ----------------------------------------- //
  /* Music */
  // ----------------------------------------- //

  // sf::Music background_music;
  // background_music.openFromFile(
  //    "Ressources/Sounds/background.wav");
  // background_music.setVolume(50);
  // background_music.play();

  // ----------------------------------------- //
  /* Other */
  // ----------------------------------------- //

  sf::Clock main_clock, animation_clock;
  sf::View view = window->getView();
  HUD hud(window, player->MAX_HP);
  KeyHandler key_handler(window, player, world);

  LogWindow log_window(world);
  log_window.close();  // Set comment to show window

  float window_movement = 0;

  bool newly_connected = false;

  // Check if editor with file should be loaded
  if (args.count("editor") > 0) key_handler.startEditor(args["editor"]);

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* While Window Open */
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  while (window->isOpen()) {
    // Change animation
    if (animation_clock.getElapsedTime().asMilliseconds() > 80) {
      world->toggleAnimation();
      animation_clock.restart();
    }

    // If elapsed time is to low, sleep to "fix" to small factor
    if (main_clock.getElapsedTime().asMicroseconds() < 13000)
      sf::sleep(sf::microseconds(13000));
    // Get time of and restart main_clock
    sf::Int32 ms = main_clock.restart().asMilliseconds();
    float factor = ((float)(ms)) / 100.0f;
    if (newly_connected) {
      factor = 0.f;
      newly_connected = false;
    }

    // ----------------------------------------- //
    /* Event Switch */
    // ----------------------------------------- //

    sf::Event event;
    while (window->pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window->close();
          break;

        case sf::Event::KeyPressed:

#ifdef DEBUG
          if (event.key.code == sf::Keyboard::F4) {
            // ----------------------------------------- //
            /* Debug */
            // ----------------------------------------- //
            player->setHP(5);

            new Buyables::JumpBoost(player, 10);
          } else if (event.key.code == sf::Keyboard::F5) {
            if (manager) {
              // Set HP max for all players
              auto all_players = world->getPlayers();
              for (auto p : all_players) {
                p->heal(p->MAX_HP);
              }
            }
          }
#endif
          if (!key_handler.keyPressed(event, factor)) {
            delete window;
            return 0;
          }
          break;

        case sf::Event::KeyReleased:
          key_handler.keyReleased(event, factor);
          break;

        default:
          break;
      }
    }

    // Window stuff
    window->clear(sf::Color::White);
    window->draw(background);

    // Movement etc. (also drawing all objects inside world)
    window_movement += world->update(factor) * -1.f;
    float vertical_movement =
        window_movement * .1f;  // Makes the window movement "smooth"

    if (window_movement > 0) {  // If moving down
      vertical_movement += window_movement * window_movement / 2000;
    }
    window_movement -= vertical_movement;
    world->draw(window);

    if (manager) {
      manager->update();
    } else if (Grounds::NetworkButton::isPressed()) {
      window->setVisible(false);
      std::string ip;
      if (args.count("ip") == 0) {  // No ip through args
        std::cout << "Please enter the host ip (leave empty for hosting): ";
        std::getline(std::cin, ip);
        player->setName("0");
      } else {  // Ip through args
        ip = args["ip"];

        std::cout << "Using host ip provided by argument: " << ip << std::endl;
        player->setName("0");
      }
      if (ip.empty()) {
        // Host

        // Setup host
        world->setState(World::State::Host);
        Network::Host* host = new Network::Host(world);
        host->start();
        manager = host;

        // Ask for race
        std::cout << "Do you want to setup a race? (y/n) ";
        if (std::getchar() == 'y') {
          // Setup race
          int race;
          std::string race_dist;
          std::getline(std::cin, race_dist);
          while (true) {
            std::cout << "Please enter the height for the distance (positive "
                         "integer): ";
            std::getline(std::cin, race_dist);
            try {
              race = std::stoi(race_dist);
              if (race <= 0) throw std::invalid_argument("");
              break;
            } catch (const std::invalid_argument&) {
              std::cout << "Not a positive integer! ";
              continue;
            }
          }
          std::cout << "Race distance was set to: " << race << std::endl;

          // Got race dest, now apply
          world->setRaceDistance((uint16_t)race);
        } else {
          // No race
          std::cout << "No race was setup." << std::endl;
        }
      } else {
        // Client
        delete world;
        player =
            new Characters::Player(Characters::Player::START_POSITION, true);

        if (predefined_scenes)
          world =
              new World(window, player, args["scenes"], World::State::Client);
        else
          world = new World(window, player, World::State::Client);

        Network::Client* client =
            new Network::Client(world, world->getCharacterFactory(),
                                world->getCollectablesFactory());

        uint8_t i = 0;
        for (; i < 10; i++) {
          if (client->start(ip)) break;
        }
        if (i == 10) {
          // Failure
          delete client;
          std::cerr << "Error: Could not connect." << std::endl;
          delete world;
          delete window;
          return 1;
        }
        key_handler.setPlayer(player);
        manager = client;
        sf::Vector2f pos = Characters::Player::START_POSITION;
        pos.x -= 32;
        pos.y -= 32;
        world->getPlayer(0)->setPosition(pos);
        vertical_movement +=
            window->getDefaultView().getCenter().y - view.getCenter().y;
      }
      window->setVisible(true);
      newly_connected = true;
    }

    if (log_window.isOpen()) {
      log_window.addLine("Loop: " + std::to_string(ms));
      log_window.addLine("Vertical Movement: " +
                         std::to_string(vertical_movement));
      std::vector<std::string> logs = world->getLogText();
      for (std::string& line : logs) log_window.addLine(line);
    }

    // Adjust view and background for movement
    background.move(sf::Vector2f(0, vertical_movement));
    view.move(sf::Vector2f(0, vertical_movement));
    window->setView(view);

    sf::Vector3f audio_pos = sf::Listener::getPosition();
    audio_pos.y += vertical_movement;
    sf::Listener::setPosition(audio_pos);

    // HUD
    hud.drawPoints(vertical_movement * -1, player->getPosition().y * -1);
    hud.drawHP(vertical_movement * -1, player->getHP());
    hud.drawCoins(vertical_movement * -1, (int)player->getCoins());

    if (key_handler.showHelp()) help_overlay.draw();

    // Window stuff
    window->display();

    if (log_window.isOpen()) log_window.update();
  }
  delete world;
  delete window;
  return 0;
}