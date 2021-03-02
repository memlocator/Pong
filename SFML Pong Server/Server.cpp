#include "Server.h"

void Server::updateGame()
{
	ball.update();

	for (int i = 0; i < paddles.size(); i++)
	{
		if (paddles[i].isCollidingWith(ball))
		{
			//Identify normal direction of paddle
			int mult = 1; //Left paddle
			if (i == 0)
				mult = -1; //Right paddle
			//

			ball.bounce({ 1.f, 0.f });
			ball.setVelocity(ball.getVelocity() - sf::Vector2f(0, paddles[i].getDeltaY()*0.25f));
			ball.setPosition(ball.getPosition() - sf::Vector2f(paddles[i].getSize().x * mult, 0));
			ball.normalizeVelocity();
		}
	}
}

void Server::createArena()
{
	paddles.push_back(Entity());
	paddles.push_back(Entity());
	paddles.at(0).setPosition({ 30.f, 0 });
	paddles.at(1).setPosition({ 600 - paddles.at(1).getSize().x - 30.f, 0 });

	ball.setPosition({ 200,200 });
	ball.setSize({ 5,5 });
	ball.setVelocity({ 2.f, 0.85f });
	ball.normalizeVelocity();
}

void Server::waitForConnectingPlayers()
{
	std::cout << "Server is waiting for players..." << std::endl;
	for (int i = 0; i < 2; i++)
	{
		//----------------------------------------------Check TCP
		//Ensure a clean packet.
		serverPacket.clear();

		tcpListener.accept(tcpSocket);
		tcpSocket.receive(serverPacket);

		//Packet form: NetworkID enum, then the gamestate data.
		int netIdentifier; //Actually an enum.
		serverPacket >> netIdentifier;

		NetworkID netEnum = (NetworkID)netIdentifier;
		switch (netEnum)
		{
		case NetworkID::REQUEST_PLR_NUM:
			serverPacket.clear(); //Prepare for response
			serverPacket << ++nrOfPlayers;
			tcpSocket.send(serverPacket);

			connectedPlayers.push_back({ nrOfPlayers, tcpSocket.getRemoteAddress() });

			std::cout << tcpSocket.getRemoteAddress() << " connected, " << nrOfPlayers + 1 << " players now." << std::endl;
			break;
		}

		tcpSocket.disconnect();
	}

	std::cout << "Game starting." << std::endl;

}

void Server::initUDPThread()
{
	//----------------------------------------------Check UDP
	//Ensure a clean packet.

	std::thread netPoll([this]()
		{
			while (running)
			{
				serverPacket.clear();
				sf::IpAddress sender;
				unsigned short port;
				udpSocket.receive(serverPacket, sender, port);
				int netIdentifier = -1;

				if (serverPacket >> netIdentifier)
				{
					NetworkID netEnum = (NetworkID)netIdentifier;

					switch (netEnum)
					{
					case NetworkID::PADDLE_UPDATE:
						int playerID = -1;
						float paddleYPos = -1.f;
						if (serverPacket >> playerID >> paddleYPos)
						{
							paddles[playerID].setDeltaY(paddles[playerID].getPosition().y - paddleYPos);
							paddles[playerID].setPosition({ paddles[playerID].getPosition().x, paddleYPos });
						}

						break;
					}
				}

			}
		});
	netPoll.detach();
}

void Server::updateClientsLoop()
{
	std::thread netPoll([this]()
	{
			//Time measurements
			Timer timer;
			timer.start();

			while (running)
			{
				if (timer.timeElapsed() > (1000.0 / updatesPerSecond)) //Measured in miliseconds
				{
					timer.restart();
					if (areAllPlrsConn())
					{
						sf::Packet packet;

						//Should ideally auto select port instead
						//Update paddle positions
						packet << (int)NetworkID::PADDLE_UPDATE << connectedPlayers[0].playerID << paddles[0].getPosition().y;
						udpSocket.send(packet, connectedPlayers[1].ip, sendPortUDP + 1);

						packet.clear();

						packet << (int)NetworkID::PADDLE_UPDATE << connectedPlayers[1].playerID << paddles[1].getPosition().y;
						udpSocket.send(packet, connectedPlayers[0].ip, sendPortUDP);
						//

						//Update ball position
						packet.clear();
						packet << (int)NetworkID::BALL_UPDATE << ball.getPosition().x << ball.getPosition().y;

						for (int i = 0; i < connectedPlayers.size(); i++)
						{
							udpSocket.send(packet, connectedPlayers[i].ip, sendPortUDP + i);
						}
						//
					}
				
				}
			}
		});
	netPoll.detach();
}

void Server::setupPorts()
{
	std::cout << "Initiating server ports." << std::endl;
	//TCP
	tcpListener.listen(receivePortTCP);
	tcpListener.setBlocking(true);
	tcpSocket.setBlocking(true);

	//UDP
	udpSocket.bind(recievePortUDP);

	std::cout << "Server ports are set up." << std::endl;
}

bool Server::areAllPlrsConn()
{
	return nrOfPlayers == 1;
}

Server::Server()
	:ball(600, 400, 2.f)
{
}

void Server::run()
{
	//Setup
	running = true;

	createArena();
	setupPorts();

	//Time measurements
	Timer timer;
	timer.start();

	waitForConnectingPlayers(); //Get input from clients
	initUDPThread();
	updateClientsLoop(); //Notify clients

	//Primary logic loop
	while (running)
	{
		//Server runs at 60 updates per second.
		if (timer.timeElapsed() > (1000.0 / updatesPerSecond)) //Measured in miliseconds
		{
			
			updateGame(); //Update game

			timer.restart();
		}

	}
}
