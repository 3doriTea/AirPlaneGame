#include "OBJ.h"
#include "MTAssert.h"
#include "DirectX11Draw.h"

#include "ReleaseUtility.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "Vector2Int.h"
#include "Screen.h"

#include <stdio.h>
#include <windows.h>
#include <d3d11.h>
#include <d3dCompiler.h>


#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dCompiler.lib")
#pragma comment(lib,"winmm.lib")

ID3D11InputLayout* mtgb::OBJ::pInputLayout_{ nullptr };
ID3D11VertexShader* mtgb::OBJ::pVertexShader_{ nullptr };
ID3D11PixelShader* mtgb::OBJ::pPixelShader_{ nullptr };
ID3D11Buffer* mtgb::OBJ::pConstantBuffer_{ nullptr };


void mtgb::OBJ::Initialize()
{
	HRESULT hResult;

	//hlslファイル読み込み ブロブ作成　ブロブとはシェーダーの塊みたいなもの。XXシェーダーとして特徴を持たない。後で各種シェーダーに成り得る。
	ID3DBlob* pCompiledShader = NULL;
	hResult = D3DCompileFromFile(L"Shader/Geometry.hlsl", NULL, NULL, "VS", "vs_5_0", 0, 0, &pCompiledShader, NULL);

	massert(SUCCEEDED(hResult)
		&& "頂点シェーダのコンパイルに失敗 @OBJ::Initialize");

	hResult = DirectX11Draw::pDevice_->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &pVertexShader_);
	massert(SUCCEEDED(hResult)
		&& "頂点シェーダの作成に失敗 @OBJ::Initialize");

	//頂点インプットレイアウトを定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	int numElements = sizeof(layout) / sizeof(layout[0]);

	//頂点インプットレイアウトを作成
	hResult = DirectX11Draw::pDevice_->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &pInputLayout_);
	massert(SUCCEEDED(hResult)
		&& "頂点インプットレイアウトの作成に失敗 @OBJ::Initialize");

	//ピクセルシェーダ作成
	hResult = D3DCompileFromFile(L"Shader/Geometry.hlsl", NULL, NULL, "PS", "ps_5_0", 0, 0, &pCompiledShader, NULL);
	massert(SUCCEEDED(hResult)
		&& "ピクセルシェーダの作成に失敗 @OBJ::Initialize");

	hResult = DirectX11Draw::pDevice_->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &pPixelShader_);
	massert(SUCCEEDED(hResult)
		&& "ピクセルシェーダの作成に失敗 @OBJ::Initialize");

	SAFE_RELEASE(pCompiledShader);

	//コンスタントバッファー作成
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SimpleConstantBuffer);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	hResult = DirectX11Draw::pDevice_->CreateBuffer(&cb, NULL, &pConstantBuffer_);
		massert(SUCCEEDED(hResult)
			&& "コンスタントバッファの作成に失敗 @OBJ::Initialize");
}

int mtgb::OBJ::Load(const std::string& fileName)
{
	ModelData* pData = new ModelData;

	OBJ& pInstance{ Game::System<OBJ>() };

	//開いたファイル一覧から同じファイル名のものが無いか探す
	bool isExist = false;
	for (int i = 0; i < pInstance.datas_.size(); i++)
	{
		//既に開いている場合
		if (pInstance.datas_[i] != nullptr && pInstance.datas_[i]->fileName == fileName)
		{
			pData->mesh = pInstance.datas_[i]->mesh;
			isExist = true;
			break;
		}
	}

	//新たにファイルを開く
	if (isExist == false)
	{
		pData->mesh = new SimpleMesh;
		pInstance.InitMesh(fileName, pData->mesh);
	}

	//使ってない番号が無いか探す
	for (int i = 0; i < pInstance.datas_.size(); i++)
	{
		if (pInstance.datas_[i] == nullptr)
		{
			pInstance.datas_[i] = pData;
			return i;
		}
	}

	//新たに追加
	pInstance.datas_.push_back(pData);
	return (int)pInstance.datas_.size() - 1;
}

void mtgb::OBJ::Update()
{
}

void mtgb::OBJ::Draw(int hModel, const Transform* transform)
{
	//DirectX::XMMATRIX mWorld;
	Matrix4x4* mWorld = new Matrix4x4();
	transform->GenerateWorldMatrix(mWorld);

	DirectX::XMMATRIX mView;
	// ビュートランスフォーム（視点座標変換）
	DirectX::XMVECTOR vEyePt = DirectX::XMVectorSet(0.0f, 0.0f, -10.0f,0.0f); //カメラ（視点）位置
	DirectX::XMVECTOR vLookatPt = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f,0.0f);//注視位置
	DirectX::XMVECTOR vUpVec = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f,0.0f);//上方位置
	mView = DirectX::XMMatrixLookAtLH(vEyePt, vLookatPt, vUpVec);

	DirectX::XMMATRIX mProj;
	static const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	mProj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI / 4, (FLOAT)SCREEN_SIZE.x/(FLOAT)SCREEN_SIZE.y, 0.1f,100.0f);

	ID3D11DeviceContext* tmpContext = DirectX11Draw::pContext_;

	tmpContext->VSSetShader(pVertexShader_, NULL, 0);
	tmpContext->PSSetShader(pPixelShader_, NULL, 0);

	//DirectX11Draw::Begin();


	//シェーダーのコンスタントバッファーに各種データを渡す	
	D3D11_MAPPED_SUBRESOURCE pData;
	SimpleConstantBuffer cb;


	HRESULT hResult = DirectX11Draw::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	massert(SUCCEEDED(hResult)
		&& "Mapに失敗 @OBJ::Draw");
	if (SUCCEEDED(hResult))
	{
		DirectX::XMMATRIX mWVP = (*mWorld) * mView * mProj;
		//cb.mWVP = DirectX::XMMatrixTranspose(*mWorld);
		cb.mWVP = DirectX::XMMatrixTranspose(mWVP);

		memcpy_s(pData.pData, pData.RowPitch, (void*)&cb, sizeof(cb));

		DirectX11Draw::pContext_->Unmap(pConstantBuffer_, 0);

		int slot = 0;
		tmpContext->VSSetConstantBuffers(slot, 1, &pConstantBuffer_);
		tmpContext->PSSetConstantBuffers(slot, 1, &pConstantBuffer_);

		tmpContext->IASetInputLayout(pInputLayout_);

		tmpContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//頂点バッファセット
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;
		tmpContext->IASetVertexBuffers(slot, 1, &(datas_[hModel]->mesh->pVertexBuffer), &stride, &offset);

		//インデックスバッファセット
		tmpContext->IASetIndexBuffer(datas_[hModel]->mesh->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		//プリミティブをレンダリング
		tmpContext->DrawIndexed(datas_[hModel]->mesh->numFace * 3, 0, 0);

		
		//DirectX11Draw::End();
	}

	delete mWorld;
}



void mtgb::OBJ::InitMesh(const std::string& fileName, SimpleMesh* mesh)
{
	float x, y, z;
	int v1 = 0, v2 = 0, v3 = 0;
	char key[190] = { 0 };
	
	//ファイルを開いて内容を読み込む
	FILE* fp = NULL;
	fopen_s(&fp, fileName.c_str(), "rt");
	
	// ファイルオープンの確認
	/*if (fp == NULL) {
		massert(false && "OBJファイルのオープンに失敗 @OBJ::InitMesh");
		return;
	}*/

	// 初期化
	mesh->numVert = 0;
	mesh->numFace = 0;

	//まずは頂点数、ポリゴン数を調べる
	while (!feof(fp))
	{
		//キーワード読み込み
		fscanf_s(fp, "%s ", key, (unsigned int)sizeof(key));
		//頂点
		if (strcmp(key, "v") == 0)
		{
			mesh->numVert++;
		}
		//フェイス（ポリゴン）
		if (strcmp(key, "f") == 0)
		{
			mesh->numFace++;
		}
	}

	//一時的なメモリ確保（頂点バッファとインデックスバッファ）
	SimpleVertex* pVertexBuffer = new SimpleVertex[mesh->numVert];
	int* pIndexBuffer = new int[mesh->numFace * 3];

	//本読み込み	
	fseek(fp, SEEK_SET, 0);
	int vertCount = 0;//読み込みカウンター
	int faceCount = 0;//読み込みカウンター

	while (!feof(fp))
	{
		//キーワード読み込み
		ZeroMemory(key, sizeof(key));
		fscanf_s(fp, "%s ", key, (unsigned int)sizeof(key));
		//頂点 読み込み
		if (strcmp(key, "v") == 0)
		{
			fscanf_s(fp, "%f %f %f", &x, &y, &z);
			//OBJは右手座標系なのでxあるいはｚを反転
			pVertexBuffer[vertCount].pos = DirectX::XMVectorSet(-x, y, z, 1);
			vertCount++;
		}
		//フェイス（ポリゴン） 読み込み
		if (strcmp(key, "f") == 0)
		{
			fscanf_s(fp, "%d %d %d", &v1, &v2, &v3);
			pIndexBuffer[faceCount * 3] = v1 - 1;
			pIndexBuffer[faceCount * 3 + 1] = v2 - 1;
			pIndexBuffer[faceCount * 3 + 2] = v3 - 1;
			faceCount++;
		}
	}

	fclose(fp);

	//頂点バッファ作成
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * mesh->numVert;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pVertexBuffer;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	HRESULT hResult = DirectX11Draw::pDevice_->CreateBuffer(&bd, &initData, &mesh->pVertexBuffer);
	massert(SUCCEEDED(hResult)
		&& "頂点バッファの作成に失敗しました @OBJ::InitStaticMesh");

	//インデックスバッファ作成
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * mesh->numFace * 3;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = pIndexBuffer;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	hResult = DirectX11Draw::pDevice_->CreateBuffer(&bd, &initData, &mesh->pIndexBuffer);
	massert(SUCCEEDED(hResult)
	&& "インデックスバッファの作成に失敗しました @OBJ::InitStaticMesh");

	delete[] pVertexBuffer;
	delete[] pIndexBuffer;
}
