#pragma once

#include <Mist_Common/include/UtilityMacros.h>
#include <cstdint>
#include <string>
#include <cmath>

MIST_NAMESPACE

class FixedPoint
{
public:
	constexpr FixedPoint() : m_Number(0), m_SignBit(0) {};

	FixedPoint(int num)
		: m_Number(abs(num) << 16), m_SignBit(num < 0) {}

	FixedPoint(float num);


	friend constexpr FixedPoint operator+(FixedPoint lhs, FixedPoint rhs);
	friend constexpr FixedPoint operator-(FixedPoint lhs, FixedPoint rhs);
	friend constexpr FixedPoint operator*(FixedPoint lhs, FixedPoint rhs);
	friend constexpr FixedPoint operator/(FixedPoint lhs, FixedPoint rhs);

	friend constexpr FixedPoint Abs(FixedPoint p);
	friend constexpr bool Signed(FixedPoint p);

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

	uint32_t m_Number : 31;
	uint32_t m_SignBit : 1;
};


// -Implementation

FixedPoint::FixedPoint(float num)
{
	m_SignBit = num < 0.0f;

	float absolute = abs(num);
	uint32_t whole = (uint32_t)floor(absolute) << 16;

	uint32_t fract = 0;
	float remainder = absolute - floor(absolute);
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

	if ((rhs.m_SignBit != lhs.m_SignBit) && rhs.m_Number > lhs.m_Number)
	{
		uint32_t leftover = rhs.m_Number - lhs.m_Number;
		p.m_Number = leftover;
		p.m_SignBit = ~lhs.m_SignBit;
	}
	else
	{
		p.m_Number = lhs.m_SignBit ^ rhs.m_SignBit ? lhs.m_Number - rhs.m_Number : lhs.m_Number + rhs.m_Number;
		p.m_SignBit = lhs.m_Number >= rhs.m_Number ? lhs.m_SignBit : rhs.m_SignBit;
	}

	return p;
}

constexpr FixedPoint operator-(FixedPoint lhs, FixedPoint rhs)
{
	FixedPoint p;

	if ((rhs.m_SignBit == lhs.m_SignBit) && rhs.m_Number > lhs.m_Number)
	{
		uint32_t leftover = rhs.m_Number - lhs.m_Number;
		p.m_Number = leftover;
		p.m_SignBit = ~lhs.m_SignBit;
	}
	else
	{
		p.m_Number = lhs.m_SignBit ^ rhs.m_SignBit ? lhs.m_Number + rhs.m_Number : lhs.m_Number - rhs.m_Number;
		p.m_SignBit = lhs.m_Number >= rhs.m_Number ? lhs.m_SignBit : ~rhs.m_SignBit;
	}

	return p;
}

constexpr FixedPoint operator*(FixedPoint lhs, FixedPoint rhs)
{
	FixedPoint p;

	bool lhsSigned = Signed(lhs);
	bool rhsSigned = Signed(rhs);

	uint32_t whole = rhs.m_Number >> 16;
	uint32_t fract = rhs.m_Number & 0x0000FFFF;
	p.m_Number = lhs.m_Number * whole;

	for (int i = 16; i > 0; i--)
	{
		uint32_t setBit = (rhs.m_Number & (1 << (i - 1))) >> (i - 1);
		p.m_Number += (lhs.m_Number / (2 * (1 << (16 - i)))) * setBit;
	}

	p.m_SignBit = lhsSigned ^ rhsSigned;

	return p;
}

constexpr FixedPoint operator/(FixedPoint lhs, FixedPoint rhs)
{
	FixedPoint p;

	bool lhsSigned = Signed(lhs);
	bool rhsSigned = Signed(rhs);

	uint64_t left = (uint64_t)lhs.m_Number << 16;
	uint64_t right = (uint64_t)rhs.m_Number;

	p.m_Number = (uint32_t)(left / right);

	p.m_SignBit = lhsSigned ^ rhsSigned;

	return p;
}

constexpr FixedPoint Abs(FixedPoint p)
{
	p.m_SignBit = 0;
	return p;
}

constexpr bool Signed(FixedPoint p)
{
	return p.m_SignBit;
}

// -Comparison operators-

constexpr bool operator<(FixedPoint lhs, FixedPoint rhs)
{
	if (Signed(lhs) && !Signed(rhs)) return true;
	if (Signed(lhs) && Signed(rhs)) return lhs.m_Number > rhs.m_Number;
	return lhs.m_Number < rhs.m_Number;
}

constexpr bool operator>(FixedPoint lhs, FixedPoint rhs)
{
	if (!Signed(lhs) && Signed(rhs)) return true;
	if (Signed(lhs) && Signed(rhs)) return lhs.m_Number < rhs.m_Number;
	return lhs.m_Number > rhs.m_Number;
}

constexpr bool operator<=(FixedPoint lhs, FixedPoint rhs)
{
	return !(lhs > rhs);
}

constexpr bool operator>=(FixedPoint lhs, FixedPoint rhs)
{
	return !(lhs < rhs);
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

	std::string print;

	if (Signed(point))
	{
		print = "-";
	}

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

	print += std::to_string(whole) + "." + fractionString;

	return print;
}

MIST_NAMESPACE_END

