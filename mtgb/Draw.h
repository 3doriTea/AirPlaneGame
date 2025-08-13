#pragma once
#include "Vector2Int.h"
#include "RectInt.h"
#include "Color.h"
#include "ISystem.h"
#include "ShaderType.h"

namespace mtgb
{
	class Figure;
	class Transform;
	class FbxModel;
	class Ground;
	//enum struct ShaderType : int8_t;
	
	/// <summary>
	/// �`�悷��n
	/// </summary>
	class Draw final : public ISystem
	{
	public:
		enum struct Pivot
		{
			TopLeft,
			Center,
			BottomMiddle,
		};

	public:
		static void SetShaderOnce(const ShaderType _type) { Draw::onceShaderType_ = _type; }
		/// <summary>
		/// �V�F�[�_���Z�b�g����Ă��邩�`�F�b�N����
		/// </summary>
		/// <param name="_default">�Z�b�g����Ă��Ȃ��ꍇ�̃f�t�H���g�V�F�[�_</param>
		static void CheckSetShader(const ShaderType _default);

		static void Box(
			const Vector2Int& _begin,
			const Vector2Int& _end,
			const Color& _color);

		static void Box(
			const RectInt& _rect,
			const Color& _color);

		/*static void Image(
			const Vector2Int& _position,
			const ImageHandle _hImage);

		static void Image(
			const RectInt& _draw,
			const ImageHandle _hImage);*/

		static void Image(
			const ImageHandle _hImage,
			const RectInt& _draw,
			const RectInt& _cut,
			const float _rotationZ);

		static void Image(
			const ImageHandle _hImage,
			const Transform* _pTransform);

		static void Model(
			const ModelHandle _hModel,
			const Transform* _pTransform);

		static void OBJModel(
			const OBJModelHandle _hOBJModel,
			const Transform* _pTransform);

		static void FBXModel(
			const FBXModelHandle _hFBXModel,
			const Transform& _pTransform,
			const int _frame);

		//static void TextBox();
		static void Text(
			const TextHandle _hText, 
			const Vector2Int& origin);

		static void ImmediateText(
			const std::string& text,
			float x,
			float y
		);

		static void ImmediateText(
			const std::string& text,
			float x,
			float y,
			int size
		);
		
		static void GroundPlane();


	public:
		Draw();
		~Draw();
		void Initialize() override;
		void Update() override;

	private:
		FbxModel* pFbxModel_;
		Figure* pFigure_;
		Ground* pGround_;
		static ShaderType onceShaderType_;
	};
}
