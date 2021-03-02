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

	float frameDeltaY = 0.f;
	float speedMultiplier = 0.5f;
	float windowHeight, windowWidth = 0.f;
public:
	//Constructor
	Entity(float windowWidth = 600.f, float windowHeight = 400.f);

	//Setters
	void setPosition(sf::Vector2f pos);
	void setVelocity(sf::Vector2f velocity);
	void setSize(sf::Vector2f size);
	void setDeltaY(float val);

	//Getters, would ideally be const
	sf::Vector2f getPosition();
	sf::Vector2f getVelocity();
	sf::Vector2f getSize();
	float getDeltaY();

	//Utility
	bool isCollidingWith(Entity& entity);
	virtual void update();
	void normalizeVelocity();

	// Inherited via Drawable
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

};
