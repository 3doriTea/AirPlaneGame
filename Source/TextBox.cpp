#include "TextBox.h"
#include "Timer.h"
#include "Text.h"
#include "Draw.h"

using namespace mtgb;

TextBox::TextBox() : TextSec_(0), finished_(false), currentIndex_(0), hTimer_(nullptr)
{
}

TextBox::~TextBox()
{
}

// 1����������̕\���b��
void TextBox::SetTextSpeedSec(const float _sec)
{
    TextSec_ = _sec;
}

// ������������\������
void TextBox::Show(const std::string& _text)
{
    testtext_ = _text;
    currentIndex_ = 0;
    finished_ = false;

    if (hTimer_)
    {
        mtgb::Timer::Remove(hTimer_);
        hTimer_ = nullptr;
    }

    hTimer_ = mtgb::Timer::AddInterval(TextSec_, [&,this]()
        {

            if (currentIndex_ < testtext_.size())
            {
                ++currentIndex_;
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

bool TextBox::IsFinished()
{
    return finished_;
}

// ��������Q�[���I�u�W�F�N�g����Ă�
void TextBox::Draw() const
{
    if (currentIndex_ > 0)
    {
        Draw::ImmediateText(testtext_.substr(0, currentIndex_), 400.0f, 400.0f, 48);
    }
}