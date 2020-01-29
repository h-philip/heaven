/**
 * @file network_manager.cpp
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Source file for NetworkManager class.
 * @version 0.1
 * @date 2019-11-24
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "network_manager.h"

namespace Network {

///////////////////////////////////////////////////////////////////////////////////////////////////
/* Static Variables */
///////////////////////////////////////////////////////////////////////////////////////////////////

NetworkManager* NetworkManager::instance = nullptr;

///////////////////////////////////////////////////////////////////////////////////////////////////
/* showPlayerWon() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void NetworkManager::showPlayerWon(sf::Uint8 id, sf::Uint32 time) {
  // Calculate minutes, seconds and ms from time
  uint16_t minutes = (uint16_t)(sf::milliseconds(time).asSeconds() / 60);
  uint8_t seconds =
      (uint8_t)(sf::milliseconds(time) - sf::seconds((float)minutes * 60.f))
          .asSeconds();
  uint16_t milliseconds =
      (sf::milliseconds(time) - sf::seconds((float)minutes * 60.f) -
       sf::seconds((float)seconds))
          .asMilliseconds();
  // Display time in stdout
  std::cout << "#####" << std::endl
            << "Player " << std::to_string(id)
            << " won! Time: " << std::to_string(minutes) << ":"
            << (seconds < 10 ? "0" : "") << std::to_string(seconds) << ":"
            << std::to_string(milliseconds) << std::endl
            << "#####" << std::endl;

  // Reset race
  this->world->setRaceDistance(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* sendTcpPacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool NetworkManager::sendTcpPacket(sf::TcpSocket* socket, sf::Packet* packet) {
  // Send tcp packets
  while (true) {
    sf::Socket::Status status = socket->send(*packet);
    switch (status) {
      case sf::Socket::Status::Done:
        break;

      case sf::Socket::Status::Partial:
        continue;
        break;

      case sf::Socket::Status::Disconnected:
        std::cerr << "Error while sending tcp packet: Code " << status
                  << " Disconnected." << std::endl;
        return false;
        break;

      case sf::Socket::Status::Error:
        std::cerr << "Error while sending tcp packet: Code " << status
                  << " Error." << std::endl;
        return false;
        break;

      case sf::Socket::Status::NotReady:
        break;
    }
    break;
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* wantaddPlayerPacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::wantAddPlayerPacket(const sf::Vector2f& position) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::wantAddPlayerPacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::WantAddPlayer;
  *packet << (sf::Int64)position.x;
  *packet << (sf::Int64)position.y;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* playerSetPosPacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::playerSetPosPacket(sf::Uint8 id,
                                               const sf::Vector2f& position) {
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::PlayerSetPos;
  *packet << id;
  *packet << (sf::Int64)position.x;
  *packet << (sf::Int64)position.y;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* playerChangeStatePacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::playerChangeStatePacket(
    sf::Uint8 id, Characters::Character::State new_state) {
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::PlayerChangeState;
  *packet << id;
  *packet << (sf::Uint8)new_state;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* playerHorizontalDirPacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::playerHorizontalDirPacket(sf::Uint8 id,
                                                      sf::Int8 dir) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::playerHorizontalDirPacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::PlayerHorizontalDir;
  *packet << id;
  *packet << dir;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* addGroundPacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::addGroundPacket(sf::Uint32 id,
                                            Grounds::Ground* ground) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::addGroundPacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::AddGround;
  *packet << (sf::Uint32)getGroundId(ground);
  *packet << (sf::Int64)ground->getGlobalBounds().left;
  *packet << (sf::Int64)ground->getGlobalBounds().top;
  *packet << (sf::Uint32)ground->getGlobalBounds().width;
  *packet << (sf::Uint32)ground->getGlobalBounds().height;
  *packet << (sf::Uint8)getGroundType(ground->name());
  if (ground->name() == "portal") {
    *packet << (sf::Int64)((Grounds::Portal*)ground)->getDestination().x;
    *packet << (sf::Int64)((Grounds::Portal*)ground)->getDestination().y;
  } else if (ground->name() == "buyable_button") {
    *packet << (sf::Uint8)((Grounds::BuyableButton*)ground)->getBuyableType();
  }
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* addPlayerPacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::addPlayerPacket(sf::Uint8 id,
                                            Characters::Player* player) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::addPlayerPacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::AddPlayer;
  *packet << (sf::Uint8)id;
  *packet << (sf::Int64)getPlayer(id)->getPosition().x;
  *packet << (sf::Int64)getPlayer(id)->getPosition().y;
  *packet << (sf::Uint8)getPlayer(id)->getState();
  *packet << (sf::Int8)getPlayer(id)->getDirection();
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* addEnemyPacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::addEnemyPacket(sf::Uint32 id,
                                           Characters::Enemy* enemy) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::addEnemyPacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::AddEnemy;
  *packet << id;
  *packet << (sf::Int64)enemy->getPosition().x;
  *packet << (sf::Int64)enemy->getPosition().y;
  *packet << (sf::Uint8)enemy->getState();
  *packet << (sf::Uint8)enemy->getDirection();
  *packet << (sf::Uint8)enemy->getType();
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* enemySetPosPacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::enemySetPosPacket(sf::Uint32 id,
                                              const sf::Vector2f& position) {
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::EnemySetPos;
  *packet << id;
  *packet << (sf::Int64)position.x;
  *packet << (sf::Int64)position.y;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* enemyChangeStatePacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::enemyChangeStatePacket(
    sf::Uint32 id, Characters::Character::State state) {
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::EnemyChangeState;
  *packet << id;
  *packet << (sf::Uint8)state;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* enemyHorizontalDirPacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::enemyHorizontalDirPacket(sf::Uint32 id,
                                                     sf::Int8 dir) {
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::EnemyHorizontalDir;
  *packet << id;
  *packet << dir;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* enemyDiePacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::enemyDiePacket(sf::Uint32 id) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::enemyDiePacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::EnemyDie;
  *packet << id;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* playerTakeDamagePacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::playerTakeDamagePacket(sf::Uint8 id,
                                                   sf::Int8 damage) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::playerTakeDamagePacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::PlayerTakeDamage;
  *packet << id;
  *packet << damage;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* playerHealPacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::playerHealPacket(sf::Uint8 id, sf::Int8 new_hp) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::playerHealPacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::PlayerHeal;
  *packet << id;
  *packet << new_hp;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* addCollectablePacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::addCollectablePacket(sf::Uint32 id) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::addCollectablePacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::AddCollectable;
  *packet << id;
  *packet << (sf::Int64)getCollectable(id)->getPosition().x;
  *packet << (sf::Int64)getCollectable(id)->getPosition().y;
  *packet << (sf::Uint8)getCollectable(id)->getType();
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* collectableCollectedPacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::collectableCollectedPacket(sf::Uint32 id,
                                                       sf::Uint8 player) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::collectableCollectedPacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::CollectableCollected;
  *packet << id;
  *packet << player;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* removeCollectablePacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::removeCollectablePacket(sf::Uint32 id) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::removeCollectablePacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::RemoveCollectable;
  *packet << id;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* removeEnemyPacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::removeEnemyPacket(sf::Uint32 id) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::removeEnemyPacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::RemoveEnemy;
  *packet << id;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* removeGroundPacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::removeGroundPacket(sf::Uint32 id) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::removeGroundPacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::RemoveGround;
  *packet << id;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* groundSetPosPacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::groundSetPosPacket(sf::Uint32 id,
                                               const sf::Vector2f& position) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::groundSetPosPacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::GroundSetPos;
  *packet << id;
  *packet << (sf::Int64)position.x;
  *packet << (sf::Int64)position.y;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* setupRacePacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::setupRacePacket(sf::Uint16 distance) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::setupRacePacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::SetupRace;
  *packet << distance;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* declareWinnerPacket() */
///////////////////////////////////////////////////////////////////////////////////////////////////
sf::Packet* NetworkManager::declareWinnerPacket(sf::Uint8 id, sf::Uint32 time) {
#ifdef NETWORK_VERBOSE
  std::cout << "NetworkManager::declareWinnerPacket" << std::endl;
#endif
  sf::Packet* packet = new sf::Packet();
  *packet << (sf::Uint8)PacketType::DeclareWinner;
  *packet << id;
  *packet << time;
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* handleReceived() */
///////////////////////////////////////////////////////////////////////////////////////////////////
void NetworkManager::handleReceived() {
  sf::Packet* packet;
  while (!this->receive_tcp_packets.empty() ||
         !this->receive_udp_packets.empty()) {
    if (!this->receive_tcp_packets.empty())
      packet = this->receive_tcp_packets.front();
    else
      packet = this->receive_udp_packets.front();
    PacketType type;
    sf::Uint8 t;
    *packet >> t;
    type = (PacketType)t;

    switch (type) {
      case PacketType::WantAddPlayer:
        handleWantAddPlayer(packet);
        break;

      case PacketType::AddPlayer:
        handleAddPlayer(packet);
        break;

      case PacketType::AcceptAddPlayer:
        handleAcceptAddPlayer(packet);
        break;

      case PacketType::PlayerSetPos:
        handlePlayerSetPos(packet);
        break;

      case PacketType::PlayerChangeState:
        handlePlayerChangeState(packet);
        break;

      case PacketType::PlayerHorizontalDir:
        handlePlayerHorizontalDir(packet);
        break;

      case PacketType::PlayerTakeDamage:
        handlePlayerTakeDamage(packet);
        break;

      case PacketType::PlayerHeal:
        handlePlayerHeal(packet);
        break;

      case PacketType::AddGround:
        handleAddGround(packet);
        break;

      case PacketType::GroundSetPos:
        handleGroundSetPos(packet);
        break;

      case PacketType::RemoveGround:
        handleRemoveGround(packet);
        break;

      case PacketType::AddEnemy:
        handleAddEnemy(packet);
        break;

      case PacketType::EnemySetPos:
        handleEnemySetPos(packet);
        break;

      case PacketType::EnemyChangeState:
        handleEnemyChangeState(packet);
        break;

      case PacketType::EnemyHorizontalDir:
        handleEnemyHorizontalDir(packet);
        break;

      case PacketType::EnemyDie:
        handleEnemyDie(packet);
        break;

      case PacketType::RemoveEnemy:
        handleRemoveEnemy(packet);
        break;

      case PacketType::AddCollectable:
        handleAddCollectable(packet);
        break;

      case PacketType::CollectableCollected:
        handleCollectableCollected(packet);
        break;

      case PacketType::RemoveCollectable:
        handleRemoveCollectable(packet);
        break;

      default:
        std::cerr << "Error while handling received packet: "
                     "Unknown PacketType: "
                  << std::to_string(t) << std::endl;
        break;
    }
#ifdef DEBUG
    if (!packet->endOfPacket())
      std::cerr << "Error while handling received packet: Packet wasn't read "
                   "completely. Type: "
                << std::to_string(t) << std::endl;
#endif
    delete packet;

    if (!this->receive_tcp_packets.empty())
      this->receive_tcp_packets.pop();
    else
      this->receive_udp_packets.pop();
  }
}
}  // namespace Network