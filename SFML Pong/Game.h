#pragma once
//Windows related
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

//SFML related
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

//Game related items
#include "Entity.h"
#include "../SFML Pong Server/Timer.h"

//Standard library
#include <string>
#include <iostream>
#include <thread>
#include "../SFML Pong Server/NetworkEnums.h"

class Game
{
private:
	//Game variables
	float width = 600;
	float height = 400;

	float paddleVelocity = 2.f;

	sf::RenderWindow window;
	std::vector<Entity> paddles;

	int playerID = -1;
	Entity ball;

	int updatesPerSecond = 60;

	//Network
	sf::IpAddress serverIp = "localhost";
	unsigned short receivePortUDP = 40000; //Offset with player id in setup
	unsigned short sendPortUDP = 55002;
	unsigned short sendPortTCP = 55003;

	sf::UdpSocket udpSocket;
	void setupUDP();
	void pollUDP();
	void sendPdlToServer();

	//Helpers
	void handleInput();
	void createArena();
	void updateElements();
	void drawElements();
	void getPlayerIndex();
public:
	Game();
	void run();

};