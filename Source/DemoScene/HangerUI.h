#pragma once
#include <mtgb.h>

class HangerUI : public GameObject
{
public:
	HangerUI(const mtgb::GameObjectLayer _layer);
	~HangerUI();

	void Update() override;
	void Draw() const override;

	bool IsOpened() const { return moveTimeLeft_ <= 0.0f; }

private:
	float moveTimeLeft_;  // ƒXƒeƒBƒbƒN‚ð“|‚µ‚Ä‚¢‚éŽžŠÔ
	ImageHandle hHangerImage_;  // Ši”[ŒÉ‚Ì‰æ‘œ
	ImageHandle hDoorImage_;    // Ši”[ŒÉƒhƒA‚Ì‰æ‘œ
	ImageHandle hConHintImage_;    // Ši”[ŒÉƒhƒA‚Ì‰æ‘œ
	ImageHandle hTextKeepImage_;    // à–¾‚Ì‰æ‘œ
	ImageHandle hTextReadyImage_;    // à–¾‚Ì‰æ‘œ
	ImageHandle hTextImage_;

	mtgb::RectF drawRectDoor_;  // ƒhƒA‚Ì•`‰æ”ÍˆÍ
	mtgb::RectF drawRectText_;  // ƒeƒLƒXƒg‚Ì•`‰æ”ÍˆÍ
};
