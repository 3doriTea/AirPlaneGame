#pragma once
#include "ImGuiShowable.h"
#include "mtgb.h"
class Player;
class TransformProxy;
class TransformGuizmo;
class PlayerProxy : public ImGuiShowable<Player>
{
public:
	PlayerProxy(Player* player);
	void ShowImGui() override;

	int* test1;
	float* test2;
	std::string_view name;
	TransformProxy* pTransformProxy_;
	TransformGuizmo* pTransformGuizmo_;
};