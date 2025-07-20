#pragma once
#include "ISystem.h"
#include "cmtgb.h"

namespace mtgb
{
	class Fbx;

	class Model : public ISystem
	{
	private:
		/// <summary>
		/// ���f���̏��
		/// </summary>
		struct ModelData
		{
			Fbx* pFbx;
			float currentFrame;
			float animationSpeed;
			int beginFrame, endFrame;
		};

	public:
		Model();
		~Model();

		void Initialize() override;
		void Update() override;

		static ModelHandle Load(const std::string_view& _fileName);

	private:
		ModelHandle handleCounter_;  // �n���h���̉��Z�p
		std::map<ModelHandle, ModelData*> modelDatas_;

	};
}
