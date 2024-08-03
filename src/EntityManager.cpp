#include "EntityManager.hpp"

void EntityManager::update() {
	for (auto entity : this->m_entitiesToAdd) {
		this->m_entities.push_back(entity);
		this->m_entityMap[entity->tag()].push_back(entity);
	}

	this->removeDeadEntities(this->m_entities);

	for (auto& [tag, entityVec] : this->m_entityMap) {
		this->removeDeadEntities(entityVec);
	}


	m_entitiesToAdd.clear();
}

void EntityManager::removeDeadEntities(EntityVec& vec) {
	std::erase_if(vec, [](const std::shared_ptr<Entity>& entity) {
		return !entity->isActive();
		}
	);
}

std::shared_ptr<Entity> EntityManager::addEntity(const EntityTag& tag) {
	auto entity = std::shared_ptr<Entity>(new Entity(this->m_totalEntities++, tag));
	this->m_entitiesToAdd.push_back(entity);
	return entity;
}

const EntityVec& EntityManager::getEntities() {
	return this->m_entities;
}

const EntityVec& EntityManager::getEntities(const EntityTag& tag) {
	return this->m_entityMap[tag];
}

EntityVec EntityManager::getEntities(const EntityTagMask& tagMask) {
	EntityVec res;

	if (tagMask.has(EntityTag::BULLET)) {
		if (this->m_entityMap.find(EntityTag::BULLET) != this->m_entityMap.end()) {
			const auto& bulletVec = this->getEntities(EntityTag::BULLET);
			concatVec(res, bulletVec);
		}
	}

	if (tagMask.has(EntityTag::PLAYER)) {
		if (this->m_entityMap.find(EntityTag::PLAYER) != this->m_entityMap.end()) {
			const auto& playerVec = this->getEntities(EntityTag::PLAYER);
			concatVec(res, playerVec);
		}
	}

	if (tagMask.has(EntityTag::ENEMY)) {
		if (this->m_entityMap.find(EntityTag::ENEMY) != this->m_entityMap.end()) {
			const auto& enemyVec = this->getEntities(EntityTag::ENEMY);
			concatVec(res, enemyVec);
		}
	}

	if (tagMask.has(EntityTag::SMALL_ENEMY)) {
		if (this->m_entityMap.find(EntityTag::SMALL_ENEMY) != this->m_entityMap.end()) {
			const auto& smallEnemyVec = this->getEntities(EntityTag::SMALL_ENEMY);
			concatVec(res, smallEnemyVec);
		}
	}

	return res;
}

void EntityManager::concatVec(EntityVec& firstVec, const EntityVec& secondVec) {
	firstVec.insert(firstVec.end(),
		secondVec.begin(),
		secondVec.end());
}

const EntityMap& EntityManager::getEntityMap() {
	return this->m_entityMap;
}