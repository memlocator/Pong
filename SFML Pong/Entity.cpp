#include "Entity.h"

Entity::Entity(float windowWidth, float windowHeight)
	:renderRectangle(sf::Vector2f(5,25)), windowWidth(windowWidth), windowHeight(windowHeight)
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

void Entity::setDeltaY(float val)
{
	frameDeltaY = val;
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

float Entity::getDeltaY()
{
	return frameDeltaY;
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
	bool validMove = false;

	//Top and bottom bounds check
	if (renderRectangle.getPosition().y + velocity.y > 0.f
		&& renderRectangle.getPosition().y + renderRectangle.getSize().y + velocity.y < windowHeight)
		validMove = true;

	if (validMove)
		move(velocity);
}

void Entity::normalizeVelocity()
{
	float len = sqrtf((velocity.x * velocity.x) + (velocity.y * velocity.y));
	velocity = sf::Vector2f(velocity.x / len, velocity.y / len) * speedMultiplier;
}

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(renderRectangle);
}
