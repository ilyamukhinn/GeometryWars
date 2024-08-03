#ifndef GAME_H
#define GAME_H

#include "Entity.hpp"
#include "EntityManager.hpp"

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

#define _USE_MATH_DEFINES
#include <math.h>

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig	{ int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game {
	sf::RenderWindow	m_window;
	EntityManager		m_entities;
	sf::Font			m_font;
	sf::Text			m_scoreText;
	PlayerConfig		m_playerConfig;
	EnemyConfig			m_enemyConfig;
	BulletConfig		m_bulletConfig;
	sf::Clock			m_deltaClock;

	int						m_score					{ 0 };
	int						m_currentFrame			{ 0 };
	int						m_lastEnemySpawnTime	{ 0 };
	bool					m_running				{ true };
	bool					m_paused				{ false };
	std::shared_ptr<Entity>	m_player				{ nullptr };
	
	const float				m_cos45				{ static_cast<float>(cos(M_PI * 45 / 180)) };
	const float				m_sin45				{ static_cast<float>(sin(M_PI * 45 / 180)) };
	const int				m_scoreInit			{ 100 };
	const int				m_smallScoreInit	{ 200 };

	void init(const std::string& path);

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2f& target);
	void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

	void sSpawner();
	void sMovement();
	void sLifespan();
	void sCollision();
	void sUserInput();
	void sGUI();
	void sRender();

	bool m_sMovementOn	{ true };
	bool m_sSpawnerOn	{ true };
	bool m_sLifespanOn	{ true };
	bool m_sCollisionOn	{ true };
	bool m_sUserInputOn	{ true };
	bool m_sGUIOn		{ true };
	bool m_sRenderOn	{ true };

public:
	Game(const std::string& config);

	void run();
};

#endif // !GAME_H
