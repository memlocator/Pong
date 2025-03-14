#pragma once
#include "Timer.h"
#include <thread>

//Windows related
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

//Strings & console
#include <string>
#include <iostream>

//SFML Networking
#include <SFML/Network.hpp>
#include "NetworkEnums.h"

//Game
#include "Ball.h"

struct ConnectedPlayer
{
	int playerID = -1;
	sf::IpAddress ip;
};

class Server
{
private:
	//Variables
	bool running = false;
	int updatesPerSecond = 60;
	int nrOfPlayers = -1; //Intended to be used as indices on the client too.

	std::vector<Entity> paddles;
	Ball ball;

	//Network
	std::vector<ConnectedPlayer> connectedPlayers;

	unsigned short recievePortUDP = 55002;
	unsigned short sendPortUDP = 40000;

	unsigned short receivePortTCP = 55009;

	sf::TcpListener tcpListener;

	sf::TcpSocket tcpSocket;
	sf::UdpSocket udpSocket;

	sf::Packet serverPacket;

	//Game helpers
	void updateGame();
	void createArena();

	//Network helpers
	void waitForConnectingPlayers();
	void initUDPThread();
	void updateClientsLoop();
	void setupPorts();
	bool areAllPlrsConn();
public:
	Server();
	void run();
};