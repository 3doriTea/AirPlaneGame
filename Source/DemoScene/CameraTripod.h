#pragma once
#include <mtgb.h>

/// <summary>
/// ŒÅ’èƒJƒƒ‰—p
/// </summary>
class CameraTripod : public GameObject
{
public:
	CameraTripod(const Vector3 _position, const Quaternion _quaternion);
	~CameraTripod();

	void Update() override;
	void Draw() const override;
};
