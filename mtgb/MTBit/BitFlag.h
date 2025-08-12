#pragma once
#include <type_traits>
#include <bitset>

namespace mtbit
{
	// 列挙型 かつ size_t型の暗黙的型変換ができない！ = enum struct
	template<typename EnumStructT>
	concept IsEnumStruct = std::is_enum_v<EnumStructT> && !std::is_convertible_v<EnumStructT, std::underlying_type_t<EnumStructT>>;

	/// <summary>
	/// スコープ付き列挙型を使ったビットフラグ
	/// </summary>
	/// <typeparam name="EnumStructT">スコープ付き列挙型</typeparam>
	template<typename EnumStructT>
	class BitFlag
	{
		static_assert(IsEnumStruct<EnumStructT>, "enum structのみ指定できます。");
		// 指定された enum struct のサイズ分のビット数
		static constexpr size_t BIT_COUNT{ sizeof(EnumStructT) * 8 };

	public:
		/// <summary>
		/// 操作可能なビットフラグ
		/// </summary>
		class BitFlagEditor
		{
			friend BitFlag;
		public:
			~BitFlagEditor() = default;

			/// <summary>
			/// 指定のフラグを立てる
			/// </summary>
			/// <param name="_e">スコープ付き列挙型</param>
			/// <returns>続けて関数を呼び出せる</returns>
			BitFlagEditor& On(const EnumStructT _e);
			/// <summary>
			/// 指定のフラグを降ろす
			/// </summary>
			/// <param name="_e">スコープ付き列挙型</param>
			/// <returns>続けて関数を呼び出せる</returns>
			BitFlagEditor& Off(const EnumStructT _e);
			/// <summary>
			/// 指定のフラグを追加する
			/// </summary>
			/// <param name="_other">スコープ付き列挙型</param>
			/// <returns>続けて関数を呼び出せる</returns>
			BitFlagEditor& Add(const BitFlag _other);
			/// <summary>
			/// 指定のフラグを引く
			/// </summary>
			/// <param name="_other">スコープ付き列挙型</param>
			/// <returns>続けて関数を呼び出せる</returns>
			BitFlagEditor& Sub(const BitFlag _other);
			/// <summary>
			/// すべてのフラグを立てる
			/// </summary>
			/// <returns>続けて関数を呼び出せる</returns>
			BitFlagEditor& OnAll();
			/// <summary>
			/// すべてのフラグを降ろす
			/// </summary>
			/// <returns>続けて関数を呼び出せる</returns>
			BitFlagEditor& OffAll();

			/// <summary>
			/// フラグの操作を終了する
			/// </summary>
			/// <returns>ビットフラグ</returns>
			BitFlag& EndEdit() { return bitFlag_; }

		private:
			BitFlagEditor(BitFlag& _bitFlag) : bitFlag_{ _bitFlag } {}

		private:
			BitFlag& bitFlag_;  // 編集するビットフラグ
		};

	public:
		BitFlag() = default;
		BitFlag(const BitFlag&) = default;
		BitFlag(const EnumStructT _e);
		~BitFlag() = default;

		/// <summary>
		/// フラグの操作を開始する
		/// </summary>
		/// <returns></returns>
		BitFlagEditor BeginEdit() { return BitFlagEditor{ *this }; }

		/// <summary>
		/// 指定フラグが立っているかどうか
		/// </summary>
		/// <param name="_e">指定フラグ</param>
		/// <returns>立っている true / false</returns>
		bool Has(const EnumStructT _e) const;
		/// <summary>
		/// 指定したフラグのいずれかが立っているかどうか
		/// </summary>
		/// <param name="_other">指定フラグ</param>
		/// <returns>立っている true / false</returns>
		bool Has(const BitFlag _other) const;

		/// <summary>
		/// フラグが一致しているか
		/// </summary>
		/// <param name="_other">フラグ</param>
		/// <returns>一致している true / false</returns>
		bool Is(const BitFlag _other) const;

		/// <summary>
		/// インスタンスを作る
		/// </summary>
		/// <returns>インスタンス</returns>
		static BitFlag New() { return BitFlag{}; }

	private:
		std::bitset<BIT_COUNT> value_;  // ビットフラグ
	};
}

template<mtbit::IsEnumStruct EnumStructT>
mtbit::BitFlag<EnumStructT> operator|(const EnumStructT _e1, const EnumStructT _e2)
{
	return mtbit::BitFlag<EnumStructT>{ _e1 } | _e2;
}

template<typename EnumStructT>
mtbit::BitFlag<EnumStructT> operator|(const mtbit::BitFlag<EnumStructT> _flag1, const mtbit::BitFlag<EnumStructT> _flag2)
{
	return mtbit::BitFlag<EnumStructT>{ _flag1 }.BeginEdit().Add(_flag2).EndEdit();
}

template<typename EnumStructT>
mtbit::BitFlag<EnumStructT> operator|(const EnumStructT _e, const mtbit::BitFlag<EnumStructT> _flag)
{
	return mtbit::BitFlag<EnumStructT>{ _flag }.BeginEdit().On(_e).EndEdit();
}

template<typename EnumStructT>
mtbit::BitFlag<EnumStructT> operator|(const mtbit::BitFlag<EnumStructT> _flag, const EnumStructT _e)
{
	return mtbit::BitFlag<EnumStructT>{ _flag }.BeginEdit().On(_e).EndEdit();
}

template<typename EnumStructT>
inline mtbit::BitFlag<EnumStructT>::BitFlag(const EnumStructT _e)
{
	value_.set(static_cast<size_t>(_e));
}

template<typename EnumStructT>
inline bool mtbit::BitFlag<EnumStructT>::Has(const EnumStructT _e) const
{
	return value_[static_cast<size_t>(_e)];
}

template<typename EnumStructT>
inline bool mtbit::BitFlag<EnumStructT>::Has(const BitFlag _other) const
{
	return (value_ & _other.value_).any();
}

template<typename EnumStructT>
inline bool mtbit::BitFlag<EnumStructT>::Is(const BitFlag _other) const
{
	return value_ == _other.value_;
}

template<typename EnumStructT>
inline typename mtbit::BitFlag<EnumStructT>::BitFlagEditor& mtbit::BitFlag<EnumStructT>::BitFlagEditor::On(const EnumStructT _e)
{
	bitFlag_.value_.set(static_cast<size_t>(_e));
	return *this;
}

template<typename EnumStructT>
inline typename mtbit::BitFlag<EnumStructT>::BitFlagEditor& mtbit::BitFlag<EnumStructT>::BitFlagEditor::Off(const EnumStructT _e)
{
	bitFlag_.value_.reset(static_cast<size_t>(_e));
	return *this;
}

template<typename EnumStructT>
inline typename mtbit::BitFlag<EnumStructT>::BitFlagEditor& mtbit::BitFlag<EnumStructT>::BitFlagEditor::Add(const BitFlag _other)
{
	bitFlag_.value_ |= _other.value_;
	return *this;
}

template<typename EnumStructT>
inline typename mtbit::BitFlag<EnumStructT>::BitFlagEditor& mtbit::BitFlag<EnumStructT>::BitFlagEditor::Sub(const BitFlag _other)
{
	bitFlag_.value_ &= !_other.value_;
	return *this;
}

template<typename EnumStructT>
inline typename mtbit::BitFlag<EnumStructT>::BitFlagEditor& mtbit::BitFlag<EnumStructT>::BitFlagEditor::OnAll()
{
	bitFlag_.value_.set();
	return *this;
}

template<typename EnumStructT>
inline typename mtbit::BitFlag<EnumStructT>::BitFlagEditor& mtbit::BitFlag<EnumStructT>::BitFlagEditor::OffAll()
{
	bitFlag_.value_.reset();
	return *this;
}
