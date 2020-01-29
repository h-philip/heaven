/**
 * @file host.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for Host class.
 * @version 0.1
 * @date 2019-11-29
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "host.h"

namespace Network {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* update() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::update() {
  if (this->update_clock.getElapsedTime().asMilliseconds() >=
      SEND_UPDATE_INTERVAL) {
    this->update_clock.restart();
    // Send all positions
    for (auto& client : this->clients) {
      // Send player pos
      sf::Packet* position_packet =
          playerSetPosPacket(0, this->world->getPlayer(0)->getPosition());
      this->send_tcp_packets.push(std::pair<sf::TcpSocket*, sf::Packet*>(
          client.second, position_packet));

      // Send enemy pos if could have changed
      for (auto& enemy : this->enemy_ids) {
        if (enemy.second->canMove()) {
          sf::Packet* enemy_pos_packet = enemySetPosPacket(
              (sf::Uint32)enemy.first, enemy.second->getPosition());
          this->send_tcp_packets.push(std::pair<sf::TcpSocket*, sf::Packet*>(
              client.second, enemy_pos_packet));
        }
      }
    }
  }

  // Check new connections if not started yet
  if (!this->world->isStarted()) {
    sf::TcpSocket* new_socket = new sf::TcpSocket();
    while (sf::Socket::Status::Done == this->tcp_listener.accept(*new_socket)) {
      // New connection attempt
      if (this->allow_add_player) {
        // Not yet started -> new connection allowed
        new_socket->setBlocking(false);

        // Get id for new player
        uint8_t new_id = 0;
        while (this->player_ids.count(new_id) > 0) new_id++;

        // Log to console
        std::cout << "New player connected! Id: " << std::to_string(new_id)
                  << ", ip: " << new_socket->getRemoteAddress().toString()
                  << std::endl;

        // Create entries for new player in maps
        this->clients[new_id] = new_socket;
        this->client_ips[new_id] = new_socket->getRemoteAddress();
        this->player_ids[new_id] =
            new Characters::Player(Characters::Player::START_POSITION, false);

        // Add new player to world
        this->world->addPlayer(this->player_ids[new_id]);
        this->player_ids[new_id]->setName(std::to_string(new_id));

        // Send start packets to new player
        sendStart(new_id);
      } else {
        // Already started -> no new connections allowed
        new_socket->disconnect();
        delete new_socket;
        break;
      }
      new_socket = new sf::TcpSocket();
    }
  }

  // Send tcp
  while (!this->send_tcp_packets.empty()) {
    auto pair = this->send_tcp_packets.front();
    sendTcpPacket(pair.first, pair.second);
    delete pair.second;
    this->send_tcp_packets.pop();
  }
  // Receive tcp
  receiveTcp();

  // sendUdp();
  // receiveUdp();

  handleReceived();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* start() */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool Host::start() {
#ifdef DEBUG
  std::cout << "Starting as host... ";
#endif
  sf::Socket::Status status = this->tcp_listener.listen(this->tcp_port);
  switch (status) {
    case sf::Socket::Status::Disconnected:
      std::cerr << "Error while listening on tcp listener: Code " << status
                << " Disconnected." << std::endl;
      return false;
      break;

    case sf::Socket::Status::Error:
      std::cerr << "Error while listening on tcp listener: Code " << status
                << " Error." << std::endl;
      return false;
      break;

    default:
      break;
  }
#ifdef DEBUG
  std::cout << "Done" << std::endl;
#endif

  // status = this->udp_socket->bind(this->udp_port);
  // switch (status) {
  //   case sf::Socket::Status::Disconnected:
  //     std::cerr << "Error while binding udp socket: Code " << status
  //               << " Disconnected." << std::endl;
  //     break;

  //   case sf::Socket::Status::Error:
  //     std::cerr << "Error while binding udp socket: Code " << status
  //               << " Error." << std::endl;
  //     break;
  // }

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* sendStart() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::sendStart(uint8_t id) {
  // Send accept player
  {
    sf::Packet* accept_packet = new sf::Packet();
    *accept_packet << (sf::Uint8)PacketType::AcceptAddPlayer;
    *accept_packet << (sf::Uint8)id;
    this->send_tcp_packets.push(std::pair<sf::TcpSocket*, sf::Packet*>(
        this->clients[id], accept_packet));
  }

  // Send add all other players
  for (auto player : this->world->getPlayers()) {
    if (player == getPlayer(id)) continue;
    if (player != this->world->getPlayer(0)) {
      // Send the newly connected player to this player
      sf::Packet* add_packet = addPlayerPacket(id, getPlayer(id));
      this->send_tcp_packets.push(std::pair<sf::TcpSocket*, sf::Packet*>(
          this->clients[getPlayerId(player)], add_packet));
    }
    // Add this player to the newly connected one
    sf::Packet* packet = addPlayerPacket(getPlayerId(player), player);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(this->clients[id], packet));
  }

  // Send add all grounds
  for (auto ground : this->world->getGrounds()) {
    sf::Packet* packet =
        addGroundPacket((sf::Uint32)getGroundId(ground), ground);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(this->clients[id], packet));
  }

  // Send add all enemies
  for (auto enemy : this->world->getEnemies()) {
    sf::Packet* packet = addEnemyPacket((sf::Uint32)getEnemyId(enemy), enemy);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(this->clients[id], packet));
  }

  // Send all collectables
  for (auto collectable : this->world->getCollectables()) {
    sf::Packet* packet =
        addCollectablePacket((sf::Uint32)getCollectableId(collectable));
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(this->clients[id], packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doAddPlayer() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doAddPlayer(Characters::Player* p) {
  uint8_t id = 0;
  while (this->player_ids.count(id) > 0) id++;
  this->player_ids[id] = p;
  for (auto client : this->clients) {
    sf::Packet* packet = addPlayerPacket(id, p);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doPlayerChangeState() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doPlayerChangeState(const Characters::Player* p) {
  uint8_t id = getPlayerId(p);
  for (auto client : this->clients) {
    sf::Packet* packet = playerChangeStatePacket(id, p->getState());
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doPlayerHorizontalDir() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doPlayerHorizontalDir(const Characters::Player* p) {
  uint8_t id = getPlayerId(p);
  sf::Int8 dir = p->getDirection();
  for (auto client : this->clients) {
    sf::Packet* packet = playerHorizontalDirPacket(id, dir);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doPlayerTakeDamage() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doPlayerTakeDamage(const Characters::Player* p, sf::Int8 damage) {
  uint8_t id = getPlayerId(p);
  for (auto client : this->clients) {
    sf::Packet* packet = playerTakeDamagePacket(id, damage);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doPlayerHeal() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doPlayerHeal(const Characters::Player* p, sf::Uint8 new_hp) {
  sf::Uint8 id = getPlayerId(p);

  for (auto client : this->clients) {
    sf::Packet* packet = playerHealPacket(id, new_hp);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doAddGround() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doAddGround(Grounds::Ground* g) {
  size_t id = 0;
  while (this->ground_ids.count(id) > 0) id++;
  this->ground_ids[id] = g;
  for (auto client : this->clients) {
    sf::Packet* packet = addGroundPacket((sf::Uint32)id, g);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doGroundSetPos() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doGroundSetPos(const Grounds::Ground* g) {
  size_t id = getGroundId(g);
  sf::Vector2f pos(g->getGlobalBounds().left, g->getGlobalBounds().top);
  for (auto client : this->clients) {
    sf::Packet* packet = groundSetPosPacket((sf::Uint32)id, pos);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doRemoveGround() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doRemoveGround(const Grounds::Ground* g) {
  size_t id = getGroundId(g);
  this->ground_ids.erase(id);
  for (auto client : this->clients) {
    sf::Packet* packet = removeGroundPacket((sf::Uint32)id);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doAddEnemy() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doAddEnemy(Characters::Enemy* e) {
  size_t id = 0;
  while (this->enemy_ids.count(id) > 0) id++;
  this->enemy_ids[id] = e;
  for (auto client : this->clients) {
    sf::Packet* packet = addEnemyPacket((sf::Uint32)id, e);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doEnemyChangeState() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doEnemyChangeState(const Characters::Enemy* e) {
  size_t id = getEnemyId(e);
  for (auto client : this->clients) {
    sf::Packet* packet = enemyChangeStatePacket((sf::Uint32)id, e->getState());
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doEnemyHorizontalDir() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doEnemyHorizontalDir(const Characters::Enemy* e) {
  size_t id = getEnemyId(e);
  for (auto client : this->clients) {
    sf::Packet* packet =
        enemyHorizontalDirPacket((sf::Uint32)id, e->getDirection());
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doEnemyDie() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doEnemyDie(const Characters::Enemy* e) {
  size_t id = getEnemyId(e);
  for (auto client : this->clients) {
    sf::Packet* packet = enemyDiePacket((sf::Uint32)id);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doRemoveEnemy() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doRemoveEnemy(const Characters::Enemy* e) {
  size_t id = getEnemyId(e);
  this->enemy_ids.erase(id);
  for (auto client : this->clients) {
    sf::Packet* packet = removeEnemyPacket((sf::Uint32)id);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doAddCollectable() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doAddCollectable(Collectables::Collectable* c) {
  size_t id = 0;
  while (this->collectable_ids.count(id) > 0) id++;
  this->collectable_ids[id] = c;
  for (auto client : this->clients) {
    sf::Packet* packet = addCollectablePacket((sf::Uint32)id);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doCollectableCollected() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doCollectableCollected(const Collectables::Collectable* c,
                                  const Characters::Player* p) {
  // Tell clients
  size_t c_id = getCollectableId(c), p_id = getPlayerId(p);
  for (auto client : this->clients) {
    sf::Packet* packet =
        collectableCollectedPacket((sf::Uint32)c_id, (sf::Uint8)p_id);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }

  // Apply to self
  this->collectable_ids.erase(c_id);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doRemoveCollectable() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doRemoveCollectable(const Collectables::Collectable* c) {
  // Tell clients
  size_t id = getCollectableId(c);
  for (auto client : this->clients) {
    sf::Packet* packet = removeCollectablePacket((sf::Uint32)id);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }

  // Apply to self
  this->collectable_ids.erase(id);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doSetupRace() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doSetupRace(sf::Uint16 distance) {
  for (auto client : this->clients) {
    sf::Packet* packet = setupRacePacket(distance);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* doDeclareWinner() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::doDeclareWinner(Characters::Player* p, sf::Uint32 time) {
  sf::Uint8 id = getPlayerId(p);
  for (auto client : this->clients) {
    sf::Packet* packet = declareWinnerPacket(id, time);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, packet));
  }

  // Set for self
  showPlayerWon(id, time);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* sendUdp() */
///////////////////////////////////////////////////////////////////////////////////////////////////
// void Host::sendUdp() {
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

// ///////////////////////////////////////////////////////////////////////////////////////////////////
// /* receiveUdp() */
// ///////////////////////////////////////////////////////////////////////////////////////////////////
// void Host::receiveUdp() {
//   // Receive udp packets
//   sf::Packet* packet = new sf::Packet();
//   bool deleted = false;
//   for (auto client : this->client_ips) {
//     while (true) {
//       sf::Socket::Status status =
//           this->udp_socket->receive(*packet, client.second,
//           this->udp_port);
//       switch (status) {
//         case sf::Socket::Status::Done:
//           this->receive_udp_packets.push(packet);
//           packet = new sf::Packet();
//           break;

//         case sf::Socket::Status::Partial:
//           continue;
//           break;

//         case sf::Socket::Status::Disconnected: {
//           std::cerr << "Error while receiving udp packet: Code " << status
//                     << " Disconnected." << std::endl;
//           this->clients.erase(client.first);
//           this->client_ips.erase(client.first);
//           Characters::Player* p = getPlayer(client.first);
//           this->world->deletePlayer(p);
//           this->player_ids.erase(client.first);
//           delete p;
//           deleted = true;
//           break;
//         }

//         case sf::Socket::Status::Error:
//           std::cerr << "Error while receiving udp packet: Code " << status
//                     << " Error." << std::endl;
//           break;

//         case sf::Socket::Status::NotReady:
//           break;
//       }
//       break;
//     }
//     if (deleted) break;
//   }
//   delete packet;
// }

///////////////////////////////////////////////////////////////////////////////////////////////////
/* receiveTcp() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::receiveTcp() {
  // Receive udp packets
  sf::Packet* packet = new sf::Packet();
  bool deleted = false;
  for (auto client : this->clients) {
    while (true) {
      sf::Socket::Status status = client.second->receive(*packet);
      switch (status) {
        case sf::Socket::Status::Done:
          this->receive_tcp_packets.push(packet);
          packet = new sf::Packet();
          continue;
          break;

        case sf::Socket::Status::Partial:
          continue;
          break;

        case sf::Socket::Status::Disconnected: {
          std::cerr << "Error while receiving tcp packet: Code " << status
                    << " Disconnected." << std::endl;
          this->clients.erase(client.first);
          this->client_ips.erase(client.first);
          Characters::Player* p = getPlayer(client.first);
          this->world->deletePlayer(p);
          this->player_ids.erase(client.first);
          delete p;
          delete client.second;
          deleted = true;
          break;
        }

        case sf::Socket::Status::Error:
          std::cerr << "Error while receiving tcp packet: Code " << status
                    << " Error." << std::endl;
          break;

        case sf::Socket::Status::NotReady:
          break;
      }
      break;
    }
    if (deleted) break;
  }
  delete packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleWantAddPlayer() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleWantAddPlayer(sf::Packet* packet) {
  // IMPLEMENT: Might be implemented later to support hotseat (in online
  // multiplayer)
  std::cerr << "Error while handling received packet: "
            << "WantAddPlayer not yet supported!" << std::endl;
  /*
  sf::Int64 x, y;
  *packet >> x;
  *packet >> y;
  sf::Vector2f pos((float)x, (float)y);
  if (this->allow_add_player) {
    uint8_t id = 0;
    while (this->player_ids.count(id) > 0) id++;
    this->player_ids[id] = new Characters::Player(pos, false);
    this->world->addPlayer(this->player_ids[id]);
    this->player_ids[id]->setName(std::to_string(id));

    // Send accept player
    {
      sf::Packet* accept_packet = new sf::Packet();
      *accept_packet << (sf::Uint8)PacketType::AcceptAddPlayer;
      *accept_packet << (sf::Uint8)id;
      this->send_tcp_packets.push(std::pair<sf::TcpSocket*, sf::Packet*>(
          this->clients[id], accept_packet));
    }

    // Send add all other players
    for (auto player : this->world->getPlayers()) {
      if (player == getPlayer(id)) continue;
      if (player != this->world->getPlayer(0)) {
        // Send the newly connected player to this player
        sf::Packet* add_packet = addPlayerPacket(id, getPlayer(id));
        this->send_tcp_packets.push(std::pair<sf::TcpSocket*, sf::Packet*>(
            this->clients[getPlayerId(player)], add_packet));
      }
      // Add this player to the newly connected one
      sf::Packet* packet = addPlayerPacket(getPlayerId(player), player);
      this->send_tcp_packets.push(
          std::pair<sf::TcpSocket*, sf::Packet*>(this->clients[id], packet));
    }
  }
  */
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleAddPlayer() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleAddPlayer(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: AddPlayer must "
            << "not be sent to the host!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleAcceptAddPlayer() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleAcceptAddPlayer(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: AcceptAddPlayer must "
            << "not be sent to the host!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handlePlayerSetPos() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handlePlayerSetPos(sf::Packet* packet) {
  sf::Uint8 id;
  *packet >> id;
  sf::Int64 x, y;
  *packet >> x;
  *packet >> y;
  getPlayer(id)->setPosition((float)x, (float)y);
  // Send this to others to
  for (auto client : this->clients) {
    if (client.first == id) continue;
    sf::Packet* send_to_others =
        playerSetPosPacket(id, sf::Vector2f((float)x, (float)y));
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, send_to_others));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handlePlayerChangeState() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handlePlayerChangeState(sf::Packet* packet) {
  sf::Uint8 id;
  *packet >> id;
  sf::Uint8 state;
  *packet >> state;
  getPlayer(id)->setState((Characters::Character::State)state);
  // Send this to others to
  for (auto client : this->clients) {
    if (client.first == id) continue;
    sf::Packet* send_to_others =
        playerChangeStatePacket(id, (Characters::Character::State)state);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, send_to_others));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handlePlayerHorizontalDir() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handlePlayerHorizontalDir(sf::Packet* packet) {
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

  // Send this to others to
  for (auto client : this->clients) {
    if (client.first == id) continue;
    sf::Packet* send_to_others = playerHorizontalDirPacket(id, dir);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, send_to_others));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handlePlayerTakeDamage() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handlePlayerTakeDamage(sf::Packet* packet) {
  sf::Uint8 id;
  *packet >> id;
  sf::Int8 damage;
  *packet >> damage;
  getPlayer(id)->takeDamage(damage);

  // Send this to others to
  for (auto client : this->clients) {
    if (client.first == id) continue;
    sf::Packet* send_to_others = playerTakeDamagePacket(id, damage);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, send_to_others));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handlePlayerHeal() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handlePlayerHeal(sf::Packet* packet) {
  sf::Uint8 id;
  *packet >> id;
  sf::Uint8 new_hp;
  *packet >> new_hp;
  getPlayer(id)->setHP(new_hp);

  // Send this to others to
  for (auto client : this->clients) {
    if (client.first == id) continue;
    sf::Packet* send_to_others = playerHealPacket(id, new_hp);
    this->send_tcp_packets.push(
        std::pair<sf::TcpSocket*, sf::Packet*>(client.second, send_to_others));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleAddGround() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleAddGround(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: AddGround must "
            << "not be sent to the host!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleGroundSetPos() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleGroundSetPos(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: GroundSetPos must "
            << "not be sent to the host!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleRemoveGround() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleRemoveGround(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: RemoveGround must "
            << "not be sent to the host!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleAddEnemy() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleAddEnemy(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: AddEnemy must "
            << "not be sent to the host!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleEnemySetPos() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleEnemySetPos(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: EnemySetPos must "
            << "not be sent to the host!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleEnemyChangeState() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleEnemyChangeState(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: EnemyChangeState must "
            << "not be sent to the host!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleEnemyHorizontalDir() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleEnemyHorizontalDir(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: EnemyHorizontalDir must "
            << "not be sent to the host!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleEnemyDie() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleEnemyDie(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: EnemyDie must "
            << "not be sent to the host!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleRemoveEnemy() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleRemoveEnemy(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: RemoveEnemy must "
            << "not be sent to the host!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleAddCollectable() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleAddCollectable(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: AddCollectable must "
            << "not be sent to the host!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleCollectableCollected() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleCollectableCollected(sf::Packet* packet) {
  std::cerr
      << "Error while handling received packet: CollectableCollected must "
      << "not be sent to the host!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleRemoveCollectable() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleRemoveCollectable(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: RemoveCollectable must "
            << "not be sent to the host!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleSetupRace() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleSetupRace(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: SetupRace must "
            << "not be sent to the host!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleDeclareWinner() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Host::handleDeclareWinner(sf::Packet* packet) {
  std::cerr << "Error while handling received packet: DeclareWinner must "
            << "not be sent to the host!" << std::endl;
}
}  // namespace Network