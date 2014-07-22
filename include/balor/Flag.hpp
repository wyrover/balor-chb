#pragma once


namespace balor {



/**
 * ビット演算を bool 値を使ってわかりやすくする。
 * 
 * WIN32 API に多数存在するフラグ定数の操作に使用する。
 * std::bitset はビット位置をマスクではなく整数位置で指定せねばならず、元の型との相互変換も簡単ではない。
 */
template<typename T>
class Flag {
public:
	/// 初期化せずに作成。
	Flag() {}
	/// 値から作成。
	Flag(T value) : _value(value) {}
	Flag(const Flag& value) : _value(value._value) {}

	/// 値を代入。
	Flag& operator=(T value) { _value = value; return *this; }

public:
	/// 指定したビットマスクのビットをオンまたオフにする。
	Flag& set(T mask, bool on = true) { _value = on ? (_value | mask) : (_value & ~mask); return *this; }

public:
	/// 値に変換。
	operator T() const { return _value; }
	/// 指定したビットマスクのビットがすべてオンかどうか。
	bool operator [] (T mask) const { return (_value & mask) == mask; }

private:
	T _value;
};


/// 任意型からフラグへの変換関数
template<typename T>
Flag<T> toFlag(T value) { return Flag<T>(value); }



}