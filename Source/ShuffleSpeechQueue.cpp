#include "ShuffleSpeechQueue.h"



ShuffleSpeechQueue::ShuffleSpeechQueue(SpeechLines&& _lines, std::mt19937 _randomNumberGenerator_)
	: lines_{ std::move(_lines) }
	, randomNumberGenerator_(std::move(_randomNumberGenerator_))
	, autoShuffle_{true}
{
	order_.resize(lines_.size());
	// 0‚©‚çŽn‚Ü‚é®”—ñ‚ð¶¬
	std::iota(order_.begin(), order_.end(), 0);
	Shuffle();
}

ShuffleSpeechQueue::ShuffleSpeechQueue(const SpeechLines& _lines, std::mt19937 _randomNumberGenerator_)
	: ShuffleSpeechQueue(SpeechLines(_lines),std::move(_randomNumberGenerator_))
{
}

const bool ShuffleSpeechQueue::TryGetNext(SPEECH_ELEMENT& _speechElement)
{
	if (index_ >= order_.size())
	{
		if (autoShuffle_)
		{
			Shuffle();
		}
		else
		{
			return false;
		}
	}

	_speechElement = lines_[order_[index_++]];

	return true;
}

void ShuffleSpeechQueue::Shuffle()
{
	if (order_.empty()) return;

	std::shuffle(order_.begin(), order_.end(), randomNumberGenerator_);
	index_ = 0;
}
