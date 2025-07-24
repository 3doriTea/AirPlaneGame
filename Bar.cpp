#include "Bar.h"
#include "MTAssert.h"


Bar::Bar() :
	currentValue_{ 0.0f },
	maxValue_{ 0.0f },
	minValue_{ 0.0f }
{
}

Bar::~Bar()
{
}

const float Bar::GetRate() const
{
	// ”ÍˆÍ“à‚Å‚Ì‘Š‘Î” / ”ÍˆÍ
	return (currentValue_ - minValue_) / (maxValue_ - minValue_);
}

void Bar::CalculateSafeValue()
{
	// ‚à‚µÅ¬’l‚ÆÅ‘å’l‚ª‹t“]‚µ‚Ä‚¢‚é‚È‚ç”½“]‚·‚é
	if (maxValue_ < minValue_)
	{
		float tmpMaxValue{ maxValue_ };
		maxValue_ = minValue_;
		minValue_ = tmpMaxValue;
	}

	// ‚à‚µŒ»İ‚Ì’l‚ª”ÍˆÍŠO‚È‚ç”ÍˆÍ“à‚É”[‚ß‚é
	if (currentValue_ > maxValue_)
	{
		currentValue_ = maxValue_;
	}
	else if (currentValue_ < minValue_)
	{
		currentValue_ = minValue_;
	}
}
