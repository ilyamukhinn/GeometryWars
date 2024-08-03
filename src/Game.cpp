#include "Game.hpp"
#include "Enums.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

Game::Game(const std::string& config) {
	init(config);
}

void Game::init(const std::string& path) {
	std::ifstream fin(path);

	if (fin.is_open()) {
		std::string buf{ "" };
		while (!fin.eof()) {
			fin >> buf;
			if (buf == "Window") {
				unsigned int width{ 0 }, height{ 0 }, 
					frameLimit{ 0 }, fullScreen{ 0 };

				fin >> width >> height >> frameLimit >> fullScreen;

				this->m_window.create(
					fullScreen ? sf::VideoMode::getFullscreenModes()[0] : sf::VideoMode(width, height),
					"Assignment 2",
					fullScreen ? sf::Style::Close | sf::Style::Resize : sf::Style::Fullscreen | sf::Style::Resize);
				this->m_window.setFramerateLimit(frameLimit);
			} else if (buf == "Font") {
				std::string filepath{ "" };
				unsigned int characterSize{ 0 }, r{ 0 }, 
					g{ 0 }, b{ 0 };

				fin >> filepath >> characterSize >> r >> g >> b;

				if (!this->m_font.loadFromFile(filepath)) {
					std::cerr << "Unable to load font from file\n";
				}

				this->m_scoreText.setFont(this->m_font);
				this->m_scoreText.setCharacterSize(characterSize);
				this->m_scoreText.setFillColor(sf::Color(r, g, b));
				this->m_scoreText.setString(std::to_string(this->m_score));
			} else if (buf == "Player") {
				fin >> this->m_playerConfig.SR >> this->m_playerConfig.CR >>
					this->m_playerConfig.S >> this->m_playerConfig.FR >>
					this->m_playerConfig.FG >> this->m_playerConfig.FB >>
					this->m_playerConfig.OR >> this->m_playerConfig.OG >>
					this->m_playerConfig.OB >> this->m_playerConfig.OT >>
					this->m_playerConfig.V;
			} else if (buf == "Enemy") {
				fin >> this->m_enemyConfig.SR >> this->m_enemyConfig.CR >>
					this->m_enemyConfig.SMIN >> this->m_enemyConfig.SMAX >>
					this->m_enemyConfig.OR >> this->m_enemyConfig.OG >>
					this->m_enemyConfig.OB >> this->m_enemyConfig.OT >>
					this->m_enemyConfig.VMIN >> this->m_enemyConfig.VMAX >>
					this->m_enemyConfig.L >> this->m_enemyConfig.SI;
			} else if (buf == "Bullet") {
				fin >> this->m_bulletConfig.SR >> this->m_bulletConfig.CR >>
					this->m_bulletConfig.S >> this->m_bulletConfig.FR >>
					this->m_bulletConfig.FG >> this->m_bulletConfig.FB >>
					this->m_bulletConfig.OR >> this->m_bulletConfig.OG >>
					this->m_bulletConfig.OB >> this->m_bulletConfig.OT >>
					this->m_bulletConfig.V >> this->m_bulletConfig.L;
			} else {
				std::cerr << "Unknown config parameter\n";
			}
		}
		fin.close();
	} else {
		std::cerr << "Config file isn't open\n";
	}

	ImGui::SFML::Init(this->m_window);
	ImGui::GetStyle().ScaleAllSizes(1.0f);

	this->spawnPlayer();
}

void Game::run() {
	while (this->m_running) {
		srand(time(0));
		ImGui::SFML::Update(this->m_window, this->m_deltaClock.restart());
		this->m_entities.update();

		if (!this->m_paused) {
			if (this->m_sLifespanOn) {
				this->sLifespan();
			}

			if (this->m_sSpawnerOn) {
				this->sSpawner();
			}

			if (this->m_sMovementOn) {
				this->sMovement();
			}

			if (this->m_sCollisionOn) {
				this->sCollision();
			}
		}

		if (this->m_sUserInputOn) {
			this->sUserInput();
		}

		if (this->m_sGUIOn) {
			this->sGUI();
		}

		if (this->m_sRenderOn) {
			this->sRender();
		} else {
			ImGui::SFML::Render(this->m_window);
		}

		this->m_currentFrame++;
	}
}

void Game::spawnPlayer() {
	auto entity = m_entities.addEntity(EntityTag::PLAYER);

	entity->cTransform = std::make_shared<CTransform>(
		Vec2f{ static_cast<float>(this->m_window.getSize().x) / 2, 
			static_cast<float>(this->m_window.getSize().y) / 2 },
		Vec2f{ 0.0f, 0.0f }, 0.0f, this->m_playerConfig.S);

	entity->cShape = std::make_shared<CShape>(this->m_playerConfig.SR, 
		this->m_playerConfig.V, 
		sf::Color(this->m_playerConfig.FR, 
			this->m_playerConfig.FG, 
			this->m_playerConfig.FB), 
		sf::Color(this->m_playerConfig.OR, 
			this->m_playerConfig.OG, 
			this->m_playerConfig.OB), 
		this->m_playerConfig.OT);

	entity->cCollision = std::make_shared<CCollision>(this->m_playerConfig.CR);
	entity->cInput = std::make_shared<CInput>();

	this->m_player = entity;
}

void Game::spawnEnemy() {
	auto entity = m_entities.addEntity(EntityTag::ENEMY);
	int minStartX{ static_cast<int>(this->m_enemyConfig.CR) },
		maxStartX{ static_cast<int>(this->m_window.getSize().x - this->m_enemyConfig.CR) },
		minStartY{ static_cast<int>(this->m_enemyConfig.CR) },
		maxStartY{ static_cast<int>(this->m_window.getSize().y - this->m_enemyConfig.CR) };

	Vec2f startPos{ static_cast<float>(minStartX + (rand() % (maxStartX - minStartX + 1))),
		static_cast<float>(minStartY + (rand() % (maxStartY - minStartY + 1))) };

	float velocityValue = static_cast<float>(
		this->m_enemyConfig.SMIN + (rand() %
			static_cast<int>(this->m_enemyConfig.SMAX - this->m_enemyConfig.SMIN + 1)));
	
	entity->cTransform = std::make_shared<CTransform>(
		startPos, Vec2f{ velocityValue, velocityValue }, 0.0f, velocityValue);

	int verticesValue = this->m_enemyConfig.VMIN + (rand() %
			static_cast<int>(this->m_enemyConfig.VMAX - this->m_enemyConfig.VMIN + 1));

	sf::Uint8 fillRed = static_cast<sf::Uint8>(rand() % (1 + 255));
	sf::Uint8 fillGreen = static_cast<sf::Uint8>(rand() % (1 + 255));
	sf::Uint8 fillBlue = static_cast<sf::Uint8>(rand() % (1 + 255));

	entity->cShape = std::make_shared<CShape>(this->m_enemyConfig.CR,
		verticesValue,
		sf::Color(fillRed, fillGreen, fillBlue),
		sf::Color(this->m_enemyConfig.OR, this->m_enemyConfig.OG, this->m_enemyConfig.OB),
		this->m_enemyConfig.OT);

	entity->cCollision = std::make_shared<CCollision>(this->m_enemyConfig.CR);
	entity->cScore = std::make_shared<CScore>(this->m_scoreInit);

	this->m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e) {
	std::vector<std::shared_ptr<Entity>> smallEnemies;

	for (size_t i = 0; i < e->cShape->m_circle.getPointCount(); ++i) {
		smallEnemies.push_back(m_entities.addEntity(EntityTag::SMALL_ENEMY));
	}

	float theta{ 0.0f };
	float deltaTheta{ 360.0f / smallEnemies.size() };
	for (auto enemy : smallEnemies) {
		enemy->cTransform = std::make_shared<CTransform>(
			e->cTransform->m_pos,
			Vec2f{ e->cTransform->m_speed * static_cast<float>(cos(theta * M_PI / 180)),
				e->cTransform->m_speed* static_cast<float>(sin(theta * M_PI / 180)) }, 0.0f,
			e->cTransform->m_speed);

		enemy->cShape = std::make_shared<CShape>(this->m_enemyConfig.SR / 3,
			e->cShape->m_circle.getPointCount(),
			e->cShape->m_circle.getFillColor(),
			e->cShape->m_circle.getOutlineColor(),
			this->m_enemyConfig.OT);

		enemy->cCollision = std::make_shared<CCollision>(this->m_enemyConfig.CR / 3);
		enemy->cScore = std::make_shared<CScore>(this->m_smallScoreInit);
		enemy->cLifespan = std::make_shared<CLifespan>(this->m_enemyConfig.L);

		theta += deltaTheta;
	}
}

void Game::spawnBullet(std::shared_ptr<Entity> e, const Vec2f& target) {
	auto entity = m_entities.addEntity(EntityTag::BULLET);
	Vec2f diff{ target.x - e->cTransform->m_pos.x,
		target.y - e->cTransform->m_pos.y };
	diff.normalize();

	entity->cTransform = std::make_shared<CTransform>(
		e->cTransform->m_pos,
		Vec2f{ this->m_bulletConfig.S * diff.x, this->m_bulletConfig.S * diff.y }, 0.0f,
		this->m_bulletConfig.S);

	entity->cShape = std::make_shared<CShape>(this->m_bulletConfig.SR,
		this->m_bulletConfig.V,
		sf::Color(this->m_bulletConfig.FR, this->m_bulletConfig.FG, this->m_bulletConfig.FB),
		sf::Color(this->m_bulletConfig.OR, this->m_bulletConfig.OG, this->m_bulletConfig.OB),
		this->m_bulletConfig.OT);

	entity->cCollision = std::make_shared<CCollision>(this->m_bulletConfig.CR);
	entity->cLifespan = std::make_shared<CLifespan>(this->m_bulletConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> e) {
	std::vector<std::shared_ptr<Entity>> bullets;

	for (size_t i = 0; i < e->cShape->m_circle.getPointCount(); ++i) {
		bullets.push_back(m_entities.addEntity(EntityTag::BULLET));
	}

	float theta{ 0.0f };
	float deltaTheta{360.0f / bullets.size()};
	for (auto bullet : bullets) {
		bullet->cTransform = std::make_shared<CTransform>(
			e->cTransform->m_pos,
			Vec2f{ this->m_bulletConfig.S * static_cast<float>(cos(theta * M_PI / 180)), 
				this->m_bulletConfig.S * static_cast<float>(sin(theta * M_PI / 180)) }, 0.0f,
			this->m_bulletConfig.S);

		bullet->cShape = std::make_shared<CShape>(this->m_bulletConfig.SR,
			this->m_bulletConfig.V,
			sf::Color(this->m_bulletConfig.FR, this->m_bulletConfig.FG, this->m_bulletConfig.FB),
			sf::Color(this->m_bulletConfig.OR, this->m_bulletConfig.OG, this->m_bulletConfig.OB),
			this->m_bulletConfig.OT);

		bullet->cCollision = std::make_shared<CCollision>(this->m_bulletConfig.CR);
		bullet->cLifespan = std::make_shared<CLifespan>(this->m_bulletConfig.L);

		theta += deltaTheta;
	}
}

void Game::sMovement() {
	Vec2f playerVelocity{ 0.0f, 0.0f };
	Vec2f veclocityInAngle{ this->m_cos45 * this->m_playerConfig.S,
		this->m_sin45 * this->m_playerConfig.S };

	if (this->m_player->cInput->m_left) {
		playerVelocity.x -= this->m_playerConfig.S;
	}

	if (this->m_player->cInput->m_right) {
		playerVelocity.x += this->m_playerConfig.S;
	}

	if (this->m_player->cInput->m_up) {
		playerVelocity.y -= this->m_playerConfig.S;
	}

	if (this->m_player->cInput->m_down) {
		playerVelocity.y += this->m_playerConfig.S;
	}

	if (this->m_player->cInput->m_left &&
		this->m_player->cInput->m_up) {
		playerVelocity = -veclocityInAngle;
	}

	if (this->m_player->cInput->m_left &&
		this->m_player->cInput->m_down) {
		playerVelocity.x = -veclocityInAngle.x;
		playerVelocity.y = veclocityInAngle.y;
	}

	if (this->m_player->cInput->m_right &&
		this->m_player->cInput->m_up) {
		playerVelocity.x = veclocityInAngle.x;
		playerVelocity.y = -veclocityInAngle.y;
	}

	if (this->m_player->cInput->m_right &&
		this->m_player->cInput->m_down) {
		playerVelocity = veclocityInAngle;
	}

	for (auto e : this->m_entities.getEntities()) {
		if (e->cTransform && e->tag() != EntityTag::PLAYER) {
			e->cTransform->m_pos += e->cTransform->m_velocity;
		} else {
			e->cTransform->m_pos += playerVelocity;
		}

		e->cTransform->m_angle += 2.0f;
		e->cShape->m_circle.setRotation(e->cTransform->m_angle);
	}
}

void Game::sLifespan() {
	for (auto e : this->m_entities.getEntities()) {
		if (!e->cLifespan) {
			continue;
		}

		if (e->cLifespan->m_remaining > 0) {
			e->cLifespan->m_remaining--;
		}

		if (e->isActive() && e->cLifespan->m_remaining > 0) {
			sf::Uint8 newAlpha = 255 * e->cLifespan->m_remaining / e->cLifespan->m_total;
			
			auto fillColor = e->cShape->m_circle.getFillColor();
			fillColor.a = newAlpha;
			e->cShape->m_circle.setFillColor(fillColor);

			auto outlineColor = e->cShape->m_circle.getOutlineColor();
			outlineColor.a = newAlpha;
			e->cShape->m_circle.setOutlineColor(outlineColor);

		} else if (e->cLifespan->m_remaining <= 0) {
			e->destroy();
		}
	}
}

void Game::sCollision() {
	// Collisions between player and enemies
	EntityTagMask enemy_smallEnemyMask;
	enemy_smallEnemyMask |= EntityTag::ENEMY;
	enemy_smallEnemyMask |= EntityTag::SMALL_ENEMY;

	for (auto e : this->m_entities.getEntities(enemy_smallEnemyMask)) {
		if (e->isActive() && this->m_player->isActive()) {
			Vec2f diff{ e->cTransform->m_pos.x - this->m_player->cTransform->m_pos.x,
			e->cTransform->m_pos.y - this->m_player->cTransform->m_pos.y };

			float dist2 = diff.length2();
			float r2 = (e->cCollision->m_radius + this->m_player->cCollision->m_radius) *
				(e->cCollision->m_radius + this->m_player->cCollision->m_radius);

			if (r2 >= dist2) {
				this->m_score = 0;
				this->m_scoreText.setString(std::to_string(this->m_score));

				e->destroy();
				this->m_player->destroy();
			}
		}
	}

	// Collisions between bullets and (small) enemies
	for (auto b : this->m_entities.getEntities(EntityTag::BULLET)) {
		if (b->isActive()) {
			for (auto e : this->m_entities.getEntities(enemy_smallEnemyMask)) {
				if (e->isActive()) {
					Vec2f diff{ e->cTransform->m_pos.x - b->cTransform->m_pos.x,
						e->cTransform->m_pos.y - b->cTransform->m_pos.y };

					float dist2 = diff.length2();
					float r2 = (e->cCollision->m_radius + b->cCollision->m_radius) *
						(e->cCollision->m_radius + b->cCollision->m_radius);

					if (r2 >= dist2) {
						this->m_score += e->cScore->m_score;
						this->m_scoreText.setString(std::to_string(this->m_score));

						e->destroy();
						b->destroy();

						if (e->tag() == EntityTag::ENEMY) {
							this->spawnSmallEnemies(e);
						}
					}
				}
			}
		}
	}

	// Collisions between player and 'walls'
	if (this->m_player->isActive()) {
		if (this->m_player->cTransform->m_pos.x + this->m_player->cCollision->m_radius > this->m_window.getSize().x ||
			this->m_player->cTransform->m_pos.x - this->m_player->cCollision->m_radius < 0) {
			this->m_player->destroy();
		}

		if (this->m_player->cTransform->m_pos.y + this->m_player->cCollision->m_radius > this->m_window.getSize().y ||
			this->m_player->cTransform->m_pos.y - this->m_player->cCollision->m_radius < 0) {
			this->m_player->destroy();
		}
	}

	// Collisions between enemies and 'walls'
	for (auto e : this->m_entities.getEntities(EntityTag::ENEMY)) {
		if (e->isActive()) {
			if (e->cTransform->m_pos.x + e->cCollision->m_radius > this->m_window.getSize().x ||
				e->cTransform->m_pos.x - e->cCollision->m_radius < 0) {
				e->cTransform->m_velocity.x *= -1;
			}

			if (e->cTransform->m_pos.y + e->cCollision->m_radius > this->m_window.getSize().y ||
				e->cTransform->m_pos.y - e->cCollision->m_radius < 0) {
				e->cTransform->m_velocity.y *= -1;
			}
		}
	}

}

void Game::sSpawner() {
	if (this->m_currentFrame - this->m_lastEnemySpawnTime >= this->m_enemyConfig.SI) {
		this->spawnEnemy();
	}

	if (!this->m_player->isActive()) {
		this->spawnPlayer();
	}

	if (this->m_player->cInput->m_shoot) {
		Vec2f mousePos{ static_cast<float>(sf::Mouse::getPosition().x),
			static_cast<float>(sf::Mouse::getPosition().y) };
		this->spawnBullet(this->m_player, mousePos);
		this->m_player->cInput->m_shoot = false;
	}

	if (this->m_player->cInput->m_shoot_special) {
		this->spawnSpecialWeapon(this->m_player);
		this->m_player->cInput->m_shoot_special = false;
	}
}

void Game::sGUI() {
	// ImGui::ShowDemoWindow();

	ImGui::Begin("Geometry Wars");

	if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None)) {
		if (ImGui::BeginTabItem("Systems")) {
			ImGui::Checkbox("Lifespan", &this->m_sLifespanOn);
			ImGui::Checkbox("Spawning", &this->m_sSpawnerOn);

			ImGui::SliderInt("Spawning interval", &this->m_enemyConfig.SI, 0, 120);

			ImGui::Checkbox("Movement", &this->m_sMovementOn);
			ImGui::Checkbox("Collision", &this->m_sCollisionOn);
			ImGui::Checkbox("User input", &this->m_sUserInputOn);
			// ImGui::Checkbox("Rendering"	, &this->m_sRenderOn);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Entities")) {
			auto constructTable{ [](const EntityVec& entities) {
				if (ImGui::BeginTable("Entities", 5)) {
					for (auto e : entities) {
						ImGui::TableNextRow();

						ImGui::TableNextColumn();
						ImGui::Text("%d", e->id());
						ImGui::TableNextColumn();
						ImGui::Text("%s", entityTagToStr.at(e->tag()).c_str());
						ImGui::TableNextColumn();
						ImGui::Text("(%.f, %.f)", e->cTransform->m_pos.x, e->cTransform->m_pos.y);
					}
					ImGui::EndTable();
				}
			} };

			if (ImGui::CollapsingHeader("Entities by Tag")) {
				ImGui::PushItemWidth(100);
				ImGui::Indent();
				if (ImGui::CollapsingHeader("bullet")) {
					constructTable(this->m_entities.getEntities(EntityTag::BULLET));
				}
				ImGui::Unindent();

				ImGui::Indent();
				if (ImGui::CollapsingHeader("enemy")) {
					constructTable(this->m_entities.getEntities(EntityTag::ENEMY));
				}
				ImGui::Unindent();

				ImGui::Indent();
				if (ImGui::CollapsingHeader("player")) {
					constructTable(this->m_entities.getEntities(EntityTag::PLAYER));
				}
				ImGui::Unindent();

				ImGui::Indent();
				if (ImGui::CollapsingHeader("small enemy")) {
					constructTable(this->m_entities.getEntities(EntityTag::SMALL_ENEMY));
				}
				ImGui::Unindent();
				ImGui::PopItemWidth();
			}

			if (ImGui::CollapsingHeader("All Entities")) {
				constructTable(this->m_entities.getEntities());
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void Game::sRender() {
	this->m_window.clear();

	for (auto e : this->m_entities.getEntities()) {
		e->cShape->m_circle.setPosition(e->cTransform->m_pos.x, e->cTransform->m_pos.y);
		this->m_window.draw(e->cShape->m_circle);
	}

	this->m_window.draw(this->m_scoreText);

	ImGui::SFML::Render(this->m_window);
	this->m_window.display();
}

void Game::sUserInput() {
	sf::Event event;
	while (this->m_window.pollEvent(event)) {
		ImGui::SFML::ProcessEvent(this->m_window, event);

		switch (event.type)
		{
		case sf::Event::Closed:
			this->m_running = false;
			break;
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::A) {
				std::cout << "A Key Pressed\n";
				this->m_player->cInput->m_left = true;
			}

			if (event.key.code == sf::Keyboard::D) {
				std::cout << "D Key Pressed\n";
				this->m_player->cInput->m_right = true;
			}

			if (event.key.code == sf::Keyboard::W) {
				std::cout << "W Key Pressed\n";
				this->m_player->cInput->m_up = true;
			}

			if (event.key.code == sf::Keyboard::S) {
				std::cout << "S Key Pressed\n";
				this->m_player->cInput->m_down = true;
			}

			if (event.key.code == sf::Keyboard::Escape) {
				this->m_running = false;
			}

			if (event.key.code == sf::Keyboard::P) {
				std::cout << "P Key Pressed\n";
				this->m_paused = !this->m_paused;
			}

			break;
		case sf::Event::KeyReleased:
			if (event.key.code == sf::Keyboard::A) {
				std::cout << "A Key Released\n";
				this->m_player->cInput->m_left = false;
			}

			if (event.key.code == sf::Keyboard::D) {
				std::cout << "D Key Released\n";
				this->m_player->cInput->m_right = false;
			}

			if (event.key.code == sf::Keyboard::W) {
				std::cout << "W Key Released\n";
				this->m_player->cInput->m_up = false;
			}

			if (event.key.code == sf::Keyboard::S) {
				std::cout << "S Key Released\n";
				this->m_player->cInput->m_down = false;
			}

			break;
		default:
			break;
		}

		switch (event.type)
		{
		case sf::Event::MouseButtonPressed:
			if (ImGui::GetIO().WantCaptureMouse) {
				continue;
			}

			if (event.mouseButton.button == sf::Mouse::Left) {
				std::cout << "Left Mouse Button Clicked at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")\n";
				this->m_player->cInput->m_shoot = true;
			}

			if (event.mouseButton.button == sf::Mouse::Right) {
				std::cout << "Right Mouse Button Clicked at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")\n";
				this->m_player->cInput->m_shoot_special = true;
			}
			break;
		case sf::Event::MouseButtonReleased:
			if (ImGui::GetIO().WantCaptureMouse) {
				continue;
			}

			if (event.mouseButton.button == sf::Mouse::Left) {
				std::cout << "Left Mouse Button Clicked at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")\n";
				this->m_player->cInput->m_shoot = false;
			}

			if (event.mouseButton.button == sf::Mouse::Right) {
				std::cout << "Right Mouse Button Clicked at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")\n";
				this->m_player->cInput->m_shoot_special = false;
			}
			break;
		default:
			break;
		}

		if (event.type == sf::Event::MouseButtonPressed) {
			if (ImGui::GetIO().WantCaptureMouse) {
				continue;
			}

			if (event.mouseButton.button == sf::Mouse::Left) {
				std::cout << "Left Mouse Button Clicked at (" << event.mouseButton.x << ", " << event.mouseButton.y << std::endl;
				this->m_player->cInput->m_shoot = true;
			}

			if (event.mouseButton.button == sf::Mouse::Right) {
				std::cout << "Right Mouse Button Clicked at (" << event.mouseButton.x << ", " << event.mouseButton.y << std::endl;
			}
		}
	}
}