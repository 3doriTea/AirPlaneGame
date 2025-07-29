#include "Direct2DResource.h"

using namespace mtgb;

void mtgb::Direct2DResource::SetResource()
{
	Game::System<Direct2D>().ChangeRenderTarget(pD2DBrush_, pRenderTarget_);
}
