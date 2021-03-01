#pragma once
#include "../SFML Pong/Entity.h"

class Ball : public Entity
{
private:
	float windowWidth, windowHeight = 0;

	void resetPosition();
public:
	Ball(float windowWidth, float windowHeight, float speed);
	void update();
	void bounce(sf::Vector2f normal);
	

};