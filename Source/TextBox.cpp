#include "TextBox.h"
#include "Timer.h"
#include "Text.h"
#include "Draw.h"
#include "MTStringUtility.h"

using namespace mtgb;

TextBox::TextBox(const float _popTimeSec, const Vector2F _drawPosition, const int _fontSize) : GameObject(GameObjectBuilder()
	.Build()),
	popTimeSec_{ _popTimeSec },
	drawPosition_{ _drawPosition },
	showText_{},
	currentIndex_{ 0 },
	finished_{ false },
	hTimer_{ nullptr },
	fontSize_{ _fontSize }
{
}

TextBox::TextBox(const std::string _initShowText, const float _popTimeSec, const Vector2F _drawPosition, const int _fontSize) :
	TextBox{ _popTimeSec, _drawPosition, _fontSize }
{
	showText_ = { _initShowText.begin(), _initShowText.end() };
}

TextBox::~TextBox()
{
	Timer::Remove(hTimer_);
}

// 1文字あたりの表示秒数
void TextBox::SetPopTimeSec(const float _timeSec)
{
	popTimeSec_ = _timeSec;
}

// 文字列をいざ表示する
void TextBox::Show(const std::u8string& _text)
{
	showText_ = _text;
	currentIndex_ = 0;
	finished_ = false;

	if (hTimer_)
	{
		mtgb::Timer::Remove(hTimer_);
		hTimer_ = nullptr;
	}

	hTimer_ = mtgb::Timer::AddInterval(popTimeSec_, [&, this]()
		{
			if (currentIndex_ < GetSizeUTF8Characters(showText_))
			{
				currentIndex_++;
			}
			else
			{
				finished_ = true;
				if (hTimer_ != nullptr)
				{
					mtgb::Timer::Remove(hTimer_);
					hTimer_ = nullptr;
				}
			}
		});
}

void TextBox::Hide()
{
	showText_.clear();
	currentIndex_ = 0;
	finished_ = false;
	hTimer_ = nullptr;
}

bool TextBox::IsFinished() const
{
	return finished_;
}


// 所属するゲームオブジェクトから呼ぶ
void TextBox::Draw() const
{
	if (currentIndex_ > 0 && !showText_.empty())
	{
		Draw::ChangeTextAlignment(TextAlignment::topLeft);
		//std::u8string_view cut{ showText_.substr(0, currentIndex_) };
		Draw::ImmediateTextW(UTF8ToWide(SubStrBegin(showText_, currentIndex_)), drawPosition_, fontSize_);
	}
	//Draw::ImmediateText(testtext_, 0, 30, 100);
}