#pragma once
#include <DirectXMath.h>
#include <string>


namespace mtgb
{
	struct Matrix4x4 : DirectX::XMMATRIX
	{
		static const size_t ROW_SIZE{ 4 };
		static const size_t COLUMN_SIZE{ 4 };

		//using DirectX::XMMATRIX::XMMATRIX;

		inline Matrix4x4() :
			XMMATRIX{ DirectX::XMMatrixIdentity() }
		{}

		inline Matrix4x4(const Matrix4x4& _other) = default;

		inline Matrix4x4(const DirectX::XMMATRIX& _xmMatrix) :
			XMMATRIX{ _xmMatrix }
		{}

		inline Matrix4x4& operator=(const Matrix4x4& _other)
		{
			if (this != &_other)
			{
				XMMATRIX::operator=(_other);
			}
			return *this;
		}
		
		std::string ToString();
	};
}
