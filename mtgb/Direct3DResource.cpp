#include "Direct3DResource.h"

using namespace mtgb;

void mtgb::Direct3DResource::SetResource()
{
	DirectX11Manager& dx11Manager{ Game::System<DirectX11Manager>() };
	dx11Manager.ChangeViewport(viewPort_);
	dx11Manager.ChangeRenderTargets(pRenderTargetView_, pDepthStencilView_);
}
