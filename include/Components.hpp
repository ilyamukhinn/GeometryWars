#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "Vec2.hpp"
#include <SFML/Graphics.hpp>

class CTransform {
public:
	Vec2f m_pos			{ 0.0f, 0.0f };
	Vec2f m_velocity	{ 0.0f, 0.0f };
	float m_angle		{ 0.0f };
	float m_speed		{ 0.0f };

	CTransform(const Vec2f& p, const Vec2f& v, 
		float a, float m)
		: m_pos(p), m_velocity(v), m_angle(a), m_speed(m) {}
};

class CShape {
public:
	sf::CircleShape m_circle;

	CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness)
		: m_circle(radius, points) {
		m_circle.setFillColor(fill);
		m_circle.setOutlineColor(outline);
		m_circle.setOutlineThickness(thickness);
		m_circle.setOrigin(radius, radius);

	}
};

class CCollision {
public:
	float m_radius { 0.0f };

	CCollision(float r) 
		: m_radius(r) {}
};

class CScore {
public:
	int m_score { 0 };

	CScore(int s)
		: m_score(s) {}
};

class CLifespan {
public:
	int m_remaining	{ 0 }; // amont of lifespan remaining on the entity
	int m_total		{ 0 }; // the total initial amount of lifespan

	CLifespan(int total)
		: m_remaining(total), m_total(total) {}
};


class CInput {
public:
	bool m_up				{ false };
	bool m_left				{ false };
	bool m_right			{ false };
	bool m_down				{ false };
	bool m_shoot			{ false };
	bool m_shoot_special	{ false };

	CInput() {}
};


#endif // !COMPONENTS_H
