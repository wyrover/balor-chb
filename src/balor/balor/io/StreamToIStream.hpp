#pragma once

#include <balor/io/Stream.hpp>
#include <balor/system/ComBase.hpp>
#include <balor/system/ComPtr.hpp>
#include <ObjBase.h>


namespace balor {
	namespace io {


/**
 * ::balor::sytem::io::Stream をラップして COM の IStream として振舞うようにするクラス。
 * 
 * Gdiplus や OleLoadPicture 等の関数で使う。
 */
class StreamToIStream : public ::balor::system::ComBase<IStream> {
	/// Stream の参照から作成。
	explicit StreamToIStream(Stream& stream);
	StreamToIStream(const StreamToIStream& );
	virtual ~StreamToIStream();
	StreamToIStream& operator=(const StreamToIStream& );

public:
	/// インスタンスを作成する。
	static ::balor::system::ComPtr<StreamToIStream> create(Stream& stream);

public:
	virtual HRESULT STDMETHODCALLTYPE Clone(IStream**);
	virtual HRESULT STDMETHODCALLTYPE Commit(DWORD);
	virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER*);
	virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD);
	virtual HRESULT STDMETHODCALLTYPE Read(void* buffer, ULONG size, ULONG* readSize);
	virtual HRESULT STDMETHODCALLTYPE Revert();
	virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER offset, DWORD origin, ULARGE_INTEGER* newPosition);
	virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER);
	virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG* stat, DWORD );
	virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD);
	virtual HRESULT STDMETHODCALLTYPE Write(const void* buffer, ULONG size, ULONG* writeSize);

private:
	Stream* _stream;
};



	}
}