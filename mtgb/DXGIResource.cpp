#include "DXGIResource.h"

using namespace mtgb;

void DXGIResource::SetResource()
{
	Game::System<DirectX11Manager>().ChangeSwapChain(pSwapChain1_);
}