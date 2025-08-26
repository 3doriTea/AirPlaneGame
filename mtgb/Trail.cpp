#include "Trail.h"
#include "CameraSystem.h"

mtgb::Trail::Trail()
{
}

mtgb::Trail::~Trail()
{
}

void mtgb::Trail::Draw(const yz::TrailEmitter& _trailEmitter)
{
	DirectX11Draw::SetIsWriteToDepthBuffer(true);
	// 描画情報をシェーダに渡す
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	DirectX11Draw::pContext_->IASetVertexBuffers(0, 1, pVertexBuffer_.GetAddressOf(), &stride, &offset);

	// 使用するコンスタントバッファをシェーダに伝える
	DirectX11Draw::pContext_->VSSetConstantBuffers(0, 1, pConstantBuffer_.GetAddressOf());
	DirectX11Draw::pContext_->PSSetConstantBuffers(0, 1, pConstantBuffer_.GetAddressOf());

	// カメラシステムへのアクセス用
	const CameraSystem& CAMERA{ Game::System<CameraSystem>() };

    {
        // viewDir from camera to target
        /*yz::F3 viewDir = yz::f3(gapp.camTarget.x - gapp.camPos.x,
            gapp.camTarget.y - gapp.camPos.y,
            gapp.camTarget.z - gapp.camPos.z);
        std::vector<yz::Vtx> ribbon;
        _trailEmitter.BuildRibbon(viewDir, ribbon);
        if (ribbon.size() < 4) return;

        D3D11_MAPPED_SUBRESOURCE ms{};
        if (SUCCEEDED(gdx.ctx->Map(gVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
            size_t count = ribbon.size();
            std::memcpy(ms.pData, ribbon.data(), sizeof(Vtx) * count);
            gdx.ctx->Unmap(gVB, 0);

            UINT stride = sizeof(Vtx), offset = 0;
            gdx.ctx->IASetVertexBuffers(0, 1, &gVB, &stride, &offset);
            gdx.ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            gdx.ctx->IASetInputLayout(gIL);
            gdx.ctx->Draw((UINT)count, 0);
        }*/
    }
}

void mtgb::Trail::InitializeVertexBuffer(ID3D11Device* _pDevice)
{
}

void mtgb::Trail::InitializeIndexBuffer(ID3D11Device* _pDevice)
{
}

void mtgb::Trail::InitializeConstantBuffer(ID3D11Device* _pDevice)
{
}
