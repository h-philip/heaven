/**
 * @file client.h
 * @author Philip Hönnecke (p.hoennecke@tu-braunschweig.de)
 * @brief Header file for Client class.
 * @version 0.1
 * @date 2019-11-29
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include "network_manager.h"

namespace Network {
/** \class Client
 *  @brief NetworkManager as client.
 */
class Client : public NetworkManager {
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Other Variables */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 private:
  sf::IpAddress host; /**< The ip address of the host. */

  sf::TcpSocket* tcp_socket = nullptr; /**< TcpSocket for sending/receiving
                                       to/from host */

  sf::UdpSocket* udp_socket = nullptr; /**< UdpSocket for sending/receiving
                                       to/from host */

  Characters::Factory* character_factory;     /**< Factory for characters */
  Collectables::Factory* collectable_factory; /**< Factory for collectables */

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  /* Methods */
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 public:
  // ----------------------------------------- //
  /* Constructors */
  // ----------------------------------------- //

  /**
   * @brief Constructs a new Client object.
   *
   * @param world Pointer to the world.
   * @param udp_port Port for udp connections.
   * @param tcp_port Port for tcp connections.
   */
  Client(World* world, Characters::Factory* character_factory,
         Collectables::Factory* collectable_factory,
         uint16_t udp_port = UDP_PORT, uint16_t tcp_port = TCP_PORT)
      : NetworkManager(world, udp_port, tcp_port),
        character_factory(character_factory),
        collectable_factory(collectable_factory) {
    this->udp_socket = new sf::UdpSocket();
    this->tcp_socket = new sf::TcpSocket();
    this->udp_socket->setBlocking(false);
    this->tcp_socket->setBlocking(false);
  }

  /**
   * @brief Destroys the Client object.
   *
   */
  virtual ~Client() {
    this->udp_socket->unbind();
    delete this->udp_socket;
    this->tcp_socket->disconnect();
    delete this->tcp_socket;
  }

  // ----------------------------------------- //
  /* Other Methods */
  // ----------------------------------------- //

  bool isHost() const override { return false; }

  void update() override;

  /**
   * @brief Connects to host and sends everything needed for the host at first
   * "contact".
   *
   * @param host Ip address of the host to connect to.
   * @return true On success.
   * @return false On failure.
   */
  bool start(sf::IpAddress host);

  // ----------------------------------------- //
  /* From World Methods */
  // ----------------------------------------- //
 public:
  void doWantAddPlayer(Characters::Player* p);

  void doPlayerChangeState(const Characters::Player* p);

  void doPlayerHorizontalDir(const Characters::Player* p);

  void doPlayerTakeDamage(const Characters::Player* p, sf::Int8 damage);

  void doPlayerHeal(const Characters::Player* p, sf::Uint8 new_hp);

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