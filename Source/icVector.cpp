#include "icVector.h"

icVector::icVector()
{
	this->x = 0.0f;
	this->y = 0.0f;
}

icVector::icVector(float x, float y)
{
	this->x = x;
	this->y = y;
}

/*virtual*/ icVector::~icVector()
{
}

icVector icVector::Lerp(const icVector& vecA, const icVector& vecB, float lerpValue)
{
	return vecA + (vecB - vecA) * lerpValue;
}

void icVector::operator+=(const icVector& vec)
{
	this->x += vec.x;
	this->y += vec.y;
}

void icVector::operator-=(const icVector& vec)
{
	this->x -= vec.x;
	this->y -= vec.y;
}

void icVector::operator*=(float scalar)
{
	this->x *= scalar;
	this->y *= scalar;
}

icVector operator+(const icVector& vecA, const icVector& vecB)
{
	return icVector(vecA.x + vecB.x, vecA.y + vecB.y);
}

icVector operator-(const icVector& vecA, const icVector& vecB)
{
	return icVector(vecA.x - vecB.x, vecA.y - vecB.y);
}

icVector operator*(const icVector& vec, float scalar)
{
	return icVector(vec.x * scalar, vec.y * scalar);
}