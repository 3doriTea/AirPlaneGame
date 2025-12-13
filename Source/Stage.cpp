#include "Stage.h"
#include "FBXModelEntity.h"

#include <mtgb.h>

using namespace nlohmann;
void GenerateStageFromJSON(const nlohmann::json& _json)
{
	// 現在のシーンを取得
	GameScene* currentScene = Game::System<SceneSystem>().GetActiveScene();

	// 配列で扱う想定
	if (_json.is_array() == false)
		return;

	for (json::const_iterator itr = _json.begin(); itr != _json.end(); itr++)
	{
		// Box3Dクラスの生成に必要な情報が書かれているJSONを取得
		json box3DJSON = *itr;
		
		// 座標、スケール、3Dモデルのファイル名が書かれたJSONを取得
		json positionJSON = box3DJSON["position"];
		json scaleJSON = box3DJSON["scale"];
		json fileNameJSON = box3DJSON["fileName"];

		// 座標を取得
		Vector3 position = Vector3
		(
			positionJSON["x"].get<float>(),
			positionJSON["y"].get<float>(),
			positionJSON["z"].get<float>()
		);

		// スケールを取得
		Vector3 scale = Vector3
		(
			scaleJSON["x"].get<float>(),
			scaleJSON["y"].get<float>(),
			scaleJSON["z"].get<float>()
		);

		// ファイル名を取得
		std::string fileName = fileNameJSON.get<std::string>();

		// 
		currentScene->Instantiate<FBXModelEntity>(position, scale, fileName);
	}
}
