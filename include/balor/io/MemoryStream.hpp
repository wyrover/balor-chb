#pragma once

#include <balor/io/Stream.hpp>
#include <balor/Exception.hpp>


namespace balor {
	namespace io {



/**
 * 固定長メモリまたは動的に割り当てたメモリのストリーム。
 *
 * std::iostream には標準では存在しないので自作する。
 */
class MemoryStream : public Stream {
public:
	// 書き込みが固定長バッファをオーバーした
	class BufferOverrunException : public Exception {};

	// オーバーロード関数のオーバーライド用
	using Stream::read;
	using Stream::write;

public:
	/// 初期のバッファサイズを指定して必要に応じてバッファを拡張していくメモリストリームを作成。
	MemoryStream(int capacity = 256);
	/// 固定長配列のメモリバッファから作成。
	template<typename T, int Size> MemoryStream(T (&buffer)[Size], bool writable = true)
		: _first(reinterpret_cast<unsigned char*>(buffer)), _last(_first + sizeof(buffer)), _end(_last), _current(_first), _allocatable(false), _writable(writable) {}
	/// 固定長のメモリバッファから作成。
	MemoryStream(void* buffer, int offset, int bufferSize, bool writable = true);
	MemoryStream(MemoryStream&& stream);
	virtual ~MemoryStream();

	MemoryStream& operator=(MemoryStream&& stream);

public:
	/// メモリバッファの先頭のポインタ。
	void* buffer();
	const void* buffer() const;
	/// バッファの大きさ。大きくなることはあっても小さくなることはない。
	int capacity() const;
	virtual void flush();
	virtual __int64 length() const;
	void length(int value);
	virtual __int64 position() const;
	virtual void position(__int64 value);
	virtual int read(void* buffer, int offset, int count);
	virtual bool readable() const;
	virtual __int64 skip(__int64 offset);
	virtual void write(const void* buffer, int offset, int count);
	virtual bool writable() const;

private:
	unsigned char* _first;
	unsigned char* _last;
	unsigned char* _end;
	unsigned char* _current;
	bool _allocatable;
	bool _writable;
};



	}
}