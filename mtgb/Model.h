#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include "IModelPack.h"
#include <map>
#include "Handlers.h"

namespace mtgb
{

	class Model : public ISystem
	{
	private:
		/// <summary>
		/// モデルの情報
		/// </summary>
		struct ModelData
		{
			IModelPack* pModelPack;
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
		ModelHandle handleCounter_;  // ハンドラの加算用
		std::map<ModelHandle, ModelData*> modelDatas_;

	};
}
