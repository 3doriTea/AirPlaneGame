#pragma once
#include "ISystem.h"
#include <string>

#pragma comment(lib, "d3d11.lib")  // DirectX11�̃��C�u����
#pragma comment(lib, "d3dcompiler.lib")  // �V�F�[�_�R���p�C���p���C�u����

struct D3D11_INPUT_ELEMENT_DESC;
struct CD3D11_RASTERIZER_DESC;
class  WindowRenderContext;

namespace mtgb
{
	enum struct ShaderType : int8_t;

	class DirectX11Manager : public ISystem
	{
	public:
		DirectX11Manager();
		~DirectX11Manager();

		void Initialize() override;
		void Update() override;

		void InitializeCommonResources();
		void InitializeWindowContext(WindowRenderContext& context, bool isMultiMonitor);

		void ChangeRenderContext(WindowRenderContext& context);

		void Release() override;
	private:
		/// <summary>
		/// �V�F�[�_�o���h��������������
		/// </summary>
		void InitializeShaderBundle();

		/// <summary>
		/// �V�F�[�_�t�@�C�����R���p�C������
		/// </summary>
		/// <param name="_fileName">�t�@�C���p�X</param>
		/// <param name="_type">�V�F�[�_�^�C�v</param>
		/// <param name="_pHLSLLayout">���͏��z��</param>
		/// <param name="_layoutLength">���͏��z��̗v�f��</param>
		/// <param name="_pRasterizerDesc">���X�^���C�U�̐ݒ�</param>
		void CompileShader(
			const std::wstring& _fileName,
			const ShaderType& _type,
			const D3D11_INPUT_ELEMENT_DESC* _pHLSLLayout,
			const unsigned int _layoutLength,
			const CD3D11_RASTERIZER_DESC* _pRasterizerDesc);
	};
}
