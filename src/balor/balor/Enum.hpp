#pragma once


namespace balor {



/**
 * 名前付き enum の定義をサポートするマクロ関数。
 *
 * enum を名前付き struct または class の中に入れてこのマクロでメンバーの定義をすることで enum と struct 間の自動的な変換を行えるようにする。
 * name に struct 名を指定する。列挙体名は必ず _enum とすること。値の正当性チェック関数が必要な場合は _validate 関数を実装すること。
 */
#define BALOR_NAMED_ENUM_MEMBERS(name) \
	name () {}\
	explicit name (int value) : _value(static_cast<_enum>(value)) {}\
	name (_enum value) : _value(value) {}\
	operator _enum() const { return _value; }\
	static bool _validate(name value);\
	_enum _value;


/**
 * 論理演算可能な名前付き enum の定義をサポートするマクロ関数。
 *
 * |、&、~ 演算子をサポートする他は BALOR_NAMED_ENUM_MEMBERS と同じ。
 */
#define BALOR_NAMED_LOGICAL_ENUM_MEMBERS(name) \
	BALOR_NAMED_ENUM_MEMBERS(name)\
	name& operator|=(_enum value) { _value = _enum(_value | value); return *this; }\
	friend _enum operator|(_enum lhs, _enum rhs) { return name::_enum(int(lhs) | rhs); }\
	friend _enum operator&(_enum lhs, _enum rhs) { return name::_enum(int(lhs) & rhs); }\
	friend _enum operator~(_enum value) { return name::_enum(~int(value)); }



}