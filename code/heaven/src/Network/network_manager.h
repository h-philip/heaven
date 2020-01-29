/**
 * @file network_manager.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for NetworkManager class.
 * @version 0.1
 * @date 2019-11-24
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <SFML/Network.hpp>
#include <queue>

#include "../world.h"

/**
 * @brief Namespace for all Network classes.
 *
 */
namespace Network {

/** \class NetworkManager
 *  @brief Base class for Host and Client.
 */
class NetworkManager {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Constant Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  /**
   * @brief Ground types for easier sending and receiving of data.
   *
   */
  enum class GroundType : uint8_t {
    SolidGround,
    BadGround,
    Portal,
    NetworkButton,
    StartButton,
    BuyableButton,
    EnemyBorder
  };

  /**
   * @brief Packet types for easier sending and receiving of data.
   *
   * Each packet has to start with a PacketType.
   * The following bytes depend on the PacketType and are described here.
   */
  enum class PacketType : uint8_t {
    WantAddPlayer,       /**< player pos x(Int64) and y(Int64)
                         are following
                         (can only be sent by clients) */
    AddPlayer,           /**< player id(Uint8), player pos x(Int64) and
                         y(Int64), state(Uint8) and direction(Int8)
                         are following (can only be sent by host) */
    AcceptAddPlayer,     /**< player id(Uint8) is following
                         (can only be sent by host) */
    PlayerSetPos,        /**< player id(Uint8), pos x(Int64) and
                         y(Int64) for sender player are following */
    PlayerChangeState,   /**< player id(Uint8) and state(Uint8)
                         are following */
    PlayerHorizontalDir, /**< player id(Uint8) and direction(Int8)
                         are following */
    PlayerTakeDamage,    /**< player id(Uint8) and amount of damage(Int8)
                         are following */
    PlayerHeal,          /**< player id(Uint8) and new hp(Int8) are following */
    AddGround,           /**< ground id(Uint32), pos x(Int64) and
                         y(Int64), width(Uint32) and height(Uint32)
                         and type(GroundType) are following;
                         if portal: dest x(Int64) and y(Int64) are following
                         if buyable_button: type(Uint8) is following
                         (can only be sent by host) */
    GroundSetPos,        /**< ground id(Uint32), pos x(Int64) and
                         y(Int64) are following;
                         if portal: dest x(Int64) and y(Int64) are following
                         (can only be sent by host) */
    RemoveGround,        /**< ground id(Uint32) is following
                         (can only be sent by host) */
    AddEnemy,            /**< enemy id(Uint32), pos x(Int64) and y(Int64),
                         state(Uint8), direction(Int8)
                         and type(EnemyType) are following
                         (can only be sent by host) */
    EnemySetPos,         /**< enemy id(Uint32), pos x(Int64) and
                         y(Int64) are following
                         (can only be sent by host) */
    EnemyChangeState,    /**< enemy id(Uint32) and state(Uint8)
                         are following
                         (can only be sent by host) */
    EnemyHorizontalDir,  /**< enemy id(Uint32) and direction(Int8)
                         are following
                         (can only be sent by host) */
    EnemyDie,            /**< enemy id(Uint32) is following
                         (can only be sent by host) */
    RemoveEnemy,         /**< enemy id(Uint32) following
                         (can only be sent by host) */
    AddCollectable,      /**< collectable id(Uint32), pos x(Int64)
                         and y(Int64) and type(CollectableType) are following
                         (can only be sent by host) */
    CollectableCollected, /**< collectable id(Uint32) and player id(Uint8)
                         are following
                          (can only be sent by host) */
    RemoveCollectable,    /**< collectable id(Uint32) is following
                         (can only be sent by host) */
    SetupRace,            /**< Race distance(Int16) is following */
    DeclareWinner         /**< player id(Uint8) and time in ms(Uint32)
                          is following */
  };

  static const uint16_t UDP_PORT = 2309, /**< The standard udp port used. */
      TCP_PORT = 2307;                   /**< The standard tcp port used.< */

  static const sf::Int32 SEND_UPDATE_INTERVAL =
      10; /**< The minumum interval in ms to wait before
           sendng new positions. */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 protected:
  World* world = nullptr; /**< Pointer to the world. */
  uint16_t udp_port = 0,  /**< Port for udp_socket */
      tcp_port = 0;       /**< Port for tcp_socket */

  std::map<uint8_t, Characters::Player*>
      player_ids; /**< Key: id (by host) of player;
                  val: pointer to player in local world */
  std::map<size_t, Grounds::Ground*>
      ground_ids; /**< Key: id (by host) of ground;
                  val: pointer to ground in local world */
  std::map<size_t, Characters::Enemy*>
      enemy_ids; /**< Key: id (by host) of enemy;
                 val: pointer to enemy in local world */
  std::map<size_t, Collectables::Collectable*> collectable_ids;

  std::queue<std::pair<sf::IpAddress*, sf::Packet*>>
      send_udp_packets; /**< Queue for sending udp packets to given ip address
                        (if sending/receiving is moved to other thread) */
  std::queue<std::pair<sf::TcpSocket*, sf::Packet*>>
      send_tcp_packets; /**< Queue for sending tcp packets over given socket
                        (if sending/receiving is moved to other thread) */

  std::queue<sf::Packet*>
      receive_udp_packets, /**< Queue for receiving udp packets to host
                           (if sending/receiving is moved to other thread) */
      receive_tcp_packets; /**< Queue for receiving tcp packets to host
                           (if sending/receiving is moved to other thread) */

  sf::Clock
      update_clock; /**< Clock to measure time until next send of positions */

  static NetworkManager* instance; /**< Current instance of NetworkManager */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Network Manager object.
   *
   * @param world Pointer to the world.
   * @param udp_port Port for udp connections.
   * @param tcp_port Port for tcp connections.
   */
  NetworkManager(World* world, uint16_t udp_port = UDP_PORT,
                 uint16_t tcp_port = TCP_PORT)
      : world(world) {
    if (instance) {
      std::cerr << "Error while creating NetworkManager: Only one can be "
                   "existent at any time."
                << std::endl;
      exit(1);
    }
    this->udp_port = udp_port;
    this->tcp_port = tcp_port;
    instance = this;
  }

  /**
   * @brief Destroys the Network Manager object.
   *
   */
  virtual ~NetworkManager() {
    while (!this->send_tcp_packets.empty()) {
      auto pair = this->send_tcp_packets.front();
      delete pair.second;
      this->send_tcp_packets.pop();
    }
    while (!this->send_udp_packets.empty()) {
      auto pair = this->send_udp_packets.front();
      delete pair.second;
      this->send_udp_packets.pop();
    }
    while (!this->receive_tcp_packets.empty()) {
      auto packet = this->receive_tcp_packets.front();
      delete packet;
      this->send_tcp_packets.pop();
    }
    while (!this->receive_udp_packets.empty()) {
      auto packet = this->receive_udp_packets.front();
      delete packet;
      this->send_udp_packets.pop();
    }

    NetworkManager::instance = nullptr;
  }

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  virtual bool isHost() const = 0;

  /**
   * @brief Updates everything.
   *
   */
  virtual void update() = 0;

  /**
   * @brief Returns the current NetworkManager instance.
   *
   * This does not create a new instance, if there is no one.
   *
   * @return NetworkManager* Instance
   */
  static NetworkManager* getInstance() { return NetworkManager::instance; }

 protected:
  /**
   * @brief Returns the id of the player.
   *
   * @param player Pointer to the player.
   * @return uint8_t The players id.
   */
  virtual uint8_t getPlayerId(const Characters::Player* player) const {
    for (auto pair : this->player_ids)
      if (pair.second == player) return pair.first;
    std::cerr << "Error: Player id not found." << std::endl;
    exit(1);
  }

  /**
   * @brief Gets the player from id.
   *
   * @param id Id of the player.
   * @return Characters::Player* Pointer to the player.
   */
  virtual Characters::Player* getPlayer(uint8_t id) const {
    return this->player_ids.at(id);
  }

  /**
   * @brief Returns the id of the enemy.
   *
   * @param enemy Pointer to the enemy.
   * @return size_t The enemys id.
   */
  virtual size_t getEnemyId(const Characters::Enemy* enemy) const {
    for (auto pair : this->enemy_ids)
      if (pair.second == enemy) return pair.first;
    std::cerr << "Error: Enemy id not found." << std::endl;
    exit(1);
  }

  /**
   * @brief Gets the enemy from id.
   *
   * @param id Id of the enemy.
   * @return Characters::Enemy* Pointer to the enemy.
   */
  virtual Characters::Enemy* getEnemy(size_t id) const {
    return this->enemy_ids.at(id);
  }

  /**
   * @brief Returns the id of the ground.
   *
   * @param ground Pointer to the ground.
   * @return size_t The ground id.
   */
  virtual size_t getGroundId(const Grounds::Ground* ground) const {
    for (auto pair : this->ground_ids)
      if (pair.second == ground) return pair.first;
    std::cerr << "Error: Ground id not found." << std::endl;
    exit(1);
  }

  /**
   * @brief Gets the ground from id.
   *
   * @param id Id of the ground.
   * @return Grounds::Ground* Pointer to the ground.
   */
  virtual Grounds::Ground* getGround(size_t id) const {
    return this->ground_ids.at(id);
  }

  /**
   * @brief Returns the id of the collectable.
   *
   * @param ground Pointer to the collectable.
   * @return size_t The collectable id.
   */
  virtual size_t getCollectableId(
      const Collectables::Collectable* collectable) const {
    for (auto pair : this->collectable_ids)
      if (pair.second == collectable) return pair.first;
    std::cerr << "Error: Collectable id not found." << std::endl;
    exit(1);
  }

  /**
   * @brief Gets the collectable from id.
   *
   * @param id Id of the collectable.
   * @return Collectables::Collectable* Pointer to the collectable.
   */
  virtual Collectables::Collectable* getCollectable(size_t id) const {
    return this->collectable_ids.at(id);
  }

  /**
   * @brief Gets the GroundType of a ground.
   *
   * @param name The string returned from Grounds::name.
   * @return GroundType The type of the ground.
   */
  GroundType getGroundType(std::string name) {
    if (name == "bad_ground")
      return GroundType::BadGround;
    else if (name == "solid_ground")
      return GroundType::SolidGround;
    else if (name == "portal")
      return GroundType::Portal;
    else if (name == "network_button")
      return GroundType::NetworkButton;
    else if (name == "start_button")
      return GroundType::StartButton;
    else if (name == "buyable_button")
      return GroundType::BuyableButton;
    else if (name == "enemy_border")
      return GroundType::EnemyBorder;
    else {
      std::cerr
          << "Error: NetworkManager::getGroundType -> Unknown ground type: "
          << name << std::endl;
      exit(1);
    }
  }

  /**
   * @brief Shows a note about the winner.
   *
   * @param id The id of the winner.
   * @param time The time of the winner.
   */
  void showPlayerWon(sf::Uint8 id, sf::Uint32 time);

  /**
   * @brief Sends a packet over tcp.
   *
   * If the packet can't be sent (satus error or disconnected),
   * it won't be sent.
   *
   * @param socket Pointer to the socket to send on.
   * @param packet Pointer to the packet to send.
   * @return true Packet was sent successfully.
   * @return false Packet could not be sent.
   */
  bool sendTcpPacket(sf::TcpSocket* socket, sf::Packet* packet);

  // ----------------------------------------- //
  /* Packet Creation */
  // ----------------------------------------- //

  /**
   * @brief Creates a new packet which contains info about a player that wants
   * to be added.
   *
   * @param position Position of the player.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* wantAddPlayerPacket(const sf::Vector2f& position);

  /**
   * @brief Creates a new packet which contains the position.
   *
   * @param id Id of the player.
   * @param position Position of the player.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* playerSetPosPacket(sf::Uint8 id, const sf::Vector2f& position);

  /**
   * @brief Creates a new packet which contains the state.
   *
   * @param id Id of the player.
   * @param new_state State of the player.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* playerChangeStatePacket(sf::Uint8 id,
                                      Characters::Character::State new_state);

  /**
   * @brief Creates a new packet which contains the direction.
   *
   * @param id Id of the player.
   * @param dir Direction of the player.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* playerHorizontalDirPacket(sf::Uint8 id, sf::Int8 dir);

  /**
   * @brief Creates a new packet which contains info about the ground.
   *
   * @param id Id of the ground.
   * @param ground Pointer to the ground.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* addGroundPacket(sf::Uint32 id, Grounds::Ground* ground);

  /**
   * @brief Creates a new packet which contains info about the player.
   *
   * @param id Id of the player.
   * @param player Pointer to the player.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* addPlayerPacket(sf::Uint8 id, Characters::Player* player);

  /**
   * @brief Creates a new packet which contains info about the enemy.
   *
   * @param id Id of the enemy.
   * @param enemy Pointer to the enemy.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* addEnemyPacket(sf::Uint32 id, Characters::Enemy* enemy);

  /**
   * @brief Creates a new packet which contains the position of an enemy.
   *
   * @param id Id of the enemy.
   * @param position Positions of the enemy.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* enemySetPosPacket(sf::Uint32 id, const sf::Vector2f& position);

  /**
   * @brief Creates a new packet which contains the state of an enemy.
   *
   * @param id Id of the enemy.
   * @param state State of the enemy.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* enemyChangeStatePacket(sf::Uint32 id,
                                     Characters::Character::State state);

  /**
   * @brief Creates a new packet which contains the direction of an enemy.
   *
   * @param id Id of the enemy.
   * @param dir Direction of the enemy.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* enemyHorizontalDirPacket(sf::Uint32 id, sf::Int8 dir);

  /**
   * @brief Creates a new packet which contains info about a dead enemy.
   *
   * @param id Id of the enemy.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* enemyDiePacket(sf::Uint32 id);

  /**
   * @brief Creates a new packet which contains info about damage to a player.
   *
   * @param id Id of the player.
   * @param damage Damge to be taken.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* playerTakeDamagePacket(sf::Uint8 id, sf::Int8 damage);

  /**
   * @brief Creates a new packet which contains info about healing a player.
   *
   * @param id Id of the player.
   * @param new_hp The new hp for the player.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* playerHealPacket(sf::Uint8 id, sf::Int8 new_hp);

  /**
   * @brief Creates a new packet which contains info about a new collectable.
   *
   * @param id Id of the new collectable.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* addCollectablePacket(sf::Uint32 id);

  /**
   * @brief Creates a new packet which contains info about a collectable being
   * collected.
   *
   * @param id Id of the collectable.
   * @param player Id of the player who collected it.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* collectableCollectedPacket(sf::Uint32 id, sf::Uint8 player);

  /**
   * @brief Creates a new packet which contains info about a collectable being
   * removed.
   *
   * @param id Id of the collectable.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* removeCollectablePacket(sf::Uint32 id);

  /**
   * @brief Creates a new packet which contains info about a enemy being
   * removed.
   *
   * @param id Id of the enemy.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* removeEnemyPacket(sf::Uint32 id);

  /**
   * @brief Creates a new packet which contains info about a ground being
   * ground.
   *
   * @param id Id of the ground.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* removeGroundPacket(sf::Uint32 id);

  /**
   * @brief Creates a new packet which contains the position of an ground.
   *
   * @param id Id of the ground.
   * @param position Positions of the ground.
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* groundSetPosPacket(sf::Uint32 id, const sf::Vector2f& position);

  /**
   * @brief Creates a new packet which contains the distance for a race.
   *
   * @param distance The distance of the race (see upper right corner of
   * screen).
   * @return sf::Packet* Pointer to the new packet.
   */
  sf::Packet* setupRacePacket(sf::Uint16 distance);

  /**
   * @brief Creates a new packet which contains the winner of a race.
   *
   * @param id The id of the winner.
   * @param time The time in ms the winner needed.
   * @return Pointer to the new packet.
   */
  sf::Packet* declareWinnerPacket(sf::Uint8 id, sf::Uint32 time);

  // ----------------------------------------- //
  /* Received Packet Handling */
  // ----------------------------------------- //

  /**
   * @brief Looks through all packets received and performs needed actions.
   *
   */
  void handleReceived();

  virtual void handleWantAddPlayer(sf::Packet* packet) = 0;

  virtual void handleAddPlayer(sf::Packet* packet) = 0;

  virtual void handleAcceptAddPlayer(sf::Packet* packet) = 0;

  virtual void handlePlayerSetPos(sf::Packet* packet) = 0;

  virtual void handlePlayerChangeState(sf::Packet* packet) = 0;

  virtual void handlePlayerHorizontalDir(sf::Packet* packet) = 0;

  virtual void handlePlayerTakeDamage(sf::Packet* packet) = 0;

  virtual void handlePlayerHeal(sf::Packet* packet) = 0;

  virtual void handleAddGround(sf::Packet* packet) = 0;

  virtual void handleGroundSetPos(sf::Packet* packet) = 0;

  virtual void handleRemoveGround(sf::Packet* packet) = 0;

  virtual void handleAddEnemy(sf::Packet* packet) = 0;

  virtual void handleEnemySetPos(sf::Packet* packet) = 0;

  virtual void handleEnemyChangeState(sf::Packet* packet) = 0;

  virtual void handleEnemyHorizontalDir(sf::Packet* packet) = 0;

  virtual void handleEnemyDie(sf::Packet* packet) = 0;

  virtual void handleRemoveEnemy(sf::Packet* packet) = 0;

  virtual void handleAddCollectable(sf::Packet* packet) = 0;

  virtual void handleCollectableCollected(sf::Packet* packet) = 0;

  virtual void handleRemoveCollectable(sf::Packet* packet) = 0;

  virtual void handleSetupRace(sf::Packet* packet) = 0;

  virtual void handleDeclareWinner(sf::Packet* packet) = 0;
};
}  // namespace Network