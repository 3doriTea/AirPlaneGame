#pragma once
#include "nlohmann/json.hpp"
/// <summary>
/// JSONからデータを読み取り、ステージを生成する
/// </summary>
/// <param name="_json">ステージ生成に使用するJSON</param>
void GenerateStageFromJSON(const nlohmann::json& _json);
