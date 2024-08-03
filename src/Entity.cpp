#include "Entity.hpp"

Entity::Entity(const size_t id, const EntityTag& tag)
	: m_id(id)
	, m_tag(tag)
{}

bool Entity::isActive() const {
	return this->m_active;
}

const EntityTag& Entity::tag() const {
	return this->m_tag;
}

const size_t Entity::id() const {
	return this->m_id;
}

void Entity::destroy() {
	this->m_active = false;
}