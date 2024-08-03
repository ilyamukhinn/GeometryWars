#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "Entity.hpp"
#include "Enums.hpp"

#include <vector>
#include <memory>
#include <map>
#include <string>

typedef std::vector<std::shared_ptr<Entity>>	EntityVec;
typedef std::map<EntityTag, EntityVec>			EntityMap;

class EntityManager {
	EntityVec	m_entities;
	EntityVec	m_entitiesToAdd;
	EntityMap	m_entityMap;
	size_t		m_totalEntities{ 0 };

	void removeDeadEntities(EntityVec& vec);
	void concatVec(EntityVec& firstVec, const EntityVec& secondVec);
public:
	EntityManager() = default;

	void update();
	std::shared_ptr<Entity> addEntity(const EntityTag& tag);
	const EntityVec& getEntities();
	const EntityVec& getEntities(const EntityTag& tag);
	EntityVec getEntities(const EntityTagMask& tagMask);
	const EntityMap& getEntityMap();

};

#endif // !ENTITY_MANAGER_H
