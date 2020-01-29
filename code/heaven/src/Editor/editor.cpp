/**
 * @file editor.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Editor class.
 * @version 0.1
 * @date 2019-11-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "editor.h"

namespace Editor {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Static Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string Editor::SCENE_DIR = "Ressources/Scenes/";

///////////////////////////////////////////////////////////////////////////////////////////////////
/* run() */
///////////////////////////////////////////////////////////////////////////////////////////////////
int Editor::run() {
  // Add line for original zero
  sf::RectangleShape zero(sf::Vector2f((float)this->window->getSize().x, 1));
  zero.setPosition(0, 0);
  zero.setFillColor(sf::Color::Red);

  if (this->load_scene) {
    // Load scene directly
    load(this->load_scene);
  } else {
    // Add grounds for start of scene
    this->grounds.push_back(
        new Grounds::SolidGround(sf::FloatRect(100, -170, 300, 70)));
    // No last change because this should always be there
    this->grounds.push_back(
        new Grounds::SolidGround(sf::FloatRect(1520, -170, 300, 70)));
    // No last change because this should always be there
  }

  while (this->window->isOpen()) {
    // ----------------------------------------- //
    /* Events */
    // ----------------------------------------- //
    sf::Event event;
    while (this->window->pollEvent(event)) {
      int code = 0;
      switch (event.type) {
        case sf::Event::Closed:
          this->window->close();
          break;

          // ----------------------------------------- //
          /* Key */
          // ----------------------------------------- //

        case sf::Event::KeyPressed:
          code = keyPressed(event);
          break;

        case sf::Event::KeyReleased:
          code = keyReleased(event);
          break;

          // ----------------------------------------- //
          /* Mouse */
          // ----------------------------------------- //

          // Mouse pressed
        case sf::Event::MouseButtonPressed:
          code = mouseDown(event);
          break;

          // Mouse released
        case sf::Event::MouseButtonReleased:
          code = mouseUp(event);
          break;

          // Mouse moved
        case sf::Event::MouseMoved:
          code = mouseMove(event);
          break;

        case sf::Event::MouseWheelScrolled:
          code = mouseScroll(event);
          break;

        default:
          break;
      }

      if (code != 0) {
        end(code, false);
        return code;
      }
    }

    // ----------------------------------------- //
    /* End of Events */
    // ----------------------------------------- //

    this->window->clear();

    // Draw grounds
    for (auto g : this->grounds) g->draw(this->window);

    // Draw enemies
    for (auto e : this->enemies) e->draw(this->window);

    // Draw collectables
    for (auto c : this->collectables) c->draw(this->window);

    // Draw blueprint if currently in work (mouse pressed)
    if (this->mouse_pressed) {
      this->window->draw(*this->blueprint);
    }

    // Draw zero line
    this->window->draw(zero);

    // Draw next scene
    if (this->show_next_scene) {
      for (auto ground : this->next_scene) ground->draw(this->window);
    }

    // Show help
    if (this->show_help) this->help_overlay->draw();

    this->window->display();
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* mouseDown() */
///////////////////////////////////////////////////////////////////////////////////////////////////
int Editor::mouseDown(sf::Event& event) {
  if (event.type != sf::Event::MouseButtonPressed) return 1;
  if (portal_mode) {
    // NOP
  } else if (this->select_key_pressed) {  // Select ground / enemy / collectable
    sf::Vector2f mouse_world = this->window->mapPixelToCoords(
        sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

    // First search collectables
    Collectables::Collectable* selected_collectable =
        getCollectable(mouse_world.x, mouse_world.y);
    if (!selected_collectable) {  // No collectable found

      // Try with enemy
      Characters::Enemy* selected_enemy =
          getEnemy(mouse_world.x, mouse_world.y);
      if (!selected_enemy) {  // No enemy found

        // Try with ground
        Grounds::Ground* selected_ground =
            getGround(mouse_world.x, mouse_world.y);
        if (!selected_ground)  // No ground found
          return 0;
        else {  // Ground found
          // Check if ground already selected
          int selected = -1;
          for (uint32_t i = 0; i < this->selected_grounds.size(); i++)
            if (this->selected_grounds[i] == selected_ground) {
              selected = i;
              break;
            }

          if (selected != -1) {  // Already selected
            // Undo color marking
            for (auto sprite : selected_ground->getSprites()) {
              sprite->setColor(sf::Color::White);
            }
            // Remove from selected
            this->selected_grounds.erase(this->selected_grounds.begin() +
                                         selected);
          } else {  // New selection
            // Set color marking
            for (auto sprite : selected_ground->getSprites()) {
              sprite->setColor(sf::Color::Red);
            }
            // Add to selected
            this->selected_grounds.push_back(selected_ground);
          }
        }
      } else {  // Enemy found
                // Check if enemy already selected
        int selected = -1;
        for (uint32_t i = 0; i < this->selected_enemies.size(); i++)
          if (this->selected_enemies[i] == selected_enemy) {
            selected = i;
            break;
          }

        if (selected != -1) {  // Already selected
          // Undo color marking
          selected_enemy->setColor(sf::Color::White);
          // Remove from selected
          this->selected_enemies.erase(this->selected_enemies.begin() +
                                       selected);
        } else {  // New selection
          // Set color marking
          selected_enemy->setColor(sf::Color::Red);
          // Add to selected
          this->selected_enemies.push_back(selected_enemy);
        }
      }
    } else {  // Collectable found
              // Check if collectable already selected
      int selected = -1;
      for (size_t i = 0; i < this->selected_collectables.size(); i++) {
        if (this->selected_collectables[i] == selected_collectable) {
          selected = (int)i;
          break;
        }
      }

      if (selected != -1) {  // Already selected
        // Undo color marking
        selected_collectable->setColor(sf::Color::White);
        // Remove from selected
        this->selected_collectables.erase(this->selected_collectables.begin() +
                                          selected);
      } else {  // New selection
        // Set color marking
        selected_collectable->setColor(sf::Color::Red);
        // Add to selected
        this->selected_collectables.push_back(selected_collectable);
      }
    }
  } else {  // New ground / enemy / coin

    // Left button
    if (event.mouseButton.button == sf::Mouse::Left) {
      // Directly place buyable_button
      if (this->ground_type == 5) {
        this->buyable_button = new Grounds::BuyableButton(
            this->window->mapPixelToCoords(
                sf::Vector2i(event.mouseButton.x, event.mouseButton.y)),
            Buyables::Type::JumpBoost);
        this->grounds.push_back(this->buyable_button);
        this->last_change.push_back(0);
      }

      // Set position for blueprint
      else {
        this->mouse_pressed = true;
        this->blueprint->setPosition(this->window->mapPixelToCoords(
            sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));
        this->blueprint->setSize(sf::Vector2f(0.f, 0.f));

        // For calculating position and size when mouse is upper left of
        // position
        this->start_pos = this->blueprint->getPosition();
      }
    }
    // Right button
    else if (event.mouseButton.button == sf::Mouse::Right) {
      // Create new enemy

      Characters::Enemy* new_enemy;
      switch (enemy_type) {
          // Shooter
        case 0:
          new_enemy = new Characters::Shooter(
              this->window->mapPixelToCoords(
                  sf::Vector2i(event.mouseButton.x, event.mouseButton.y)),
              this->world->getPlayers());
          break;

        case 1:
          new_enemy = new Characters::Walker(
              this->window->mapPixelToCoords(
                  sf::Vector2i(event.mouseButton.x, event.mouseButton.y)),
              this->world->getPlayers(), this->world->getGrounds(),
              this->window);
          break;
      }

      this->enemies.push_back(new_enemy);
      this->last_change.push_back(1);
    }  // Middle button
    // Middle button
    else if (event.mouseButton.button == sf::Mouse::Middle) {
      // Create new collectable

      Collectables::Collectable* new_collectable;
      switch (collectable_type) {
          // Coin
        case 0:
          new_collectable =
              new Collectables::Coin(this->window->mapPixelToCoords(
                  sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));
          break;
      }

      this->collectables.push_back(new_collectable);
      this->last_change.push_back(2);
    }
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* mouseUp() */
///////////////////////////////////////////////////////////////////////////////////////////////////
int Editor::mouseUp(sf::Event& event) {
  if (event.type != sf::Event::MouseButtonReleased) return 1;
  if (this->portal_mode)
    this->portal_mode = false;
  else if (this->select_key_pressed) {  // Select key
    // NOP
  } else if (this->mouse_pressed) {  // Blueprint
    // Left button
    if (event.mouseButton.button == sf::Mouse::Left) {
      // Save current blueprint to new ground

      this->mouse_pressed = false;
      sf::FloatRect rect;
      sf::Vector2f pos = this->blueprint->getPosition();
      rect.left = pos.x;
      rect.top = pos.y;
      sf::Vector2f size = this->blueprint->getSize();
      rect.width = size.x;
      rect.height = size.y;
      switch (this->ground_type) {
        case 1:  // solid_ground
          this->grounds.push_back(new Grounds::SolidGround(rect));
          this->last_change.push_back(0);
          break;

        case 2:  // bad_ground
          this->grounds.push_back(new Grounds::BadGround(rect));
          this->last_change.push_back(0);
          break;

        case 3:  // portal
          this->portal = new Grounds::Portal(
              rect, this->window->mapPixelToCoords(sf::Vector2i(
                        event.mouseButton.x, event.mouseButton.y)) -
                        sf::Vector2f(rect.left, rect.top));
          portal->enableDrawDest();
          this->grounds.push_back(this->portal);
          this->last_change.push_back(0);
          portal_mode = true;
          break;

        case 4:  // enemy_border
          this->grounds.push_back(new Grounds::EnemyBorder(rect, true));
          this->last_change.push_back(0);
          break;
      }
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* mouseMove() */
///////////////////////////////////////////////////////////////////////////////////////////////////
int Editor::mouseMove(sf::Event& event) {
  if (event.type != sf::Event::MouseMoved) return 1;
  if (this->portal_mode) {
    sf::Vector2f offset((float)event.mouseMove.x, (float)event.mouseMove.y);
    offset -= this->last_mouse_pos;

    this->portal->moveDestination(offset);
  } else if (this->move_key_pressed) {  // Move selection
    sf::Vector2f move_vector =
        sf::Vector2f((float)event.mouseMove.x, (float)event.mouseMove.y) -
        this->last_mouse_pos;

    for (auto ground : this->selected_grounds) {
      ground->move(move_vector);
    }

    for (auto enemy : this->selected_enemies) {
      enemy->move(move_vector);
    }

    for (auto collectable : this->selected_collectables) {
      collectable->move(move_vector);
    }
  }
  if (this->mouse_pressed) {  // Recalculate position and size of blueprint
    sf::Vector2f mouse_pos = this->window->mapPixelToCoords(
                     sf::Vector2i(event.mouseMove.x, event.mouseMove.y)),
                 new_pos, new_size;

    // Position (most upper left point)
    new_pos.x =
        mouse_pos.x < this->start_pos.x ? mouse_pos.x : this->start_pos.x;
    new_pos.y =
        mouse_pos.y < this->start_pos.y ? mouse_pos.y : this->start_pos.y;

    // Size (-> most lower right point)
    new_size.x =
        (mouse_pos.x > this->start_pos.x ? mouse_pos.x : this->start_pos.x) -
        new_pos.x;
    new_size.y =
        (mouse_pos.y > this->start_pos.y ? mouse_pos.y : this->start_pos.y) -
        new_pos.y;

    // Check maximum y size
    if (new_size.y > 1800) {
      float diff = new_size.y - 1800;
      if (new_pos.y < this->start_pos.y)  // Mouse if higher then start
        new_pos.y += diff;
      new_size.y -= diff;
    }

    this->blueprint->setPosition(new_pos);
    this->blueprint->setSize(new_size);
  }
  // Reset last_mouse_pos to current pos
  this->last_mouse_pos =
      sf::Vector2f((float)event.mouseMove.x, (float)event.mouseMove.y);

  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* save() */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool Editor::save() {
  // Save current scene to file

  // Skip if no grounds and no enemies
  if (this->grounds.size() == 0 && this->enemies.size() == 0) {
    std::cout << std::endl
              << "There are no grounds and no enemies. Nothing will be saved."
              << std::endl;
    return true;
  }

  using namespace tinyxml2;
  XMLDocument doc;
  doc.InsertFirstChild(doc.NewDeclaration());
  doc.InsertEndChild(doc.NewElement("scene"));  // Root element

  // ----------------------------------------- //
  /* Grounds */
  // ----------------------------------------- //

  XMLElement* g_node = doc.NewElement("grounds");
  doc.LastChild()->InsertFirstChild(g_node);

  // Add grounds to doc
  for (auto ground : this->grounds) {
    XMLElement* ele = doc.NewElement("ground");
    ele->SetAttribute("pos_x_px", std::round(ground->getGlobalBounds().left));
    ele->SetAttribute("pos_y_px", std::round(ground->getGlobalBounds().top));
    ele->SetAttribute("size_x_px", std::round(ground->getGlobalBounds().width));
    ele->SetAttribute("size_y_px",
                      std::round(ground->getGlobalBounds().height));
    ele->SetAttribute("type", ground->name().c_str());
    if (ground->name() == "portal") {
      ele->SetAttribute(
          "dest_x", std::round(((Grounds::Portal*)ground)->getDestination().x));
      ele->SetAttribute(
          "dest_y", std::round(((Grounds::Portal*)ground)->getDestination().y));
    } else if (ground->name() == "buyable_button") {
      ele->SetAttribute("buyable_type",
                        Buyables::Helper::typeToName(
                            ((Grounds::BuyableButton*)ground)->getBuyableType())
                            .c_str());
    } else if (Grounds::StartButton::getStartGround() == ground) {
      ele->SetAttribute("start_ground", true);
    }
    g_node->InsertEndChild(ele);
  }

  // ----------------------------------------- //
  /* Enemies */
  // ----------------------------------------- //

  XMLElement* e_node = doc.NewElement("enemies");
  doc.LastChild()->InsertAfterChild(g_node, e_node);

  // Add grounds
  for (auto enemy : this->enemies) {
    XMLElement* ele = doc.NewElement("enemy");
    ele->SetAttribute("pos_x_px", std::round(enemy->getPosition().x));
    ele->SetAttribute("pos_y_px", std::round(enemy->getPosition().y));
    ele->SetAttribute("type",
                      Characters::Helper::typeToName(enemy->getType()).c_str());
    e_node->InsertEndChild(ele);
  }

  // ----------------------------------------- //
  /* Collectables */
  // ----------------------------------------- //

  XMLElement* c_node = doc.NewElement("collectables");
  doc.LastChild()->InsertAfterChild(e_node, c_node);

  // Add collectables
  for (auto collectable : this->collectables) {
    XMLElement* ele = doc.NewElement("collectable");
    ele->SetAttribute("pos_x_px", std::round(collectable->getPosition().x));
    ele->SetAttribute("pos_y_px", std::round(collectable->getPosition().y));
    ele->SetAttribute(
        "type",
        Collectables::Helper::typeToName(collectable->getType()).c_str());
    c_node->InsertEndChild(ele);
  }

  // Ask for where to save
  std::string file_name;
  this->window->setVisible(false);
  std::cout << "Please enter the file name to save to (Ressources/Scenes/): ";
  std::getline(std::cin, file_name);
  if (!file_name.empty()) {
    // File name was provided
  } else {
    // Get current time as string
    time_t my_time = time(NULL);
    tm ltm;
#ifndef GCC
    localtime_s(&ltm, &my_time);
#else
    ltm = *localtime(&my_time);
#endif
    file_name.append(std::to_string(ltm.tm_year + 1900) + "_" +
                     std::to_string(ltm.tm_mon + 1) + "_" +
                     std::to_string(ltm.tm_mday) + "_");
    file_name.append(std::to_string(ltm.tm_hour) + "_" +
                     std::to_string(ltm.tm_min) + "_" +
                     std::to_string(ltm.tm_sec) + ".xml");
  }

  // Save
  doc.SaveFile(("Ressources/Scenes/" + file_name).c_str());
  std::cout << std::endl
            << "Saved xml-file to: "
            << "Ressources/Scenes/" << file_name << std::endl
            << std::endl;

  // Append new file to file with scene list
  std::ofstream outfile;
  outfile.open("Ressources/scene_list.txt", std::ios_base::app);
  outfile << std::endl << "Ressources/Scenes/" << file_name;

  // Check if warning for small scene is needed
  int highest = 0;
  for (auto g : this->grounds)
    if ((int)g->getGlobalBounds().top < highest)
      highest = (int)g->getGlobalBounds().top;
  if (highest > -1200) {
    std::cout << "Warning: The saved scene isn't that high. You "
                 "might want to add more grounds for a better experience."
              << std::endl
              << std::endl
              << std::endl
              << std::endl;
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* load() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Editor::load(char* scene_name = nullptr) {
  std::string file;
  if (!scene_name) {
    // Get file name
    this->window->setVisible(false);
    std::cout << "Please enter the file to load (Ressources/Scenes/): ";
    getline(std::cin, file);
    this->window->setVisible(true);
  } else
    file = std::string(scene_name);
  if (file.empty()) return;

  // Load xml into world
  World::scene_t scene = this->world->loadScene("Ressources/Scenes/" + file, 0);

  this->grounds.insert(this->grounds.end(), std::get<0>(scene).begin(),
                       std::get<0>(scene).end());

  this->enemies.insert(this->enemies.end(), std::get<1>(scene).begin(),
                       std::get<1>(scene).end());

  this->collectables.insert(this->collectables.end(),
                            std::get<2>(scene).begin(),
                            std::get<2>(scene).end());

  for (auto ground : this->grounds)
    if (ground->name() == "portal")
      ((Grounds::Portal*)ground)->enableDrawDest();
    else if (ground->name() == "enemy_border")
      ((Grounds::EnemyBorder*)ground)->enableDraw();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* keyPressed() */
///////////////////////////////////////////////////////////////////////////////////////////////////
int Editor::keyPressed(sf::Event& event) {
  if (event.type != sf::Event::KeyPressed) return 1;
  sf::View view;
  switch (event.key.code) {
      // ----------------------------------------- //
      /* View */
      // ----------------------------------------- //

      // Move view up
    case sf::Keyboard::Up:
      view = this->window->getView();
      view.move(sf::Vector2f(0.f, -30));
      this->window->setView(view);
      break;

      // Move view down
    case sf::Keyboard::Down:
      view = this->window->getView();
      view.move(sf::Vector2f(0.f, 30.f));
      this->window->setView(view);
      break;

      // Enter test mode
    case sf::Keyboard::P:
      testRun(this->window->mapPixelToCoords(sf::Mouse::getPosition()));
      break;

      // ----------------------------------------- //
      /* Save / Load */
      // ----------------------------------------- //

      // Save
    case sf::Keyboard::Enter:
      if (save()) return 1;
      break;

      // Load file
    case sf::Keyboard::L:
      load();
      break;

      // ----------------------------------------- //
      /* Modifications */
      // ----------------------------------------- //

      // Undo
    case sf::Keyboard::Backspace:
      this->portal_mode = false;
      if (this->last_change.size() == 0) break;  // Do nothing on no changes
      if (this->last_change[this->last_change.size() - 1] == 0) {  // Ground
        if (this->grounds.size() == 0) {
          // This should not happen :-)
          // But it happend just a moment ago, sooooooo
          // Yeah
        } else {
          // Iterate over selected grounds
          for (uint32_t i = 0; i < this->selected_grounds.size(); i++) {
            // Remove selected ground too
            if (this->selected_grounds[i] ==
                this->grounds[this->grounds.size() - 1]) {
              this->selected_grounds.erase(this->selected_grounds.begin() + i);
              break;
            }
          }
          if (this->grounds[this->grounds.size() - 1] == this->buyable_button)
            this->buyable_button = nullptr;
          delete this->grounds[this->grounds.size() - 1];
          this->grounds.pop_back();
        }
      } else if (this->last_change[this->last_change.size() - 1] ==
                 1) {  // Enemy
        if (this->enemies.size() == 0) {
          // This should not happen :-)
          // But it happend just a moment ago, sooooooo
          // Yeah
        } else {
          for (uint32_t i = 0; i < this->selected_enemies.size();
               i++) {  // Iterate over selected enemies
            if (this->selected_enemies[i] ==
                this->enemies[this->enemies.size() -
                              1]) {  // Remove selected enemy too
              this->selected_enemies.erase(this->selected_enemies.begin() + i);
              break;
            }
          }
          delete this->enemies[this->enemies.size() - 1];
          this->enemies.pop_back();
        }
      } else if (this->last_change[this->last_change.size() - 1] ==
                 2) {  // Collectable
        if (this->collectables.size() == 0) {
          // Shouldn't happen
        } else {
          for (size_t i = 0; i < this->selected_collectables.size(); i++) {
            if (this->selected_collectables[i] ==
                this->collectables[this->collectables.size() -
                                   1]) {  // Remove selected collectable too
              this->selected_collectables.erase(
                  this->selected_collectables.begin() + i);
              break;
            }
          }
          delete this->collectables[this->collectables.size() - 1];
          this->collectables.pop_back();
        }
      }
      this->last_change.pop_back();
      break;

      // Copy (duplicate) selection
    case sf::Keyboard::C:
      for (auto ground : this->selected_grounds) {
        if (Grounds::ButtonHelper::isButton(ground->name())) {
          Grounds::Button* b = this->button_factory->createButton(
              Grounds::ButtonHelper::nameToType(ground->name()),
              ground->getSprites()[0]->getPosition());
          if (!b) {
            if (((Grounds::Button*)ground)->getType() ==
                Grounds::ButtonType::BuyableButton) {
              b = new Grounds::BuyableButton(
                  ground->getSprites()[0]->getPosition(),
                  ((Grounds::BuyableButton*)ground)->getBuyableType());
            }
          }
          this->last_change.push_back(0);
        } else if (ground->name() == "solid_ground") {
          this->grounds.push_back(
              new Grounds::SolidGround(ground->getGlobalBounds()));
          this->last_change.push_back(0);
        } else if (ground->name() == "bad_ground") {
          this->grounds.push_back(
              new Grounds::BadGround(ground->getGlobalBounds()));
          this->last_change.push_back(0);
        } else if (ground->name() == "portal") {
          Grounds::Portal* new_portal =
              new Grounds::Portal(ground->getGlobalBounds(),
                                  ((Grounds::Portal*)ground)->getDestination());
          new_portal->enableDrawDest();
          this->grounds.push_back(new_portal);
          this->last_change.push_back(0);
        } else {
          std::cerr << "Error: Unknown Ground type while duplicating grounds: "
                    << ground->name() << std::endl;
        }
      }
      for (auto enemy : this->selected_enemies) {
        this->enemies.push_back(this->character_factory->createEnemy(
            enemy->getType(), enemy->getPosition()));
      }
      for (auto collectable : this->selected_collectables) {
        this->collectables.push_back(
            this->collectable_factory->createCollectable(
                collectable->getType(), collectable->getPosition()));
      }
      break;

      // Remove selected
    case sf::Keyboard::Delete:
      // Remove grounds
      for (auto ground : this->selected_grounds) {
        if (ground == this->buyable_button) this->buyable_button = nullptr;
        for (size_t i = 0; i < this->grounds.size(); i++) {
          if (ground == this->grounds[i]) {  // Remove
            this->grounds.erase(this->grounds.begin() + i);
            delete ground;
            i--;
            break;
          }
        }
      }
      this->selected_grounds.clear();

      // Remove enemies
      for (auto enemy : this->selected_enemies) {
        for (size_t i = 0; i < this->enemies.size(); i++) {
          if (enemy == this->enemies[i]) {  // Remove
            this->enemies.erase(this->enemies.begin() + i);
            delete enemy;
            i--;
            break;
          }
        }
      }
      this->selected_enemies.clear();

      // Remove collectables
      for (auto collectable : this->selected_collectables) {
        for (size_t i = 0; i < this->collectables.size(); i++) {
          if (collectable == this->collectables[i]) {  // Remove
            this->collectables.erase(this->collectables.begin() + i);
            delete collectable;
            i--;
            break;
          }
        }
      }
      break;

    // ----------------------------------------- //
    /* Toggle Keys */
    // ----------------------------------------- //

    // Select key
    case sf::Keyboard::LControl:
      this->select_key_pressed = true;
      break;

      // Move selection key
    case sf::Keyboard::LShift:
      this->move_key_pressed = true;
      break;

      // Show next scene start
    case sf::Keyboard::N:
      setNextSceneGrounds();
      this->show_next_scene = true;
      break;

      // Toggle buyable type of last buyable_button
    case sf::Keyboard::B:
      if (this->buyable_button)
        this->buyable_button->setBuyableType(Buyables::Factory::getNextType(
            this->buyable_button->getBuyableType()));
      break;

    case HelpOverlay::HELP_KEY:
      this->show_help = true;
      break;

      // ----------------------------------------- //
      /* Numbers */
      // ----------------------------------------- //

    case sf::Keyboard::Num1:
      this->ground_type = 1;  // Solid ground
      break;

    case sf::Keyboard::Num2:
      this->ground_type = 2;  // Bad ground
      break;

    case sf::Keyboard::Num3:
      this->ground_type = 3;  // Portal
      break;

    case sf::Keyboard::Num4:
      this->ground_type = 4;  // EnemyBorder
      break;

    case sf::Keyboard::Num5:
      this->ground_type = 5;  // BuyableButton
      break;

    case sf::Keyboard::Num6:
      break;

    case sf::Keyboard::Num7:
      break;

    case sf::Keyboard::Num8:
      break;

    case sf::Keyboard::Num9:
      this->enemy_type = 1;  // Walker
      break;

    case sf::Keyboard::Num0:
      this->enemy_type = 0;  // Shooter
      break;

      // ----------------------------------------- //
      /* Direct Placements */
      // ----------------------------------------- //

      // Place heart
    case sf::Keyboard::H:
      this->collectables.push_back(new Collectables::Heart(
          this->window->mapPixelToCoords(sf::Mouse::getPosition())));
      this->last_change.push_back(2);

    default:
      break;
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* keyReleased() */
///////////////////////////////////////////////////////////////////////////////////////////////////
int Editor::keyReleased(sf::Event& event) {
  if (event.type != sf::Event::KeyReleased) return 1;
  switch (event.key.code) {
    // Select key
    case sf::Keyboard::LControl:
      this->select_key_pressed = false;
      break;

      // Move selection key
    case sf::Keyboard::LShift:
      this->move_key_pressed = false;
      break;

      // Show next scene start
    case sf::Keyboard::N:
      this->show_next_scene = false;
      break;

    case HelpOverlay::HELP_KEY:
      this->show_help = false;
      break;

    default:
      break;
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* mouseScroll() */
///////////////////////////////////////////////////////////////////////////////////////////////////
int Editor::mouseScroll(sf::Event& event) {
  if (event.type != sf::Event::MouseWheelScrolled) return 1;
  sf::View view = this->window->getView();
  sf::Vector2f offset(0.f, event.mouseWheelScroll.delta * -50);
  view.move(offset);
  this->window->setView(view);

  if (this->portal_mode) {
    // Change destination of portal
    this->portal->moveDestination(offset);
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getGround() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Grounds::Ground* Editor::getGround(float x, float y) {
  for (int32_t i = (int32_t)this->grounds.size() - 1; i >= 0; i--) {
    auto bounds = this->grounds[i]->getGlobalBounds();
    if (x > bounds.left && y > bounds.top)
      if (x < bounds.left + bounds.width && y < bounds.top + bounds.height)
        return this->grounds[i];
  }
  return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getEnemy() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Characters::Enemy* Editor::getEnemy(float x, float y) {
  for (int32_t i = (int32_t)this->enemies.size() - 1; i >= 0; i--) {
    auto bounds = this->enemies[i]->getGlobalBounds();
    if (x > bounds.left && y > bounds.top)
      if (x < bounds.left + bounds.width && y < bounds.top + bounds.height)
        return this->enemies[i];
  }
  return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* getCollectable() */
///////////////////////////////////////////////////////////////////////////////////////////////////
Collectables::Collectable* Editor::getCollectable(float x, float y) {
  for (int32_t i = (int32_t)this->collectables.size() - 1; i >= 0; i--) {
    auto bounds = this->collectables[i]->getGlobalBounds();
    if (x > bounds.left && y > bounds.top)
      if (x < bounds.left + bounds.width && y < bounds.top + bounds.height)
        return this->collectables[i];
  }
  return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setNextSceneGrounds() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Editor::setNextSceneGrounds() {
  float max = 0;
  for (auto ground : this->grounds) {
    if (ground->getGlobalBounds().top < max)
      max = ground->getGlobalBounds().top;
  }
  for (auto ground : this->next_scene) {
    ground->setPosition(
        sf::Vector2f(ground->getGlobalBounds().left, -170 + max));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* end() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Editor::end(int code, bool call_exit) {
  for (size_t i = 0; i < this->enemies.size(); i++) {
    delete this->enemies[i];
  }
  this->enemies.clear();
  this->selected_enemies.clear();
  this->world->setEnemies(this->enemies);

  for (size_t i = 0; i < this->grounds.size(); i++) {
    delete this->grounds[i];
  }
  this->grounds.clear();
  this->selected_grounds.clear();
  this->world->setGrounds(this->grounds);

  this->portal = nullptr;
  for (size_t i = 0; i < this->next_scene.size(); i++) {
    delete this->next_scene[i];
  }
  this->next_scene.clear();
  delete this->blueprint;
  this->blueprint = nullptr;

  this->window->close();
  // delete window; // Commenting out seems to solve atio6axx.dll problems

  // Exit
  if (call_exit) exit(code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* testRun() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Editor::testRun(const sf::Vector2f& start_position) {
  this->world->getPlayer(0)->setPosition(start_position);
  this->world->getPlayer(0)->setHP(Characters::Player::MAX_HP);

  this->world->setGrounds(this->grounds);
  this->world->setEnemies(this->enemies);
  this->world->setCollectables(this->collectables);

  KeyHandler key_handler(this->window, this->world->getPlayer(0), this->world);

  sf::Clock main_clock, animation_clock;
  sf::View view = window->getView();
  float window_movement = 0;

  while (window->isOpen()) {
    // Change animation
    if (animation_clock.getElapsedTime().asMilliseconds() > 80) {
      this->world->toggleAnimation();
      animation_clock.restart();
    }

    // If elapsed time is to low, sleep to "fix" to small factor
    if (main_clock.getElapsedTime().asMicroseconds() < 13000)
      sf::sleep(sf::microseconds(13000));
    // Get time of and restart main_clock
    sf::Int32 ms = main_clock.restart().asMilliseconds();
    float factor = ((float)(ms)) / 100.0f;

    sf::Event event;
    while (window->pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window->close();
          break;

        case sf::Event::KeyPressed:
          if (event.key.code == sf::Keyboard::Escape) {
            return;
          }
          if (!key_handler.keyPressed(event, factor)) {
            delete window;
            exit(0);
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

    // Movement etc. (also drawing all objects inside world)
    window_movement += world->update(factor) * -1.f;
    float vertical_movement =
        window_movement * .1f;  // Makes the window movement "smooth"

    if (window_movement > 0) {  // If moving down
      vertical_movement += window_movement * window_movement / 2000;
    }
    window_movement -= vertical_movement;
    world->draw(window);

    // Adjust view for movement
    view.move(sf::Vector2f(0, vertical_movement));
    window->setView(view);

    // Window stuff
    window->display();
  }
}
}  // namespace Editor