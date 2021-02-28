//Windows related
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

//Standard C++ libraries
#include <iostream>
#include <string>
#include <thread>

//SFML related
#include <SFML/Network.hpp>

//Game related objects
#include "Ball.h"

//Server
#include "Server.h"

void initiateConsole()
{
    AllocConsole();
    FILE* newstdin = nullptr;
    FILE* newstdout = nullptr;
    FILE* newstderr = nullptr;

    freopen_s(&newstdin, "CONIN$", "r", stdin);
    freopen_s(&newstdout, "CONOUT$", "w", stdout);
    freopen_s(&newstderr, "CONOUT$", "w", stderr);
    std::cout << "Server has been initiated." << std::endl;
}

const unsigned int WIDTH = 600;
const unsigned int HEIGHT = 400;

class GameServer
{
public:
    static std::vector<Entity> paddles; //These do not strictly need to be here for more than number representations

    static void gameloop(Ball& ball)
    {
        sf::UdpSocket udpSocket;
        sf::Packet packet;
        float ballX, ballY = 0;
        
        std::string msgIdentifier = "ball";
        


        ball.setVelocity({ 1.f,-1.f });
        while (true)
        {
            ball.update();
            packet.clear();
            ballX = ball.getPosition().x;
            ballY = ball.getPosition().y;
            packet << msgIdentifier << ballX << ballY;

            udpSocket.send(packet, "localhost", (unsigned short)40000 + 0);
            udpSocket.send(packet, "localhost", (unsigned short)40000 + 1);

            for (auto paddle : GameServer::paddles)
            {
                if (ball.isCollidingWith(paddle))
                {
                    ball.bounce({ 1.f,0.f });
                }
            }

            Sleep(1000 / 60);
        }
    }
};
std::vector<Entity> GameServer::paddles;



void networkThread()
{
    sf::Packet packet;

    //Two players need to connect before proceeding.
    int nrOfPlayers = -1;
    //Create TCP listener
    sf::TcpListener tcpListener;
    tcpListener.listen(55003);
    while (nrOfPlayers < 1)
    {
        // Wait for a connection
        sf::TcpSocket tcpSocket;
        tcpListener.accept(tcpSocket);

        tcpSocket.receive(packet);
        std::string msg;
        if (packet >> msg)
            if (msg == "ID")
            {
                nrOfPlayers++;
                packet.clear();

                packet << nrOfPlayers;
                tcpSocket.send(packet);
                tcpSocket.disconnect();

                std::cout << nrOfPlayers << " nr of players now." << std::endl;
            }
        Sleep(100);
    }

    sf::UdpSocket udpSocket;
    udpSocket.bind(55002); //Receive on port 55002
    while (true)
    {
        std::size_t received = 0;
        sf::IpAddress sender;
        unsigned short port;

        //Blocking receive call
        udpSocket.receive(packet, sender, port);

        float y = 0;
        int fromPlayerID = -1;
        packet >> y >> fromPlayerID; //Unpack
        packet.clear();

        std::string msgIdentifier = "paddle";
        packet << msgIdentifier <<  y << fromPlayerID;

        if (fromPlayerID != -1)
            GameServer::paddles.at(fromPlayerID).setPosition({ GameServer::paddles.at(fromPlayerID).getPosition().x, y });

        if (fromPlayerID == 1)
            udpSocket.send(packet, "localhost", (unsigned short) 40000 + 0);
        else
            udpSocket.send(packet, "localhost", (unsigned short) 40000 + 1);
        Sleep(1);
    }
}

int main()
{
    initiateConsole();
    Server server;

    server.run();

    /*Ball ball(600, 400);

    GameServer::paddles.push_back(Entity());
    GameServer::paddles.push_back(Entity());
    GameServer::paddles.at(1).setPosition({ 600 - GameServer::paddles.at(1).getSize().x, 0 });


    std::thread gameThread(GameServer::gameloop, std::ref(ball));
    gameThread.detach();

    networkThread();*/

    return 0;
}