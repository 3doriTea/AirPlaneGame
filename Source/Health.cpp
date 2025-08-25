#include "Health.h"

Health::Health(const int _maxValue, const int _initValue) :
	value_{ _initValue },
	max_{ _maxValue }
{
}

void Health::SetMax(const int _max)
{
	max_ = _max;
	if (value_ > max_)
	{
		value_ = max_;
	}
}

void Health::Damage(const int _damage)
{
	value_ -= _damage;
	if (value_ < 0)
	{
		value_ = 0;
	}
}

void Health::Heal(const int _heal)
{
	value_ += _heal;
	if (value_ > max_)
	{
		value_ = max_;
	}
}

int Health::GetValue() const
{
	return value_;
}

bool Health::IsDead() const
{
	return value_ <= 0;
}
