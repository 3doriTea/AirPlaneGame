#pragma once
#include <type_traits>
#include <limits>
#include <cmath>
namespace mtgb
{
	template<typename T>
	struct Vector2T
	{
		T x;
		T y;

		template<typename U>
		Vector2T(const Vector2T<U>& other)
			:x{ static_cast<T>(other.x) }
			,y{ static_cast<T>(other.y)}
		{ }
		template<typename U>
		Vector2T(U x, U y)
			:x{ static_cast<T>(x)}
			,y{ static_cast<T>(y)}
		{ }
		
		Vector2T() = default;
		Vector2T(T x, T y);
		// コピーコンストラクタ
		Vector2T(const Vector2T& other) = default;

		// ムーブコンストラクタ
		Vector2T(Vector2T&& other)  = default;
		Vector2T& operator=(const Vector2T&) = default;
		Vector2T& operator=(Vector2T&&) = default;
		~Vector2T() = default;

#pragma region 四則演算
		inline Vector2T& operator*=(T _scalar) { x *= _scalar; y *= _scalar; return *this; }
		inline Vector2T& operator*=(const Vector2T& _other) { x *= _other.x; y *= _other.y; return *this; }
		inline Vector2T& operator/=(T _scalar) { x /= _scalar; y /= _scalar; return *this; }
		inline Vector2T& operator/=(const Vector2T& _other) { x /= _other.x; y /= _other.y; return *this; }

		inline Vector2T& operator+=(const Vector2T& _other) { x += _other.x; y += _other.y; return *this; }
		inline Vector2T& operator-=(const Vector2T& _other) { x -= _other.x; y -= _other.y; return *this; }
#pragma endregion

#pragma region 定数
		static Vector2T Zero() 
		{
			return Vector2T( T{}, T{} );
		}
		static Vector2T One()
		{
			return Vector2T(T{1}, T{1});
		}
#pragma endregion


	};
	#pragma region グローバルオペレータ
		template<typename T>
		inline Vector2T<T> operator*(const Vector2T<T>& _v, T _scalar) { return Vector2T<T>{ _v } *= _scalar; }
		template<typename T>
		inline Vector2T<T> operator*(T _scalar, const Vector2T<T>& _v) { return Vector2T<T>{ _v } *= _scalar; }
		template<typename T>
		inline Vector2T<T> operator/(const Vector2T<T>& _v, T _scalar) { return Vector2T<T>{ _v } /= _scalar; }
		template<typename T>
		inline Vector2T<T> operator/(T _scalar, const Vector2T<T>& _v) { return Vector2T<T>{ _v } /= _scalar; }
		template<typename T>
		inline Vector2T<T> operator/(const Vector2T<T>& _v1, const Vector2T<T>& _v2) { return Vector2T<T>{_v1.x / _v2.x, _v1.y / _v2.y}; }
		template<typename T>
		inline Vector2T<T> operator+(const Vector2T<T>& _v1, Vector2T<T> _v2) { return Vector2T<T>{ _v1 } += _v2; }
		template<typename T>
		inline Vector2T<T> operator-(const Vector2T<T>& _v1, const Vector2T<T>& _v2) { return Vector2T<T>{ _v1 } -= _v2; }
		
		// 異なる型同士の演算

		template<typename T, typename U>
		inline auto operator+(const Vector2T<T>& _v1, const Vector2T<U>& _v2)
			-> Vector2T<std::common_type_t<T, U>>
		{
			using R = std::common_type_t<T, U>;
			return Vector2T<R>(static_cast<R>(_v1.x) + static_cast<R>(_v2.x),
				static_cast<R>(_v1.y) + static_cast<R>(_v2.y));
		}

		template<typename T, typename U>
		inline auto operator-(const Vector2T<T>& _v1, const Vector2T<U>& _v2)
			-> Vector2T<std::common_type_t<T, U>>
		{
			using R = std::common_type_t<T, U>;
			return Vector2T<R>(static_cast<R>(_v1.x) - static_cast<R>(_v2.x),
				static_cast<R>(_v1.y) - static_cast<R>(_v2.y));
		}

		template<typename T, typename U>
		inline auto operator*(const Vector2T<T>& _v, U _scalar)
			-> Vector2T<std::common_type_t<T, U>>
		{
			using R = std::common_type_t<T, U>;
			return Vector2T<R>(static_cast<R>(_v.x) * static_cast<R>(_scalar),
				static_cast<R>(_v.y) * static_cast<R>(_scalar));
		}

		// TODO: _scalarが変換できずにエラー
		template<typename T, typename U>
		inline auto operator*(const Vector2T<T>& _v1, Vector2T<U> _v2)
			-> Vector2T<std::common_type_t<T, U>>
		{
			using R = std::common_type_t<T, U>;
			return Vector2T<R>
				(
					static_cast<R>(_v1.x) * static_cast<R>(_v2.x),
					static_cast<R>(_v1.y) * static_cast<R>(_v2.y)
				);
		}

		template<typename T, typename U>
		inline auto operator*(U _scalar, const Vector2T<T>& _v)
			-> Vector2T<std::common_type_t<T, U>>
		{
			return _v * _scalar;
		}

		
		template<typename T, typename U>
		inline auto operator/(const Vector2T<T>& _v, U _scalar)
			-> Vector2T<std::common_type_t<T, U>>
		{
			using R = std::common_type_t<T, U>;
			return Vector2T<R>(static_cast<R>(_v.x) / static_cast<R>(_scalar),
				static_cast<R>(_v.y) / static_cast<R>(_scalar));
		}

		template<typename T>
		inline bool operator==(const Vector2T<T>& _v1, const Vector2T<T>& _v2) {
			if constexpr (std::is_floating_point_v<T>)
			{
				constexpr T epsilon = std::numeric_limits<T>::epsilon();
				return std::abs(_v1.x - _v2.x) < epsilon && std::abs(_v1.y - _v2.y) < epsilon;
			}
			else 
			{
				return _v1.x == _v2.x  && _v1.y == _v2.y;
			}
		}
		template<typename T>
		inline bool operator!=(const Vector2T<T>& _v1, const Vector2T<T>& _v2) { return !(_v1 == _v2); }
#pragma endregion
		template<typename T>
		inline Vector2T<T>::Vector2T(T x, T y) :
			x{ x },
			y{ y }
		{}


}
