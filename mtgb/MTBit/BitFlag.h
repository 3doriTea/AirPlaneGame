#pragma once
#include <type_traits>
#include <bitset>

namespace mtbit
{
	// 列挙型 かつ size_t型の暗黙的型変換ができない！ = enum struct
	template<typename EnumStructT>
	concept IsEnumStruct = std::is_enum_v<EnumStructT> && !std::is_convertible_v<EnumStructT, std::underlying_type_t<EnumStructT>>;

	template<typename EnumStructT>
	class BitFlag
	{
		static_assert(
			IsEnumStruct<EnumStructT>,
			"enum structのみ指定できます。");
		// 指定された enum struct のサイズ分のビット数
		static constexpr size_t BIT_COUNT{ sizeof(EnumStructT) * 8 };

	public:
		class BitFlagEditor
		{
			friend BitFlag;
		public:
			~BitFlagEditor() = default;

			BitFlagEditor& On(const EnumStructT _e);
			BitFlagEditor& Off(const EnumStructT _e);
			BitFlagEditor& Add(const BitFlag _other);
			BitFlagEditor& Sub(const BitFlag _other);
			BitFlagEditor& OnAll();

			BitFlag& EndEdit() { return bitFlag_; }

		private:
			BitFlagEditor(BitFlag& _bitFlag) : bitFlag_{ _bitFlag } {}

		private:
			BitFlag& bitFlag_;
		};

	public:
		BitFlag() = default;
		BitFlag(const BitFlag&) = default;
		BitFlag(const EnumStructT _e);
		~BitFlag() = default;

		BitFlagEditor BeginEdit() { return BitFlagEditor{ *this }; }

		bool Has(const EnumStructT _e) const;
		bool Has(const BitFlag _other) const;

		bool Is(const BitFlag _other) const;

		static BitFlag New() { return BitFlag{}; }

	private:
		std::bitset<BIT_COUNT> value_;
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
