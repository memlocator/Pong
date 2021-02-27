#include "Entity.h"

Entity::Entity()
	:renderRectangle(sf::Vector2f(5,25))
{
}

void Entity::setPosition(sf::Vector2f pos)
{
	renderRectangle.setPosition(pos);
}

void Entity::setVelocity(sf::Vector2f velocity)
{
	this->velocity = velocity;
}

void Entity::move(sf::Vector2f offset)
{
	renderRectangle.setPosition(renderRectangle.getPosition() + offset);
}

void Entity::setSize(sf::Vector2f size)
{
	renderRectangle.setSize(size);
}

sf::Vector2f Entity::getPosition()
{
	return renderRectangle.getPosition();
}

sf::Vector2f Entity::getVelocity()
{
	return velocity;
}

sf::Vector2f Entity::getSize()
{
	return renderRectangle.getSize();
}

bool Entity::isCollidingWith(Entity& otherEntity)
{
	sf::Vector2f otherPos = otherEntity.getPosition();
	sf::Vector2f otherSize = otherEntity.getSize();

	bool colliding = renderRectangle.getGlobalBounds().intersects(otherEntity.renderRectangle.getGlobalBounds());

	return colliding;
}

void Entity::update()
{
	move(velocity);
}

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(renderRectangle);
}
