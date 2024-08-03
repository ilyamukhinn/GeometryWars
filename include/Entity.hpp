#ifndef ENTITY_H
#define ENTITY_H

#include "Components.hpp"
#include "Enums.hpp"
#include <memory>
#include <unordered_map>

class Entity {
	friend class EntityManager;

	bool			m_active	{ true };
	size_t			m_id		{ 0 };
	EntityTag		m_tag		{ EntityTag::UNDEFINED };

	Entity(const size_t id, const EntityTag& tag);
public:

	std::shared_ptr<CTransform> cTransform	{ nullptr };
	std::shared_ptr<CShape>		cShape		{ nullptr };
	std::shared_ptr<CCollision>	cCollision	{ nullptr };
	std::shared_ptr<CInput>		cInput		{ nullptr };
	std::shared_ptr<CScore>		cScore		{ nullptr };
	std::shared_ptr<CLifespan>	cLifespan	{ nullptr };

	bool isActive() const;
	const EntityTag& tag() const;
	const size_t id() const;
	void destroy();
};

#endif // !ENTITY_H
