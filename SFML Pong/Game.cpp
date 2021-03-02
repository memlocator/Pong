#include "Game.h"

void Game::setupUDP()
{
    //udpSocket.setBlocking(false);
    udpSocket.bind(receivePortUDP + playerID);
}

int nr = 0;
void Game::pollUDP()
{
    std::thread netPoll([this]()
    {
        while (true)
        {
            sf::Packet packet;
            sf::IpAddress sender;
            unsigned short port;
            udpSocket.receive(packet, sender, port);

            //Packet form: NetworkID enum, then the rest of the data.
            int netIdentifier; //Actually an enum.
            if (packet >> netIdentifier)
            {

                NetworkID netEnum = (NetworkID)netIdentifier;
                netEnum = (NetworkID)netIdentifier;

                switch (netEnum)
                {
                case NetworkID::PADDLE_UPDATE:
                {
                    float paddleYPos = 0.f;
                    int fromPlayerID = -1;
                    packet >> fromPlayerID >> paddleYPos;
                    

                    //Update game state according to new information to client, not super secure 
                    //as the client gets to dictate too much.
                    paddles[fromPlayerID].setPosition({ paddles[fromPlayerID].getPosition().x, paddleYPos });
                }
                    break;
                    
                case NetworkID::BALL_UPDATE:
                {
                    float x, y = 0;
                    packet >> x >> y;
                    ball.setPosition({ x, y });
                }
                    break;
                }
            }
        }
    });
    netPoll.detach();


    
}

void Game::sendPdlToServer()
{
    sf::Packet packet;
    packet << (int)NetworkID::PADDLE_UPDATE << playerID << paddles[playerID].getPosition().y;

    udpSocket.send(packet, serverIp, sendPortUDP);
    
}

void Game::handleInput()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Up)
                paddles[playerID].setVelocity(paddles[playerID].getVelocity() + sf::Vector2f(0, -paddleVelocity));
            if (event.key.code == sf::Keyboard::Down)
                paddles[playerID].setVelocity(paddles[playerID].getVelocity() + sf::Vector2f(0, paddleVelocity));
        }
        if (event.type == sf::Event::KeyReleased)
        {
            if (event.key.code == sf::Keyboard::Up)
                paddles[playerID].setVelocity(paddles[playerID].getVelocity() + sf::Vector2f(0, paddleVelocity));
            if (event.key.code == sf::Keyboard::Down)
                paddles[playerID].setVelocity(paddles[playerID].getVelocity() + sf::Vector2f(0, -paddleVelocity));
        }

        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void Game::createArena()
{
    paddles.push_back(Entity());
    paddles.push_back(Entity());
    paddles.at(0).setPosition({ 30.f, 0 });
    paddles.at(1).setPosition({ 600 - paddles.at(1).getSize().x - 30.f, 0 });

    ball.setSize({ 5,5 });
}

void Game::updateElements()
{
    for (auto& paddle : paddles)
        paddle.update();
}

void Game::drawElements()
{
    for (auto& paddle : paddles)
        window.draw(paddle);
    window.draw(ball);
}

void Game::getPlayerIndex()
{
    //Send inquiry
    sf::TcpSocket socket;
    sf::Packet packet;
    socket.connect(serverIp, sendPortTCP);
    packet << (int)NetworkID::REQUEST_PLR_NUM;
    socket.send(packet);
    packet.clear();

    //Get response
    socket.receive(packet);
    packet >> playerID;

    std::cout << "I have client id " << playerID << std::endl;

    socket.disconnect();
}

void Game::getFolderPath()
{
    size_t slashIndex = binaryPath.find_last_of('\\');
    binaryPath = binaryPath.substr(0, slashIndex);
}

void Game::setIP()
{
    getFolderPath();

    std::ifstream ipFile;
    ipFile.open(binaryPath + "\\IP.txt");
    if (ipFile.is_open())
    {
        std::string tempIPAddr;
        ipFile >> tempIPAddr;
        serverIp = sf::IpAddress(tempIPAddr);
        std::cout << tempIPAddr << std::endl;
        ipFile.close();
    }
    else
        std::cout << "Failed to open IP file, defaulting to localhost." << std::endl;
}

Game::Game(std::string binaryPath)
    :window(sf::VideoMode(width, height), "Pong - awaiting server handshake"), binaryPath(binaryPath)
{
    window.setKeyRepeatEnabled(false);
}

void Game::run()
{
    setIP();

    createArena();
    getPlayerIndex();
    setupUDP();
	
    //Time measurements
    Timer timer;
    timer.start();

    //Change title for window
    window.setTitle(" Pong Client " + std::to_string(playerID));
    pollUDP();

    while (window.isOpen()) //Primary game, game and logic loop
    {

        if (timer.timeElapsed() > (1000.0 / updatesPerSecond)) //Measured in miliseconds
        {
            //Begin frame
            window.clear();
            handleInput();
            updateElements();

            sendPdlToServer();

            drawElements();
            window.display();
            //End frame

            timer.restart();
        }
       


    }
}
