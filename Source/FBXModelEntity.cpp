#include "FBXModelEntity.h"

FBXModelEntity::FBXModelEntity(const Vector3& _position, const Vector3& _scale, std::string _fileName)
	: GameObject(GameObjectBuilder().SetName("FBXModelEntity")
	.SetPosition(_position)
	.SetScale(_scale).Build())
	, pTransform_{Component<Transform>()}
	, hModel_{Fbx::Load(_fileName)}
{
}

void FBXModelEntity::Update()
{
}

void FBXModelEntity::Draw() const
{
	// TransformのスケールによってUV座標をスケーリングするシェーダーを使用
	Draw::FBXModel(hModel_, *pTransform_, 0,ShaderType::Box3D);
}
