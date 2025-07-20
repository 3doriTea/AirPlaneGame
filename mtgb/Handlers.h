#pragma once
#include <cstdint>


#pragma region Handlers

namespace mtgb
{
	/// <summary>
	/// 画像のハンドル
	/// </summary>
	using ImageHandle = int32_t;
	/// <summary>
	/// 音声のハンドル
	/// </summary>
	using AudioHandle = int32_t;
	/// <summary>
	/// モデルのハンドル
	/// </summary>
	using ModelHandle = int32_t;
	/// <summary>
	/// OBJモデルのハンドル
	/// </summary>
	using OBJModelHandle = int32_t;

	/// <summary>
	/// テキストのハンドル
	/// </summary>
	using TextHandle = int32_t;

	/// <summary>
	/// 無効なハンドル値
	/// </summary>
	static const int32_t INVALID_HANDLE{ -1 };
}

#pragma endregion
