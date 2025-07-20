#pragma once
#include "ISystem.h"
#include <DirectXMath.h>

#include <string>
#include <vector>



struct ID3D11Buffer;
struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;

namespace mtgb
{
    class Transform;
    struct SimpleVertex
    {
        DirectX::XMVECTOR pos;
    };

    struct SimpleContantBuffer
    {
        DirectX::XMMATRIX mWVP;
    };

    struct SimpleMesh
    {
        int numVert;
        int numFace;
        ID3D11Buffer* pVertexBuffer;
        ID3D11Buffer* pIndexBuffer;
        SimpleMesh() :numVert{ 0 }, numFace{ 0 }, pVertexBuffer{ nullptr }, pIndexBuffer{nullptr} {}
    };

    struct ModelData
    {
        //ファイル名
        std::string fileName;

        SimpleMesh* mesh;
    };

    class OBJ : public ISystem
    {
    public:
        
        void Initialize() override;
        void Update() override;

        static int Load(const std::string& fileName);
        void Draw(int hModel, const Transform * transform);
    private:
        std::vector<ModelData*> datas_;
        void InitMesh(const std::string& fileName, SimpleMesh* mesh);

        //↓モデルの種類ごと(モデルの構造が全て同一ならアプリにひとつ）
        static ID3D11InputLayout* pInputLayout_;
        static ID3D11VertexShader* pVertexShader_;
        static ID3D11PixelShader* pPixelShader_;
        static ID3D11Buffer* pConstantBuffer_;
        //↓モデルごと	
        //ID3D11Buffer* pVertexBuffer_;
        //SimpleMesh mesh_;
    };
}

