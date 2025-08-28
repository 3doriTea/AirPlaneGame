#include "Trail.h"
#include "CameraSystem.h"
#include "Debug.h"

mtgb::Trail::Trail()
{
}

mtgb::Trail::~Trail()
{
}

void mtgb::Trail::Draw(const yz::TrailEmitter& _trailEmitter)
{
	DirectX11Draw::SetIsWriteToDepthBuffer(true);
	DirectX11Draw::SetBlendMode(BlendMode::Default);
	DirectX11Draw::SetShader(ShaderType::Trail);

	// 描画情報をシェーダに渡す
	UINT stride{ sizeof(yz::Vtx) };
	UINT offset{ 0 };
	DirectX11Draw::pContext_->IASetVertexBuffers(0, 1, pVertexBuffer_.GetAddressOf(), &stride, &offset);

	// 使用するコンスタントバッファをシェーダに伝える

	DirectX11Draw::pContext_->VSSetConstantBuffers(0, 1, pConstantBuffer_.GetAddressOf());
	DirectX11Draw::pContext_->PSSetConstantBuffers(0, 1, pConstantBuffer_.GetAddressOf());

	// カメラシステムへのアクセス用
	const CameraSystem& CAMERA{ Game::System<CameraSystem>() };
	Vector3 cameraDir{ CAMERA.GetTransform().Forward() };

	Matrix4x4 matWorld{};

	//DirectX11Draw::pContext_->UpdateSubresource(pConstantBuffer_.Get(), 0, nullptr, &matWorld, 0, 0);
	//DirectX11Draw::pContext_->VSSetConstantBuffers(0, 1, pConstantBuffer_.GetAddressOf());

	{
		// viewDir from camera to target
		yz::F3 viewDir = yz::f3(cameraDir.x, cameraDir.y, cameraDir.z);
		std::vector<yz::Vtx> ribbon;
		_trailEmitter.BuildRibbon(viewDir, ribbon);
		if (ribbon.size() < 4) return;

		HRESULT hResult{};

		D3D11_MAPPED_SUBRESOURCE mappedSubresource{};

		size_t indexCount{ ribbon.size() };

		hResult = DirectX11Draw::pContext_->Map(
			pVertexBuffer_.Get(),
			NULL,
			D3D11_MAP_WRITE_DISCARD,
			NULL,
			&mappedSubresource);

		massert(SUCCEEDED(hResult)  // GPUデータアクセスせき止めに成功
			&& "GPUデータアクセスせき止めに失敗");

		// データ書き込み
		memcpy_s(
			mappedSubresource.pData,
			mappedSubresource.RowPitch,
			ribbon.data(),
			sizeof(yz::Vtx) * indexCount);

		LOGF("indexCount_%d\n");

		DirectX11Draw::pContext_->Unmap(pVertexBuffer_.Get(), 0);

		Matrix4x4 mView{};  // ビュー行列
		CAMERA.GetViewMatrix(&mView);

		Matrix4x4 mProj{};  // プロジェクション行列
		CAMERA.GetProjMatrix(&mProj);


		ConstantBuffer constantBuffer{};
		constantBuffer.wvp_ = XMMatrixTranspose(mView * mProj);
		mappedSubresource = {};

		hResult = DirectX11Draw::pContext_->Map(
			pConstantBuffer_.Get(),
			NULL,
			D3D11_MAP_WRITE_DISCARD,
			NULL,
			&mappedSubresource);

		massert(SUCCEEDED(hResult)  // GPuデータアクセスせき止めに成功
			&& "GPUデータアクセスせき止めに失敗");

		// データ書き込み
		memcpy_s(
			mappedSubresource.pData,
			mappedSubresource.RowPitch,
			static_cast<void*>(&constantBuffer),
			sizeof(ConstantBuffer));

		DirectX11Draw::pContext_->Unmap(pConstantBuffer_.Get(), 0);

		DirectX11Draw::pContext_->Draw(static_cast<UINT>(indexCount), 0);
		//DirectX11Draw::pContext_->DrawIndexed(static_cast<UINT>(indexCount), 0, 0);

		//if (SUCCEEDED(DirectX11Draw::pContext_->Map(pVertexBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
		{
			//size_t count = ribbon.size();
			//std::memcpy(ms.pData, ribbon.data(), sizeof(yz::Vtx) * count);
			//DirectX11Draw::pContext_->Unmap(pVertexBuffer_.Get(), 0);

			/*UINT stride = sizeof(yz::Vtx), offset = 0;
			DirectX11Draw::pContext_->IASetVertexBuffers(0, 1, pVertexBuffer_.GetAddressOf(), &stride, &offset);
			DirectX11Draw::pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			*///DirectX11Draw::pContext_->IASetInputLayout(gIL);
		}
	}
}

void mtgb::Trail::InitializeVertexBuffer(ID3D11Device* _pDevice)
{
	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = static_cast<UINT>(sizeof(yz::Vtx) * 262144),
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		nullptr,
		pVertexBuffer_.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // 頂点バッファの作成に成功
		&& "頂点バッファの作成に失敗 @Trail::InitializeVertexBuffer");
}

void mtgb::Trail::InitializeIndexBuffer(ID3D11Device* _pDevice)
{
	
}

void mtgb::Trail::InitializeConstantBuffer(ID3D11Device* _pDevice)
{
	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(ConstantBuffer),//static_cast<UINT>(sizeof(Vtx) * 262144),
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		nullptr,
		pConstantBuffer_.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // 定数バッファの作成に成功
		&& "定数バッファの作成に失敗 @Trail::InitializeConstantBuffer");
}
