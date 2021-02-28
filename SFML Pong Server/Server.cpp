#include "Server.h"

void Server::updateGame()
{
	ball.update();

	for (auto paddle : paddles)
	{
		if (paddle.isCollidingWith(ball))
			ball.bounce({ 1,0 });
	}
}

void Server::createArena()
{
	paddles.push_back(Entity());
	paddles.push_back(Entity());
	paddles.at(1).setPosition({ 600 - paddles.at(1).getSize().x, 0 });

	ball.setPosition({ 200,200 });
	ball.setSize({ 5,5 });
	ball.setVelocity({ 2.f, 0.85f });
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

			connectedClients.push_back({ nrOfPlayers, tcpSocket.getRemoteAddress() });

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
							paddles[playerID].setPosition({ paddles[playerID].getPosition().x, paddleYPos });
						}

						break;
					}
				}

			}
		});
	netPoll.detach();
}

int other(int n)
{
	if (n == 1)
		return 0;
	else
		return 1;
}

void Server::updateClientsLoop()
{
	std::thread netPoll([this]()
	{
		while (running)
		{

			if (areAllPlrsConn())
			{
				sf::Packet packet;

				//Should ideally auto select port instead
				//Update paddle positions
				packet << (int)NetworkID::PADDLE_UPDATE << connectedClients[0].playerID << paddles[0].getPosition().y;
				udpSocket.send(packet, connectedClients[1].ip, sendPortUDP + 1);

				packet.clear();

				packet << (int)NetworkID::PADDLE_UPDATE << connectedClients[1].playerID << paddles[1].getPosition().y;
				udpSocket.send(packet, connectedClients[0].ip, sendPortUDP);
				//

				//Update ball position
				packet.clear();
				packet << (int)NetworkID::BALL_UPDATE << ball.getPosition().x << ball.getPosition().y;

				for (int i = 0; i < connectedClients.size(); i++)
				{
					udpSocket.send(packet, connectedClients[i].ip, sendPortUDP + i);
				}
				//

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

	//UDP
	udpSocket.bind(recievePortUDP);

	std::cout << "Server ports are set up." << std::endl;
}

bool Server::areAllPlrsConn()
{
	return nrOfPlayers == 1;
}

Server::Server()
	:ball(600, 400)
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

	initUDPThread();
	waitForConnectingPlayers(); //Get input from clients
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
