#pragma once

#include <balor/NonCopyable.hpp>


namespace balor {
	namespace io {



/**
 * バイナリストリームを表す抽象クラス。
 */
class Stream : private NonCopyable {
protected:
	Stream();
	virtual ~Stream();

public:
	/// ストリームのバッファをフラッシュ（同期）する。
	virtual void flush() = 0;
	/// ストリームの長さ。
	virtual __int64 length() const = 0;
	/// 現在位置。
	virtual __int64 position() const = 0;
	virtual void position(__int64 value) = 0;
	/// １バイト読み出して返す。ファイルの終わりに達していたら -1 を返す。
	virtual int read();
	/// 指定したバイト数の読み出しを試みて、実際に読み出したバイト数を返す。
	virtual int read(void* buffer, int offset, int count) = 0;
	/// 読み出し可能かどうか。
	virtual bool readable() const = 0;
	/// 現在位置から指定したバイト数分移動する。
	virtual __int64 skip(__int64 offset) = 0;
	/// １バイト書き込む。
	virtual void write(unsigned char value);
	/// 指定したバイト数書き込む。
	virtual void write(const void* buffer, int offset, int count) = 0;
	/// 書き込み可能かどうか。
	virtual bool writable() const = 0;
};



	}
}