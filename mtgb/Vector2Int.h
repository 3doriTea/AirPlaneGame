#pragma once

namespace mtgb
{
	//struct Vector2;
	
	/// <summary>
	/// 2次元の整数型ベクトル
	/// </summary>
	struct Vector2Int
	{
		int x;  // x成分
		int y;  // y成分
	
		Vector2Int() :
			Vector2Int{ 0, 0 }
		{}
	
		Vector2Int(const int& x, const int& y) :
			x{ x },
			y{ y }
		{}
	
#pragma region 四則演算
		inline Vector2Int& operator*=(const int& _scalar) { x *= _scalar; y *= _scalar; return *this; }
		inline Vector2Int& operator/=(const int& _scalar) { x /= _scalar; y /= _scalar; return *this; }
		inline Vector2Int& operator+=(const Vector2Int& _other) { x += _other.x; y += _other.y; return *this; }
		inline Vector2Int& operator-=(const Vector2Int& _other) { x -= _other.x; y -= _other.y; return *this; }
#pragma endregion
	
#pragma region キャスト
		//explicit operator Vector2() const noexcept;
#pragma endregion
	
#pragma region 定数
		inline static Vector2Int Zero() { return { 0, 0 }; }
		inline static Vector2Int One() { return { 1, 1 }; }
#pragma endregion
	};

#pragma region グローバルオペレータ
	inline Vector2Int operator*(const Vector2Int& _v, const int& _scalar) { return Vector2Int{ _v } *= _scalar; }
	inline Vector2Int operator*(const int& _scalar, const Vector2Int& _v) { return Vector2Int{ _v } *= _scalar; }
	inline Vector2Int operator/(const Vector2Int& _v, const int& _scalar) { return Vector2Int{ _v } /= _scalar; }
	inline Vector2Int operator/(const int& _scalar, const Vector2Int& _v) { return Vector2Int{ _v } /= _scalar; }
	inline Vector2Int operator+(const Vector2Int& _v1, const Vector2Int& _v2) { return Vector2Int{ _v1 } += _v2; }
	inline Vector2Int operator-(const Vector2Int& _v1, const Vector2Int& _v2) { return Vector2Int{ _v1 } -= _v2; }
	
	inline bool operator==(const Vector2Int& _v1, const Vector2Int& _v2) { return _v1.x - _v2.x == 0 && _v1.y - _v2.y == 0; }
	inline bool operator!=(const Vector2Int& _v1, const Vector2Int& _v2) { return !(_v1 == _v2); }
#pragma endregion
}
