#ifndef VEC2_H
#define VEC2_H

#include <cmath>

template <typename T>
class Vec2 {
public:
	T x	{ 0 };
	T y	{ 0 };

	Vec2() = default;
	Vec2<T>(T x, T y);
	Vec2<T>(const Vec2<T>& other);

	void normalize();
	T length();
	T length2(); // vector's length without sqrt

	Vec2<T>	operator-();

	Vec2<T>& operator+=(const Vec2<T>& other);
	Vec2<T>& operator-=(const Vec2<T>& other);
	Vec2<T>& operator*=(const T scalar);
	Vec2<T>& operator/=(const T scalar);

	Vec2<T>	operator+(const Vec2<T>& other);
	Vec2<T>	operator-(const Vec2<T>& other);
	Vec2<T>	operator*(const T scalar);
	Vec2<T>	operator/(const T scalar);

	bool operator==(const Vec2<T>& other) const;
	bool operator!=(const Vec2<T>& other) const;
};

template <typename T>
Vec2<T>::Vec2(T x, T y)
	: x(x), y(y)
{}

template <typename T>
Vec2<T>::Vec2(const Vec2<T>& other)
	: x(other.x), y(other.y)
{}

template <typename T>
void Vec2<T>::normalize() {
	T length = this->length();
	this->x /= length;
	this->y /= length;
}

template <typename T>
T Vec2<T>::length() {
	return sqrt(this->x * this->x + this->y * this->y);
}

template <typename T>
T Vec2<T>::length2() {
	return (this->x * this->x + this->y * this->y);
}

template <typename T>
Vec2<T> Vec2<T>::operator-() {
	return Vec2<T>(-this->x, -this->y);
}

template <typename T>
Vec2<T>& Vec2<T>::operator+=(const Vec2<T>& other) {
	this->x += other.x;
	this->y += other.y;
	return *this;
}

template <typename T>
Vec2<T>& Vec2<T>::operator-=(const Vec2<T>& other) {
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}

template <typename T>
Vec2<T>& Vec2<T>::operator*=(const T scalar) {
	this->x *= scalar;
	this->y *= scalar;
	return *this;
}

template <typename T>
Vec2<T>& Vec2<T>::operator/=(const T scalar) {
	this->x /= scalar;
	this->y /= scalar;
	return *this;
}

template <typename T>
Vec2<T>	Vec2<T>::operator+(const Vec2<T>& other) {
	return Vec2<T>(this->x + other.x, this->y + other.y);
}

template <typename T>
Vec2<T>	Vec2<T>::operator-(const Vec2<T>& other) {
	return Vec2<T>(this->x - other.x, this->y - other.y);
}

template <typename T>
Vec2<T>	Vec2<T>::operator*(const T scalar) {
	return Vec2<T>(this->x * scalar, this->y * scalar);
}

template <typename T>
Vec2<T>	Vec2<T>::operator/(const T scalar) {
	return Vec2<T>(this->x / scalar, this->y / scalar);
}

template <typename T>
bool Vec2<T>::operator==(const Vec2<T>& other) const {
	return this->x == other.x && this->y == other.y;
}

template <typename T>
bool Vec2<T>::operator!=(const Vec2<T>& other) const {
	return this->x != other.x || this->y != other.y;
}

using Vec2f = Vec2<float>;

#endif // !VEC2_H