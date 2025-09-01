#include "SpeechQueue.h"

SpeechQueue::SpeechQueue(SpeechLines&& _lines) :
	lines_{ std::move(_lines) },
	current_{ -1 }  // 0s–Ú‚Í‰‚ß‚Ä“Ç‚İã‚°‚é‚½‚ß -1
{
}

SpeechQueue::~SpeechQueue()
{
}

const bool SpeechQueue::TryGetNext(SPEECH_ELEMENT& _speechElement)
{
	if (current_ >= static_cast<int>(lines_.size()) - 1)
	{
		return false;
	}

	current_++;
	_speechElement = lines_[current_];
	
	return true;
}
