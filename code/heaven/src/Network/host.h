/**
 * @file host.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Host class.
 * @version 0.1
 * @date 2019-11-29
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "network_manager.h"

namespace Network {
/** \class Host
 *  @brief NetworkManager as host.
 */
class Host : public NetworkManager {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 private:
  sf::TcpListener tcp_listener;
  std::map<uint8_t, sf::TcpSocket*> clients;   /**< Player id with socket */
  std::map<uint8_t, sf::IpAddress> client_ips; /**< Player id with ip addr */

  bool allow_add_player = true; /**< Can be set to false to deny new players */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Host object.
   *
   * @param world Pointer to the world.
   * @param udp_port Port for udp connections.
   * @param tcp_port Port for tcp connections.
   */
  Host(World* world, uint16_t udp_port = UDP_PORT, uint16_t tcp_port = TCP_PORT)
      : NetworkManager(world, udp_port, tcp_port) {
    this->tcp_listener.setBlocking(false);
    this->player_ids[0] = this->world->getPlayer(0);

    // Add all grounds
    std::vector<Grounds::Ground*> grounds = this->world->getGrounds();
    uint32_t i = 0;
    for (auto ground : grounds) {
      while (this->ground_ids.count(i) > 0) i++;
      this->ground_ids[i] = ground;
    }

    // Add all enemies
    std::vector<Characters::Enemy*> enemies = this->world->getEnemies();
    i = 0;
    for (auto enemy : enemies) {
      while (this->enemy_ids.count(i) > 0) i++;
      this->enemy_ids[i] = enemy;
    }

    // Add all collectables
    std::vector<Collectables::Collectable*> collectables =
        this->world->getCollectables();
    i = 0;
    for (auto collectable : collectables) {
      while (this->collectable_ids.count(i) > 0) i++;
      this->collectable_ids[i] = collectable;
    }
  }

  /**
   * @brief Destroys the Host object.
   *
   */
  virtual ~Host() {
    for (auto client : this->clients) {
      client.second->disconnect();
      delete client.second;
    }
  }

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  bool isHost() const override { return true; }

  void update() override;

  /**
   * @brief Setups everything to be ready to accept connections from clients.
   *
   * @return true On success.
   * @return false On failure.
   */
  bool start();

 private:
  /**
   * @brief Sends all packets needed for a client when first connected to this
   * host.
   *
   * @param id The id of the new client.
   */
  void sendStart(uint8_t id);

  // ----------------------------------------- //
  /* From World Methods */
  // ----------------------------------------- //
 public:
  void doAddPlayer(Characters::Player* p);

  void doPlayerChangeState(const Characters::Player* p);

  void doPlayerHorizontalDir(const Characters::Player* p);

  void doPlayerTakeDamage(const Characters::Player* p, sf::Int8 damage);
 
  void doPlayerHeal(const Characters::Player* p, sf::Uint8 new_hp);

  void doAddGround(Grounds::Ground* g);

  void doGroundSetPos(const Grounds::Ground* g);

  void doRemoveGround(const Grounds::Ground* g);

  void doAddEnemy(Characters::Enemy* e);

  void doEnemyChangeState(const Characters::Enemy* e);

  void doEnemyHorizontalDir(const Characters::Enemy* e);

  void doEnemyDie(const Characters::Enemy* e);

  void doRemoveEnemy(const Characters::Enemy* e);

  void doAddCollectable(Collectables::Collectable* c);

  void doCollectableCollected(const Collectables::Collectable* c,
                              const Characters::Player* p);

  void doRemoveCollectable(const Collectables::Collectable* c);

  void doSetupRace(sf::Uint16 distance);

  void doDeclareWinner(Characters::Player* p, sf::Uint32 time);

  // ----------------------------------------- //
  /* Send / Receive */
  // ----------------------------------------- //
 private:
  // void sendUdp();

  // void receiveUdp();

  void receiveTcp();

  // ----------------------------------------- //
  /* Received Packet Handling */
  // ----------------------------------------- //

  virtual void handleWantAddPlayer(sf::Packet* packet) override;

  virtual void handleAddPlayer(sf::Packet* packet) override;

  virtual void handleAcceptAddPlayer(sf::Packet* packet) override;

  virtual void handlePlayerSetPos(sf::Packet* packet) override;

  virtual void handlePlayerChangeState(sf::Packet* packet) override;

  virtual void handlePlayerHorizontalDir(sf::Packet* packet) override;

  virtual void handlePlayerTakeDamage(sf::Packet* packet) override;

  virtual void handlePlayerHeal(sf::Packet* packet) override;

  virtual void handleAddGround(sf::Packet* packet) override;

  virtual void handleGroundSetPos(sf::Packet* packet) override;

  virtual void handleRemoveGround(sf::Packet* packet) override;

  virtual void handleAddEnemy(sf::Packet* packet) override;

  virtual void handleEnemySetPos(sf::Packet* packet) override;

  virtual void handleEnemyChangeState(sf::Packet* packet) override;

  virtual void handleEnemyHorizontalDir(sf::Packet* packet) override;

  virtual void handleEnemyDie(sf::Packet* packet) override;

  virtual void handleRemoveEnemy(sf::Packet* packet) override;

  virtual void handleAddCollectable(sf::Packet* packet) override;

  virtual void handleCollectableCollected(sf::Packet* packet) override;

  virtual void handleRemoveCollectable(sf::Packet* packet) override;

  virtual void handleSetupRace(sf::Packet* packet) override;

  virtual void handleDeclareWinner(sf::Packet* packet) override;
};
}  // namespace Network