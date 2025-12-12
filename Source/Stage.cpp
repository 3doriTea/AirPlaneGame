#include "Stage.h"

#include <mtgb.h>

using namespace nlohmann;
void GenerateStageFromJSON(const nlohmann::json& _json)
{
	GameScene* currentScene = Game::System<SceneSystem>().GetActiveScene();

	for (json::const_iterator itr = _json.begin(); itr != _json.end(); itr++)
	{
		// Box3Dクラスの生成に必要な情報が書かれているJSONを取得
		json box3DJSON = *itr;
		
		// 座標、スケール、3Dモデルのファイル名を取得
		json positionJSON = box3DJSON["position"];
		json scaleJSON = box3DJSON["scale"];
		json fileNameJSON = box3DJSON["fileName"];

		Vector3 position = Vector3
		(
			positionJSON["x"].get<float>(),
			positionJSON["y"].get<float>(),
			positionJSON["z"].get<float>()
		);

		Vector3 scale = Vector3
		(
			scaleJSON["x"].get<float>(),
			scaleJSON["y"].get<float>(),
			scaleJSON["z"].get<float>()
		);

		std::string fileName = fileNameJSON.get<std::string>();
	}
}
