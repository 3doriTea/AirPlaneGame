#pragma once
#include "ISystem.h"

namespace mtgb
{
	class DirectWrite : public ISystem
	{
	public:
		DirectWrite();
		~DirectWrite();

		void Initialize() override;
		void Update() override;
	};
}
