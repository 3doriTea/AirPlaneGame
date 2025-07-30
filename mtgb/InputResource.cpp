#include "InputResource.h"
#

void mtgb::InputResource::SetResource()
{
	Input& input = Game::System<Input>();
	input.ChangeKeyDevice(pKeyDevice_);
	input.ChangeMouseDevice(pMouseDevice_);
}
