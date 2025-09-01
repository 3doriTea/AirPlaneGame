#include "SpeechQueue.h"

SpeechQueue::SpeechQueue(SpeechLines&& _lines) :
	lines_{ std::move(_lines) },
	current_{ -1 }  // 0s–Ú‚Í‰‚ß‚Ä“Ç‚İã‚°‚é‚½‚ß -1
{
}

SpeechQueue::~SpeechQueue()
{
}

const SPEECH_ELEMENT& SpeechQueue::GetNext()
{
	current_++;

	return lines_[current_];
}
