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
    Vector3 cameraDir{ CAMERA.GetTransform().Forward() };
    {
        // viewDir from camera to target
        yz::F3 viewDir = yz::f3(cameraDir.x, cameraDir.y, cameraDir.z);
        std::vector<yz::Vtx> ribbon;
        _trailEmitter.BuildRibbon(viewDir, ribbon);
        if (ribbon.size() < 4) return;

        D3D11_MAPPED_SUBRESOURCE ms{};
        if (SUCCEEDED(DirectX11Draw::pContext_->Map(pVertexBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
        {
            size_t count = ribbon.size();
            std::memcpy(ms.pData, ribbon.data(), sizeof(yz::Vtx) * count);
            DirectX11Draw::pContext_->Unmap(pVertexBuffer_.Get(), 0);

            UINT stride = sizeof(yz::Vtx), offset = 0;
            DirectX11Draw::pContext_->IASetVertexBuffers(0, 1, pVertexBuffer_.GetAddressOf(), &stride, &offset);
            DirectX11Draw::pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            //DirectX11Draw::pContext_->IASetInputLayout(gIL);
            DirectX11Draw::pContext_->Draw((UINT)count, 0);
        }
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
