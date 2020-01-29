/**
 * @file client.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Client class.
 * @version 0.1
 * @date 2019-11-29
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "client.h"

namespace Network {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* update() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::update() {
  if (this->player_ids.size() > 0) {
    if (this->update_clock.getElapsedTime().asMilliseconds() >=
        SEND_UPDATE_INTERVAL) {
      this->update_clock.restart();
      // Update position
      sf::Packet* position_packet =
          playerSetPosPacket(getPlayerId(this->world->getPlayer(0)),
                             this->world->getPlayer(0)->getPosition());
      this->send_tcp_packets.push(std::pair<sf::TcpSocket*, sf::Packet*>(
          this->tcp_socket, position_packet));
    }
  }

  while (!this->send_tcp_packets.empty()) {
    auto pair = this->send_tcp_packets.front();
    sendTcpPacket(pair.first, pair.second);
    delete pair.second;
    this->send_tcp_packets.pop();
  }
  receiveTcp();
  // sendUdp();
  // receiveUdp();
  handleReceived();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* start() */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool Client::start(sf::IpAddress host) {
#ifdef DEBUG
  std::cout << "Starting connection to " << host.toString() << "... ";
#endif
  this->host = host;
  this->tcp_socket->setBlocking(true);
  sf::Socket::Status status = this->tcp_socket->connect(
      this->host, this->tcp_port, sf::milliseconds(1000));
  switch (status) {
    case sf::Socket::Status::Disconnected:
      std::cerr << "Error while connecting tcp socket: Code " << status
                << " Disconnected." << std::endl;
      return false;
      break;

    case sf::Socket::Status::Error:
      std::cerr << "Error while connecting tcp socket: Code " << status
                << " Error." << std::endl;
      return false;
      break;

    default:
      break;
  }
  this->tcp_socket->setBlocking(false);

#ifdef DEBUG
  std::cout << "Done" << std::endl;
#endif
  // this->udp_socket->setBlocking(true);
  // status = this->udp_socket->bind(this->udp_port);
  // switch (status) {
  //  case sf::Socket::Status::Disconnected:
  //    std::cerr << "Error while binding udp socket: Code " << status
  //              << " Disconnected." << std::endl;
  // return false;
  //    break;

  //  case sf::Socket::Status::Error:
  //    std::cerr << "Error while binding udp socket: Code " << status
  //              << " Error." << std::endl;
  //    this->tcp_socket->disconnect();
  //    return false;
  //    break;
  //}
  // this->udp_socket->setBlocking(false);
  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doAddPlayer() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::doWantAddPlayer(Characters::Player* p) {
  sf::Packet* packet = wantAddPlayerPacket(p->getPosition());
  this->send_tcp_packets.push(
      std::pair<sf::TcpSocket*, sf::Packet*>(this->tcp_socket, packet));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doPlayerChangeState() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::doPlayerChangeState(const Characters::Player* p) {
  uint8_t id = getPlayerId(p);
  sf::Packet* packet = playerChangeStatePacket(id, p->getState());
  this->send_tcp_packets.push(
      std::pair<sf::TcpSocket*, sf::Packet*>(this->tcp_socket, packet));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doPlayerHorizontalDir() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::doPlayerHorizontalDir(const Characters::Player* p) {
  uint8_t id = getPlayerId(p);
  sf::Int8 dir = p->getDirection();
  sf::Packet* packet = playerHorizontalDirPacket(id, dir);
  this->send_tcp_packets.push(
      std::pair<sf::TcpSocket*, sf::Packet*>(this->tcp_socket, packet));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doPlayerTakeDamage() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::doPlayerTakeDamage(const Characters::Player* p, sf::Int8 damage) {
  uint8_t id = getPlayerId(p);
  sf::Packet* packet = playerTakeDamagePacket(id, damage);
  this->send_tcp_packets.push(
      std::pair<sf::TcpSocket*, sf::Packet*>(this->tcp_socket, packet));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doPlayerHeal() */
///////////////////////////////////////////////////////////////////////////////////////////////////2
void Client::doPlayerHeal(const Characters::Player* p, sf::Uint8 new_hp) {
  sf::Uint8 id = getPlayerId(p);
  sf::Packet* packet = playerHealPacket(id, new_hp);
  this->send_tcp_packets.push(
      std::pair<sf::TcpSocket*, sf::Packet*>(this->tcp_socket, packet));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/* sendUdp() */
///////////////////////////////////////////////////////////////////////////////////////////////////
// void Client::sendUdp() {
//   // Send udp packets
//   while (!this->send_udp_packets.empty()) {
//     std::pair<sf::IpAddress*, sf::Packet*> send =
//         this->send_udp_packets.front();
//     sf::Socket::Status status =
//         this->udp_socket->send(*send.second, *send.first, this->udp_port);
//     switch (status) {
//       case sf::Socket::Status::Done:
//         break;

//       case sf::Socket::Status::Partial:
//         continue;
//         break;

//       case sf::Socket::Status::Disconnected:
//         std::cerr << "Error while sending udp packet: Code " << status
//                   << " Disconnected." << std::endl;
//         break;

//       case sf::Socket::Status::Error:
//         std::cerr << "Error while sending udp packet: Code " << status
//                   << " Error." << std::endl;
//         break;

//       case sf::Socket::Status::NotReady:
//         break;
//     }
//     this->send_udp_packets.pop();
//     delete send.second;
//   }
// }

///////////////////////////////////////////////////////////////////////////////////////////////////
/* receiveUdp() */
///////////////////////////////////////////////////////////////////////////////////////////////////
// void Client::receiveUdp() {
//   // Receive udp packets
//   sf::Packet* packet = new sf::Packet();
//   while (true) {
//     sf::Socket::Status status =
//         this->udp_socket->receive(*packet, this->host, this->udp_port);
//     switch (status) {
//       case sf::Socket::Status::Done:
//         this->receive_udp_packets.push(packet);
//         packet = new sf::Packet();
//         continue;
//         break;

//       case sf::Socket::Status::Partial:
//         break;

//       case sf::Socket::Status::Disconnected:
//         std::cerr << "Error while receiving udp packet: Code " << status
//                   << " Disconnected." << std::endl;
//         exit(1);
//         continue;
//         break;

//       case sf::Socket::Status::Error:
//         std::cerr << "Error while receiving udp packet: Code " << status
//                   << " Error." << std::endl;
//         break;

//       case sf::Socket::Status::NotReady:
//         break;
//     }
//     break;
//   }
//   delete packet;
// }

///////////////////////////////////////////////////////////////////////////////////////////////////
/* receiveTcp() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::receiveTcp() {
  // Receive tcp packets
  sf::Packet* packet = new sf::Packet();
  while (true) {
    sf::Socket::Status status = this->tcp_socket->receive(*packet);
    switch (status) {
      case sf::Socket::Status::Done:
        this->receive_tcp_packets.push(packet);
        packet = new sf::Packet();
        continue;
        break;

      case sf::Socket::Status::Partial:
        continue;
        break;

      case sf::Socket::Status::Disconnected:
        std::cerr << "Error while receiving tcp packet: Code " << status
                  << " Disconnected." << std::endl;
        exit(1);
        break;

      case sf::Socket::Status::Error:
        std::cerr << "Error while receiving tcp packet: Code " << status
                  << " Error." << std::endl;
        break;

      case sf::Socket::Status::NotReady:
        break;
    }
    break;
  }
  delete packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleWantAddPlayer() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleWantAddPlayer(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: AddPlayer must "
            << "not be sent to a client!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleAddPlayer() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleAddPlayer(sf::Packet* packet) {
  sf::Uint8 id;
  *packet >> id;
  sf::Int64 x, y;
  *packet >> x;
  *packet >> y;
  sf::Uint8 state;
  *packet >> state;
  sf::Int8 dir;
  *packet >> dir;
  Characters::Player* new_player =
      new Characters::Player(sf::Vector2f((float)x, (float)y), false);
  new_player->setState((Characters::Character::State)state);
  new_player->setHorizontalMovement(dir);
  new_player->setName(std::to_string(id));
  this->player_ids[id] = new_player;
  this->world->addPlayer(new_player);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleAcceptAddPlayer() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleAcceptAddPlayer(sf::Packet* packet) {
  sf::Uint8 id;
  *packet >> id;
  this->player_ids[id] = this->world->getPlayer(0);
  this->player_ids[id]->setName(std::to_string(id));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handlePlayerSetPos() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handlePlayerSetPos(sf::Packet* packet) {
  sf::Uint8 id;
  *packet >> id;
  sf::Int64 x, y;
  *packet >> x;
  *packet >> y;
  getPlayer(id)->setPosition((float)x, (float)y);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handlePlayerChangeState() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handlePlayerChangeState(sf::Packet* packet) {
  sf::Uint8 id;
  *packet >> id;
  sf::Uint8 state;
  *packet >> state;
  getPlayer(id)->setState((Characters::Character::State)state);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handlePlayerHorizontalDir() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handlePlayerHorizontalDir(sf::Packet* packet) {
  sf::Uint8 id;
  *packet >> id;
  sf::Int8 dir;
  *packet >> dir;
  if (dir < 0)
    getPlayer(id)->setMoveDirLeft(0);
  else if (dir > 0)
    getPlayer(id)->setMoveDirRight(0);
  else
    getPlayer(id)->setHorizontalMovement(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handlePlayerTakeDamage() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handlePlayerTakeDamage(sf::Packet* packet) {
  sf::Uint8 id;
  *packet >> id;
  sf::Int8 damage;
  *packet >> damage;
  getPlayer(id)->takeDamage(damage);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handlePlayerHeal() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handlePlayerHeal(sf::Packet* packet) {
  sf::Uint8 id;
  *packet >> id;
  sf::Uint8 new_hp;
  *packet >> new_hp;
  getPlayer(id)->setHP(new_hp);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleAddGround() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleAddGround(sf::Packet* packet) {
  sf::Uint32 id;
  *packet >> id;
  sf::Int64 x, y;
  *packet >> x;
  *packet >> y;
  sf::Uint32 width, height;
  *packet >> width;
  *packet >> height;
  sf::Uint8 type;
  *packet >> type;
  Grounds::Ground* new_ground = nullptr;
  if ((GroundType)type == GroundType::BadGround) {
    new_ground = new Grounds::BadGround(
        sf::FloatRect((float)x, (float)y, (float)width, (float)height));
  } else if ((GroundType)type == GroundType::SolidGround) {
    new_ground = new Grounds::SolidGround(
        sf::FloatRect((float)x, (float)y, (float)width, (float)height));
  } else if ((GroundType)type == GroundType::Portal) {
    sf::Int64 dest_x, dest_y;
    *packet >> dest_x;
    *packet >> dest_y;
    new_ground = new Grounds::Portal(
        sf::FloatRect((float)x, (float)y, (float)width, (float)height),
        sf::Vector2f((float)dest_x, (float)dest_y));
  } else if ((GroundType)type == GroundType::NetworkButton) {
    new_ground = new Grounds::NetworkButton(sf::Vector2f((float)x, (float)y));
  } else if ((GroundType)type == GroundType::StartButton) {
    new_ground = new Grounds::StartButton(sf::Vector2f((float)x, (float)y));
  } else if ((GroundType)type == GroundType::BuyableButton) {
    sf::Uint8 buyable_type;
    *packet >> buyable_type;
    new_ground = new Grounds::BuyableButton(sf::Vector2f((float)x, (float)y),
                                            (Buyables::Type)buyable_type);
  } else if ((GroundType)type == GroundType::EnemyBorder) {
    new_ground = new Grounds::EnemyBorder(
        sf::FloatRect((float)x, (float)y, (float)width, (float)height));
  }
  this->ground_ids[id] = new_ground;
  this->world->addGround(new_ground);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleGroundSetPos() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleGroundSetPos(sf::Packet* packet) {
  sf::Uint32 id;
  *packet >> id;
  sf::Int64 x, y;
  *packet >> x;
  *packet >> y;
  sf::Vector2f pos((float)x, (float)y);
  Grounds::Ground* g = getGround(id);
  g->setPosition(pos);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleRemoveGround() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleRemoveGround(sf::Packet* packet) {
  sf::Uint32 id;
  *packet >> id;
  Grounds::Ground* g = getGround(id);
  this->world->removeGround(g);
  this->ground_ids.erase(id);
  delete g;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleAddEnemy() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleAddEnemy(sf::Packet* packet) {
  sf::Uint32 id;
  *packet >> id;
  sf::Int64 x, y;
  *packet >> x;
  *packet >> y;
  sf::Uint8 state;
  *packet >> state;
  sf::Int8 dir;
  *packet >> dir;
  sf::Uint8 type;
  *packet >> type;
  Characters::Enemy* new_enemy = this->character_factory->createEnemy(
      (Characters::Type)type, sf::Vector2f((float)x, (float)y));
  new_enemy->setState((Characters::Character::State)state);
  new_enemy->setHorizontalMovement(dir);
  this->enemy_ids[id] = new_enemy;
  this->world->addEnemy(new_enemy);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleEnemySetPos() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleEnemySetPos(sf::Packet* packet) {
  sf::Uint32 id;
  *packet >> id;
  sf::Int64 x, y;
  *packet >> x;
  *packet >> y;
  getEnemy((size_t)id)->setPosition((float)x, (float)y);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleEnemyChangeState() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleEnemyChangeState(sf::Packet* packet) {
  sf::Uint32 id;
  *packet >> id;
  sf::Uint8 state;
  *packet >> state;
  getEnemy(id)->setState((Characters::Character::State)state);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleEnemyHorizontalDir() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleEnemyHorizontalDir(sf::Packet* packet) {
  sf::Uint32 id;
  *packet >> id;
  sf::Int8 dir;
  *packet >> dir;
  getEnemy(id)->setHorizontalMovement(dir);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleEnemyDie() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleEnemyDie(sf::Packet* packet) {
  sf::Uint32 id;
  *packet >> id;
  getEnemy(id)->die();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleRemoveEnemy() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleRemoveEnemy(sf::Packet* packet) {
  sf::Uint32 id;
  *packet >> id;
  Characters::Enemy* enemy = getEnemy(id);
  this->world->removeEnemy(enemy);
  this->enemy_ids.erase(id);
  delete enemy;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleAddCollectable() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleAddCollectable(sf::Packet* packet) {
  sf::Uint32 id;
  *packet >> id;
  sf::Int64 pos_x, pos_y;
  *packet >> pos_x;
  *packet >> pos_y;
  sf::Uint8 type;
  *packet >> type;
  Collectables::Collectable* new_collectable =
      this->collectable_factory->createCollectable(
          (Collectables::Type)type, sf::Vector2f((float)pos_x, (float)pos_y));
  this->collectable_ids[id] = new_collectable;
  this->world->addCollectable(new_collectable);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleCollectableCollected() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleCollectableCollected(sf::Packet* packet) {
  sf::Uint32 collectable_id;
  *packet >> collectable_id;
  sf::Uint8 player_id;
  *packet >> player_id;
  if (this->getPlayer(player_id)->collect(getCollectable(collectable_id)))
    this->world->removeCollectable(getCollectable(collectable_id));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleRemoveCollectable() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleRemoveCollectable(sf::Packet* packet) {
  sf::Uint32 id;
  *packet >> id;
  Collectables::Collectable* c = getCollectable(id);
  if (!c) return;  // Because sometimes a wrong / not working id is sent
  this->world->removeCollectable(c);
  this->collectable_ids.erase(id);
  delete c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleSetupRace() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleSetupRace(sf::Packet* packet) {
  sf::Uint16 distance;
  *packet >> distance;
  this->world->setRaceDistance(distance);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleDeclareWinner() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Client::handleDeclareWinner(sf::Packet* packet) {
  sf::Uint8 id;
  sf::Uint32 time;
  *packet >> id;
  *packet >> time;

  showPlayerWon(id, time);
}
}  // namespace Network