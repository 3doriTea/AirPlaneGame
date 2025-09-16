#pragma once
#include <mtgb.h>

class HighlightEnemy : public mtgb::GameObject
{
public:
	HighlightEnemy();
	~HighlightEnemy();

	void Update() override;
	void Draw() const override;
	void Highlight();
private:
	RectDetector firstWndRectDetector_;
	RectDetector secondWndRectDetector_;
	ImageHandle highlightFrameImage_;
};

