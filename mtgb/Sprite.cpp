#include "Sprite.h"
#include "Transform.h"
#include "Debug.h"


mtgb::Sprite::Sprite() :
	texture2D_{}
{
}

mtgb::Sprite::~Sprite()
{
}

void mtgb::Sprite::Load(const std::wstring& _fileName)
{
	texture2D_.Load({ _fileName });
}

void mtgb::Sprite::Draw(
	const RectInt& _draw,
	const float _rotationZ,
	const RectInt& _cut,
	const Color& _color)
{
	using DirectX::XMMatrixScaling;      // 拡縮
	using DirectX::XMMatrixTranspose;    // 行と列を入れ替える
	using DirectX::XMMatrixTranslation;  // 平行移動
	using DirectX::XMMatrixIdentity;     // 単位行列
	using DirectX::XMMatrixRotationZ;    // Z軸の回転行列

	DirectX11Draw::SetBlendMode(BlendMode::Default);  // ブレンドモードデフォルト
	DirectX11Draw::SetIsWriteToDepthBuffer(false);    // 深度バッファへの書き込みなし

	IShader::Draw<ConstantBuffer, Vertex>(
		// シェーダに渡すやつ
		[&, this](ConstantBuffer* _pCB)
		{
			_pCB->g_color = _color.ToVector4Norm();
			_pCB->g_angle = {};
			_pCB->g_matrixCameraRotation = XMMatrixIdentity();  // カメラは無し = UI座標
			_pCB->g_matrixCameraTranslate = XMMatrixIdentity();
			_pCB->g_matrixWorldRotation = XMMatrixRotationZ(_rotationZ);

#pragma region TODO: 計算見直し必要
			// スクリーンサイズを一度だけ取得
			static const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

			// 数学座標と描画座標のy軸差異解消
			RectInt cartesianBox{ _draw };
			cartesianBox.y = SCREEN_SIZE.y - cartesianBox.y;
			cartesianBox.height *= -1;

			const Vector2Int VIEW_BEGIN{ cartesianBox.GetBegin() };
			const Vector2Int VIEW_END{ cartesianBox.GetEnd() };

			// 表示するサイズに合わせる
			Matrix4x4 scalingBox = XMMatrixScaling(
				std::abs(VIEW_END.x - VIEW_BEGIN.x) * 2.0f,
				std::abs(VIEW_END.y - VIEW_BEGIN.y) * 2.0f,
				1.0f);

			// 表示するボックスの位置を移動する
			Matrix4x4 moveBox = XMMatrixTranslation(
				((VIEW_END.x - VIEW_BEGIN.x) / 2.0f + VIEW_BEGIN.x) / (SCREEN_SIZE.x / 2.0f),
				((VIEW_BEGIN.y - VIEW_END.y) / 2.0f + VIEW_END.y) / (SCREEN_SIZE.y / 2.0f),
				0.0f);

			// 画面に合わせる
			Matrix4x4 scalingView = XMMatrixScaling(
				1.0f / (SCREEN_SIZE.x * 2),
				1.0f / (SCREEN_SIZE.y * 2),
				1.0f);

			// オフセット - 画面中心は(0, 0) 左下は(-1, -1)
			Matrix4x4 offsetView
			{
				XMMatrixTranslation(-1.0f, -1.0f, 0.0f)
			};

			// 最終的な行列
			Matrix4x4 world
			{
				scalingBox * scalingView * moveBox * offsetView
			};

			_pCB->g_matrixWorldTranslate = XMMatrixTranspose(world);
#pragma endregion

#pragma region UV計算
			// トリミング計算

			const Vector2Int CUT_BEGIN{ _cut.GetBegin() };
			const Vector2Int CUT_END{ _cut.GetEnd() };

			// トリミング矩形の左上点を並行移動
			Matrix4x4 uvMove = XMMatrixTranslation(
				CUT_BEGIN.x * 1.0f / texture2D_.GetSize().x,
				CUT_BEGIN.y * 1.0f / texture2D_.GetSize().y,
				0.0f);

			// トリミング矩形の拡縮
			Matrix4x4 uvScaling = XMMatrixScaling(
				static_cast<float>(CUT_END.x) / texture2D_.GetSize().x,
				static_cast<float>(CUT_END.y) / texture2D_.GetSize().y,
				1.0f);

			Matrix4x4 uv{ uvScaling * uvMove };

			_pCB->g_matrixTexture = XMMatrixTranspose(uv);
#pragma endregion
		},
		[&, this](ID3D11DeviceContext* _pDC)
		{
			ID3D11SamplerState* pSamplerState{ texture2D_.GetSamplerState() };
			_pDC->PSSetSamplers(0, 1, &pSamplerState);

			ID3D11ShaderResourceView* pSRV{ texture2D_.GetShaderResourceView() };
			_pDC->PSSetShaderResources(0, 1, &pSRV);
		});

#if 0  // テスト用 (GPUに送ったデータを参照する)
	{  // 頂点バッフ
		// 1. ステージングバッファの作成
		ID3D11Buffer* pStagingBuffer = nullptr;
		D3D11_BUFFER_DESC stagingDesc;
		ZeroMemory(&stagingDesc, sizeof(stagingDesc));
		stagingDesc.ByteWidth = sizeof(Vertex) * 4;
		stagingDesc.Usage = D3D11_USAGE_STAGING;
		stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingDesc.StructureByteStride = 0;

		DirectX11Draw::pDevice_->CreateBuffer(&stagingDesc, nullptr, &pStagingBuffer);

		// 2. GPUバッファからコピー
		DirectX11Draw::pContext_->CopyResource(pStagingBuffer, pVertexBuffer_);

		// 3. データマッピング
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(DirectX11Draw::pContext_->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
		{
			Vertex retrievedData[4];
			memcpy(&retrievedData, mappedResource.pData, sizeof(Vertex) * 4);

			DirectX11Draw::pContext_->Unmap(pStagingBuffer, 0);
		}

		// 4. リソース解放
		pStagingBuffer->Release();
	}

	{  // コンスタントバッフ
		// 1. ステージングバッファの作成
		ID3D11Buffer* pStagingBuffer = nullptr;
		D3D11_BUFFER_DESC stagingDesc;
		ZeroMemory(&stagingDesc, sizeof(stagingDesc));
		stagingDesc.ByteWidth = sizeof(ConstantBuffer);
		stagingDesc.Usage = D3D11_USAGE_STAGING;
		stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingDesc.StructureByteStride = 0;

		DirectX11Draw::pDevice_->CreateBuffer(&stagingDesc, nullptr, &pStagingBuffer);

		// 2. GPUバッファからコピー
		DirectX11Draw::pContext_->CopyResource(pStagingBuffer, pConstantBuffer_);

		// 3. データマッピング
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(DirectX11Draw::pContext_->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
		{
			ConstantBuffer retrievedData;
			memcpy(&retrievedData, mappedResource.pData, sizeof(ConstantBuffer));

			DirectX11Draw::pContext_->Unmap(pStagingBuffer, 0);
		}

		// 4. リソース解放
		pStagingBuffer->Release();
	}
#endif
}

void mtgb::Sprite::Draw(const Transform* _pTransform, const Transform* _pCameraTransform, const Vector2Int& _imageSize, const Color& _color)
{
	massert(_pTransform != nullptr
		&& "描画する_pTransformにnullptrが指定された @Sprite::Draw");

	using DirectX::XMMatrixScaling;             // 拡縮
	using DirectX::XMMatrixTranspose;           // 行と列を入れ替える
	using DirectX::XMMatrixTranslation;         // 平行移動
	using DirectX::XMMatrixIdentity;            // 単位行列
	using DirectX::XMMatrixRotationQuaternion;  // 回転行列

	DirectX11Draw::SetBlendMode(BlendMode::Default);  // ブレンドモードデフォルト
	DirectX11Draw::SetIsWriteToDepthBuffer(false);    // 深度バッファへの書き込みなし

	IShader::Draw<ConstantBuffer, Vertex>(
		// シェーダに渡すやつ
		[&, this](ConstantBuffer* _pCB)
		{
			_pCB->g_color = _color.ToVector4Norm();
			_pCB->g_angle = {};

			_pCB->g_matrixCameraTranslate = XMMatrixIdentity();
			if (_pCameraTransform != nullptr)
			{
				// カメラがあるならカメラのワールド行列を生成して入れておく
				_pCameraTransform->GenerateWorldMatrix(&_pCB->g_matrixCameraTranslate);
				_pCB->g_matrixCameraTranslate = XMMatrixTranspose(_pCB->g_matrixCameraTranslate);

				_pCameraTransform->GenerateWorldRotationMatrix(&_pCB->g_matrixCameraRotation);
				_pCB->g_matrixCameraTranslate = XMMatrixTranspose(_pCB->g_matrixCameraRotation);
			}
			Matrix4x4 test = _pCB->g_matrixCameraTranslate;


#pragma region TODO: 計算見直し必要
			// スクリーンサイズを一度だけ取得
			static const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

			// 数学座標と描画座標のy軸差異解消
			/*RectInt cartesianBox{ _draw };
			cartesianBox.y = SCREEN_SIZE.y - cartesianBox.y;
			cartesianBox.height *= -1;*/

			/*const Vector2Int VIEW_BEGIN{ cartesianBox.GetBegin() };
			const Vector2Int VIEW_END{ cartesianBox.GetEnd() };*/

			// 表示するサイズに合わせる
			/*Matrix4x4 scalingBox = XMMatrixScaling(
				std::abs(VIEW_END.x - VIEW_BEGIN.x) * 2.0f,
				std::abs(VIEW_END.y - VIEW_BEGIN.y) * 2.0f,
				1.0f);*/

			// 表示するボックスの位置を移動する
			//DirectX::XMMatrixTranslationFromVector(_pTransform->position_);
			
			// ボックスの座標変換
			Matrix4x4 boxTranslate{}; 
			_pTransform->GenerateParentMatrix(&boxTranslate);

			//// 画像サイズに合わせる
			Matrix4x4 scalingBox = XMMatrixScaling(
				static_cast<float>(_imageSize.x),
				static_cast<float>(_imageSize.y),
				1.0f);

			// オフセット - 画面中心は(0, 0) 左下は(-1, -1)
			/*Matrix4x4 offsetView
			{
				XMMatrixTranslation(-1.0f, -1.0f, 0.0f)
			};*/

			// 画面に合わせる
			Matrix4x4 scalingView = XMMatrixScaling(
				1.0f / (SCREEN_SIZE.x * 1),
				1.0f / (SCREEN_SIZE.y * 1),
				1.0f);

			// 最終的な行列
			Matrix4x4 worldTranslate
			{
				scalingBox * scalingView * boxTranslate
			};
			_pCB->g_matrixWorldTranslate = XMMatrixTranspose(worldTranslate);

			_pTransform->GenerateWorldRotationMatrix(&_pCB->g_matrixWorldRotation);
			_pCB->g_matrixWorldRotation = XMMatrixTranspose(_pCB->g_matrixWorldRotation);
#pragma endregion

#pragma region UV計算
			_pCB->g_matrixTexture = XMMatrixIdentity();
#pragma endregion
		},
		[&, this](ID3D11DeviceContext* _pDC)
		{
			ID3D11SamplerState* pSamplerState{ texture2D_.GetSamplerState() };
			_pDC->PSSetSamplers(0, 1, &pSamplerState);

			ID3D11ShaderResourceView* pSRV{ texture2D_.GetShaderResourceView() };
			_pDC->PSSetShaderResources(0, 1, &pSRV);
		});
}

void mtgb::Sprite::InitializeVertexBuffer(ID3D11Device* _pDevice)
{
	Vertex vertexes[]
	{
		{ Vector3{ -1,  1,  0 }, Vector3{ 0, 0, 0 } },  // 左上
		{ Vector3{  1,  1,  0 }, Vector3{ 1, 0, 0 } },  // 右上
		{ Vector3{ -1, -1,  0 }, Vector3{ 0, 1, 0 } },  // 左下
		{ Vector3{  1, -1,  0 }, Vector3{ 1, 1, 0 } },  // 右下
	};

	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(vertexes),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA
	{
		.pSysMem = vertexes,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		&INITIALIZE_DATA,
		&pVertexBuffer_);

	massert(SUCCEEDED(hResult)  // 頂点バッファの作成に成功
		&& "頂点バッファの作成に失敗 @Sprite::InitializeVertexBuffer");
}

void mtgb::Sprite::InitializeIndexBuffer(ID3D11Device* _pDevice)
{
	static const int INDEXES[]{ 2, 1, 0, 2, 3, 1 };
	//static const int INDEXES[]{ 0, 1, 2, 2, 1, 3 };

	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(INDEXES),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA
	{
		.pSysMem = INDEXES,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		&INITIALIZE_DATA,
		&pIndexBuffer_);

	massert(SUCCEEDED(hResult)  // インデックスバッファの作成に成功
		&& "インデックスバッファの作成に失敗 @Sprite::InitializeIndexBuffer");
}

void mtgb::Sprite::InitializeConstantBuffer(ID3D11Device* _pDevice)
{
	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(ConstantBuffer),
		.Usage = D3D11_USAGE_DYNAMIC,  // MEMO: 途中で書き換えるためdynamic
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		nullptr,  // 初期データなし
		&pConstantBuffer_);

	massert(SUCCEEDED(hResult)
		&& "コンスタントバッファの作成に失敗 @Sprite::InitializeConstantBuffer");
}
