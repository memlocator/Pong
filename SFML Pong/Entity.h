#pragma once

//SFML related
#include <SFML\Graphics.hpp>

class Entity : public sf::Drawable
{
private:
	sf::Vector2f velocity;
	
	//Helpers

protected:
	sf::RectangleShape renderRectangle;
	void move(sf::Vector2f offset);


public:
	//Constructor
	Entity();

	//Setters
	void setPosition(sf::Vector2f pos);
	void setVelocity(sf::Vector2f velocity);
	void setSize(sf::Vector2f size);

	//Getters, would ideally be const
	sf::Vector2f getPosition();
	sf::Vector2f getVelocity();
	sf::Vector2f getSize();

	//Utility
	bool isCollidingWith(Entity& entity);
	virtual void update();

	// Inherited via Drawable
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

};
