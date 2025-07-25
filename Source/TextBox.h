#pragma once
#include <string>
#include <mtgb.h>

class TextBox
{
public:
	TextBox();
	~TextBox();
	// 1����������̕\���b��
	void SetTextSpeedSec(const float _sec);
	void Show(const std::string& _text);  // ������������\������
	bool IsFinished();  // �\�����I����Ă��� true / false
	void Draw() const;  // ��������Q�[���I�u�W�F�N�g����Ă�

private:
	// �ϐ����͉�
	float TextSec_;
	std::string testtext_;
	size_t currentIndex_;
	bool finished_;
	TimerHandle hTimer_;
	TimerHandle cTimer_;
};