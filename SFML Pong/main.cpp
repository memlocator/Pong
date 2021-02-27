//Windows related
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

//SFML related
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

//Game related items
#include "Entity.h"

//Standard library
#include <string>
#include <iostream>
#include <thread>

const unsigned int WIDTH = 600;
const unsigned int HEIGHT = 400;


class Network
{
public:
    static sf::Vector2f ballPosition;
    static void handleNetworkMovement(int localPlayerIndex, std::vector<Entity*>& paddles)
    {
        while (true)
        {
            sf::UdpSocket socket;
            //socket.setBlocking(false);
            socket.bind(70000+localPlayerIndex); //Receive on 55001
            sf::Packet packet;

            //Prepare the packet
            packet << paddles[localPlayerIndex]->getPosition().y << localPlayerIndex;
            std::string identifierMsg;

            //Send packet
            socket.send(packet, "localhost", 55002); //Send on 55002
            packet.clear();

            //Await potential updates
            std::size_t received = 0;
            sf::IpAddress sender;
            unsigned short port;
            socket.receive(packet, sender, port);

            

            float y = 0;
            int fromPlayerId = -1;

            packet >> identifierMsg;
            if (identifierMsg == "paddle")
                if (packet >> y >> fromPlayerId && fromPlayerId != localPlayerIndex && fromPlayerId != -1)
                    paddles[fromPlayerId]->setPosition({ paddles[fromPlayerId]->getPosition().x, y });
            if (identifierMsg == "ball")
            {
                // std::cout << "updating ball" << std::endl;

                float ballX, ballY;
                packet >> ballX >> ballY;

                ballPosition = { ballX, ballY };
                std::cout << ballX << ", " << ballY << std::endl;
            }
                    
            //Decipher messages from packet
           // float ballX, ballY;
           // if (packet >> identifierMsg >> ballX >> ballY && identifierMsg == "ball")
               // Network::ballPosition = { ballX, ballY };
            //std::cout << x << ", " << y << std::endl;

            //std::cout << "Updating position of " << fromPlayerId << std::endl;

            Sleep(1);
        }
        
    }
};
sf::Vector2f Network::ballPosition = sf::Vector2f(0.f, 0.f);


int getPlayerNum() //Ask server if you are player 1 or 2.
{
    DWORD num = GetCurrentProcessId();

    //Send inquiry
    sf::TcpSocket socket;
    sf::Packet packet;
    socket.connect("localhost", 55003);

    std::string msg = "ID";
    packet << msg;
    socket.send(packet);


    //Wait for response
    socket.receive(packet);
    int playerID = -1;
    socket.disconnect();

    if (packet >> playerID)
        std::cout << "You are player" << playerID << std::endl;
    return playerID;
}



void updatePaddles(std::vector<Entity*>& paddles, Entity& ball)
{
    for (auto paddle : paddles)
    {
        paddle->update();
        if (ball.isCollidingWith(*paddle))
        {
            //ball.bounce({ 1.f,0.f });
        }
    }   
}

void drawPaddles(std::vector<Entity*>& paddles, sf::RenderWindow& window)
{
    for (auto paddle : paddles)
        window.draw(*paddle);
}

void createPaddles(std::vector<Entity*>& paddles)
{
    Entity* paddle1 = new Entity();
    Entity* paddle2 = new Entity();
    paddle2->setPosition(sf::Vector2f(WIDTH - paddle2->getSize().x, 0));

    paddles.push_back(paddle1);
    paddles.push_back(paddle2);
}

void cleanUp(std::vector<Entity*> paddles)
{
    for (auto paddle : paddles)
        delete paddle;
    paddles.clear();
}

int main()
{
    int localPlayerIndex = getPlayerNum();
    int otherPaddleIndex = (localPlayerIndex == 0) ? 1 : 0;
    std::vector<Entity*> paddles;
    createPaddles(paddles);
    

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Pong");
    window.setKeyRepeatEnabled(false);

    Entity ball;
    ball.setSize({ 5,5 });


    std::thread networkThread(Network::handleNetworkMovement, localPlayerIndex, std::ref(paddles));
    networkThread.detach();

    while (window.isOpen()) //Primary game, game and logic loop
    {
        updatePaddles(paddles, ball);

        ball.setPosition(Network::ballPosition);

        

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Up)
                    paddles[localPlayerIndex]->setVelocity(paddles[localPlayerIndex]->getVelocity() + sf::Vector2f(0, -0.05f));
                if (event.key.code == sf::Keyboard::Down)
                    paddles[localPlayerIndex]->setVelocity(paddles[localPlayerIndex]->getVelocity() + sf::Vector2f(0, 0.05f));

                //paddle2.setPosition(sendData({ paddle2.getPosition().x, paddles[localPlayerIndex]->getPosition().y }));
            }
            if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Up)
                    paddles[localPlayerIndex]->setVelocity(paddles[localPlayerIndex]->getVelocity() + sf::Vector2f(0, 0.05f));
                if (event.key.code == sf::Keyboard::Down)
                    paddles[localPlayerIndex]->setVelocity(paddles[localPlayerIndex]->getVelocity() + sf::Vector2f(0, -0.05f));
            }

            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ball);
        drawPaddles(paddles, window);
        window.display();

            
    }

    cleanUp(paddles);
    return 0;
}