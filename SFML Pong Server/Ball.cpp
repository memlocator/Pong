#include "Ball.h"
#include <iostream>

void Ball::resetPosition()
{
	setPosition({ windowWidth / 2, windowHeight / 2 });

	//Reset velocity of ball, important in case it is moving at steep angles.
	setVelocity({ getVelocity().x, 0.85f });

	bounce({ 1,0 });
}

Ball::Ball(float windowWidth, float windowHeight, float speed)
	:windowWidth(windowWidth), windowHeight(windowHeight)
{
	this->speedMultiplier = speed;
}

void Ball::update()
{
	//Checking bounds
	//Top
	if (renderRectangle.getPosition().y <= 0)
		bounce(sf::Vector2f(0, 1));

	//Bottom
	if (renderRectangle.getPosition().y + renderRectangle.getSize().y / 2 >= windowHeight)
		bounce(sf::Vector2f(0, -1));

	//Right
	if (renderRectangle.getPosition().x + renderRectangle.getSize().x / 2 > windowWidth && !(getVelocity().x < 0))
		resetPosition();

	//Left
	if (renderRectangle.getPosition().x <= 0 && !(getVelocity().x > 0) && !(getVelocity().x > 0))
		resetPosition();

	this->move(getVelocity());
}

void Ball::bounce(sf::Vector2f normal)
{
	
	//Calculate dot product
	float dotProduct = getVelocity().x * normal.x + getVelocity().y * normal.y;

	//Reflect over normal
	sf::Vector2f reflectionVec = getVelocity() - 2 * dotProduct * normal;

	this->setVelocity(reflectionVec);
}


