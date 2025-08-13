#pragma once
#include <mtgb.h>

class PlayerGunner : public mtgb::GameObject,ImGuiShowable<PlayerGunner>
{
public:
	PlayerGunner(const EntityId _plane);
	~PlayerGunner();

	void Update() override;
	void Draw() const override;

	void ShowImGui() override;
private:
	Transform* pTransform_;
	float angleX_;
	float angleY_;
};
