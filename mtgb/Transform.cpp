#include "Transform.h"

//mtgb::Transform::Transform()
//{
//}

mtgb::Transform::~Transform()
{
}

void mtgb::Transform::GenerateWorldMatrix(Matrix4x4* _pMatrix) const
{
	*_pMatrix *= matrixScale_;
	*_pMatrix *= matrixRotate_;
	*_pMatrix *= matrixTranslate_;
	if (parentId_ != INVALD_ENTITY)
	{
		GetParent()->GenerateWorldMatrix(_pMatrix);
	}
}

void mtgb::Transform::GenerateWorldRotationMatrix(Matrix4x4* _pMatrix) const
{
	*_pMatrix *= matrixRotate_;
	if (parentId_ != INVALD_ENTITY)
	{
		GetParent()->GenerateWorldMatrix(_pMatrix);
	}
}

void mtgb::Transform::GenerateParentMatrix(Matrix4x4* _pMatrix) const
{
	*_pMatrix *= matrixScale_;
	*_pMatrix *= matrixRotate_;
	*_pMatrix *= matrixTranslate_;
}

void mtgb::Transform::GenerateParentRotationMatrix(Matrix4x4* _pMatrix) const
{
	*_pMatrix *= matrixRotate_;
}

mtgb::Transform* mtgb::Transform::GetParent() const
{
	// eId‚ª–³Œø’l‚È‚ç nullptr ‚ğ•Ô‚·
	if (parentId_ == INVALD_ENTITY)
	{
		return nullptr;
	}
	else
	{
		return &Transform::Get(parentId_);
	}
}
