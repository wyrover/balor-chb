#pragma once


namespace balor {



/**
 * ビット演算を bool 値を使ってわかりやすくする。
 * 
 * WIN32 API に多数存在するフラグ定数の?作に使用する。
 * std::bitset はビット位置を?スクではなく整数位置で指定せねばならず、元の?との相互変換も簡単ではない。
 */
template<typename T>
class Flag {
public:
	/// 初期化せずに作成。
	Flag() {}
	/// 値から作成。
	Flag(T value) : _value(value) {}
	Flag(const Flag& value) : _value(value._value) {}

	/// 値をｴ・ﾔ。
	Flag& operator=(T value) { _value = value; return *this; }

public:
	/// 指定したビット?スクのビットをオンまたオフにする。
	Flag& set(T mask, bool on = true) { _value = on ? (_value | mask) : (_value & ~mask); return *this; }

public:
	/// 値に変換。
	operator T() const { return _value; }
	/// 指定したビット?スクのビットがすべてオンかどうか。
	bool operator [] (T mask) const { return (_value & mask) == mask; }

private:
	T _value;
};


/// 任意?からフラグへの変換関数
template<typename T>
Flag<T> toFlag(T value) { return Flag<T>(value); }



}