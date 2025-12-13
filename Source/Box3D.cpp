#include "Box3D.h"

Box3D::Box3D(const Vector3& _position, const Vector3& _scale, std::string _fileName)
	: GameObject(GameObjectBuilder().SetName("Box3D")
	.SetPosition(_position)
	.SetScale(_scale).Build())
	, pTransform_{Component<Transform>()}
	, hModel_{Fbx::Load(_fileName)}
{
}

void Box3D::Update()
{
}

void Box3D::Draw() const
{
	Draw::FBXModel(hModel_, *pTransform_, 0,ShaderType::Box3D);
}
