#pragma once
#include "ISystem.h"
#include "Vector2Int.h"

namespace mtgb
{
	class Screen : public ISystem
	{
	public:
		Screen();
		~Screen();

		void Initialize() override;
		void Update() override;

		inline const Vector2Int GetSize() const { return{ width_, height_ }; }

	private:
		int width_;     // �X�N���[���̉���
		int height_;    // �X�N���[���̏c��
		int fpsLimit_;  // FPS���� (1�t���[��������̍X�V�t���[����)
	};
}
