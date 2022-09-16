#pragma once

#define MIN(valueA, valueB)						((valueA) < (valueB) ? (valueA) : (valueB))
#define MAX(valueA, valueB)						((valueA) > (valueB) ? (valueA) : (valueB))
#define CLAMP(value, minValue, maxValue)		MAX(MIN(value, maxValue), minValue)

class icVector
{
public:
	icVector();
	icVector(float x, float y);
	virtual ~icVector();

	float Length() const;
	float Dot(const icVector& vec) const;
	float Cross(const icVector& vec) const;

	icVector Lerp(const icVector& vecA, const icVector& vecB, float lerpValue);

	void operator+=(const icVector& vec);
	void operator-=(const icVector& vec);
	void operator*=(float scalar);

	float x, y;
};

icVector operator+(const icVector& vecA, const icVector& vecB);
icVector operator-(const icVector& vecA, const icVector& vecB);
icVector operator*(const icVector& vec, float scalar);