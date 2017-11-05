#pragma once

#include <Mist_Common/include/UtilityMacros.h>
#include <cstdint>
#include <string>
#include <cmath>

MIST_NAMESPACE

class FixedPoint
{
public:
	constexpr FixedPoint() : m_Number(0) {};

	constexpr FixedPoint(int num)
		: m_Number(num << 16) {}

	FixedPoint(float num);


	friend constexpr FixedPoint operator+(FixedPoint lhs, FixedPoint rhs);
	friend constexpr FixedPoint operator-(FixedPoint lhs, FixedPoint rhs);
	friend constexpr FixedPoint operator*(FixedPoint lhs, FixedPoint rhs);
	friend constexpr FixedPoint operator/(FixedPoint lhs, FixedPoint rhs);

	friend constexpr bool operator<(FixedPoint lhs, FixedPoint rhs);
	friend constexpr bool operator>(FixedPoint lhs, FixedPoint rhs);
	friend constexpr bool operator<=(FixedPoint lhs, FixedPoint rhs);
	friend constexpr bool operator>=(FixedPoint lhs, FixedPoint rhs);
	friend constexpr bool operator==(FixedPoint lhs, FixedPoint rhs);
	friend constexpr bool operator!=(FixedPoint lhs, FixedPoint rhs);

	FixedPoint operator+=(FixedPoint rhs);
	FixedPoint operator-=(FixedPoint rhs);
	FixedPoint operator*=(FixedPoint rhs);
	FixedPoint operator/=(FixedPoint rhs);


	friend std::string ToString(FixedPoint point, int decimalCount = 4);

private:
	uint32_t m_Number;
};


// -Implementation

FixedPoint::FixedPoint(float num)
{
	uint32_t whole = (uint32_t)floor(num) << 16;

	uint32_t fract = 0;
	float remainder = num - floor(num);
	for (int i = 16; i > 0; i--)
	{
		remainder *= 2;
		fract += remainder > 0.0f ? (1 << (i - 1)) : 0;
		remainder = remainder - floor(remainder);
	}

	m_Number = whole + fract;
}


FixedPoint FixedPoint::operator+=(FixedPoint rhs)
{
	*this = *this + rhs;
	return *this;
}

FixedPoint FixedPoint::operator-=(FixedPoint rhs)
{
	*this = *this - rhs;
	return *this;
}

FixedPoint FixedPoint::operator*=(FixedPoint rhs)
{
	*this = *this * rhs;
	return *this;
}

FixedPoint FixedPoint::operator/=(FixedPoint rhs)
{
	*this = *this / rhs;
	return *this;
}

// -Modification operators-

constexpr FixedPoint operator+(FixedPoint lhs, FixedPoint rhs)
{
	FixedPoint p;
	p.m_Number = lhs.m_Number + rhs.m_Number;
	return p;
}

constexpr FixedPoint operator-(FixedPoint lhs, FixedPoint rhs)
{
	FixedPoint p;
	p.m_Number = lhs.m_Number - rhs.m_Number;
	return p;
}

constexpr FixedPoint operator*(FixedPoint lhs, FixedPoint rhs)
{
	FixedPoint p;
	uint32_t whole = rhs.m_Number >> 16;
	uint32_t fract = rhs.m_Number & 0x0000FFFF;
	p.m_Number = lhs.m_Number * whole;

	for (int i = 15; i > 0; i--)
	{
		uint32_t setBit = (rhs.m_Number & (1 << (i - 1))) >> (i - 1);
		p.m_Number += (lhs.m_Number / (2 * (1 << (16 - i)))) * setBit;
	}

	return p;
}

constexpr FixedPoint operator/(FixedPoint lhs, FixedPoint rhs)
{
	FixedPoint p;

	uint64_t left = (uint64_t)lhs.m_Number << 16;
	uint64_t right = (uint64_t)rhs.m_Number;

	p.m_Number = (uint32_t)(left / right);

	return p;
}

// -Comparison operators-

constexpr bool operator<(FixedPoint lhs, FixedPoint rhs)
{
	return lhs.m_Number < rhs.m_Number;
}

constexpr bool operator>(FixedPoint lhs, FixedPoint rhs)
{
	return lhs.m_Number > rhs.m_Number;
}

constexpr bool operator<=(FixedPoint lhs, FixedPoint rhs)
{
	return lhs.m_Number <= rhs.m_Number;
}

constexpr bool operator>=(FixedPoint lhs, FixedPoint rhs)
{
	return lhs.m_Number >= rhs.m_Number;
}

constexpr bool operator==(FixedPoint lhs, FixedPoint rhs)
{
	return lhs.m_Number == rhs.m_Number;
}

constexpr bool operator!=(FixedPoint lhs, FixedPoint rhs)
{
	return lhs.m_Number != rhs.m_Number;
}

// -Printing-

std::string ToString(FixedPoint point, int decimalCount)
{
	uint32_t fraction = (uint32_t)(point.m_Number & 0x0000FFFF);
	uint32_t whole = (uint32_t)(point.m_Number >> 16);

	whole = whole & (~(1 << 16));

	std::string fractionString;
	if (fraction == 0)
	{
		fractionString = "0";
	}

	for (int i = 0; i < decimalCount && fraction > 0; i++)
	{
		fraction *= 10;
		fractionString += '0' + (fraction >> 16);
		fraction &= ((1 << 16) - 1);
	}

	std::string print = std::to_string(whole) + "." + fractionString;

	return std::to_string(whole) + "." + fractionString;
}

MIST_NAMESPACE_END

