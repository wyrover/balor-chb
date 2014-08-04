#include "StreamToIStream.hpp"

#include <balor/test/verify.hpp>
#include <balor/Exception.hpp>


namespace balor {
	namespace io {

using namespace balor::system;



StreamToIStream::StreamToIStream(Stream& stream) : _stream(&stream) {
}


StreamToIStream::~StreamToIStream() {
}


ComPtr<StreamToIStream> StreamToIStream::create(Stream& stream) {
	ComPtr<StreamToIStream> pointer;
	*(&pointer) = new StreamToIStream(stream);
	return pointer;
}


HRESULT STDMETHODCALLTYPE StreamToIStream::Clone(IStream**) { return E_NOTIMPL; }
HRESULT STDMETHODCALLTYPE StreamToIStream::Commit(DWORD) { return E_NOTIMPL; }
HRESULT STDMETHODCALLTYPE StreamToIStream::CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER*) { return E_NOTIMPL; }


HRESULT STDMETHODCALLTYPE StreamToIStream::LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) { return E_NOTIMPL; }


HRESULT STDMETHODCALLTYPE StreamToIStream::Read(void* buffer, ULONG size, ULONG* readSize) {
	try {
		ULONG l = _stream->read(buffer, 0, size);
		if (readSize) {
			*readSize = l; 
		}
		return S_OK;
	} catch (Exception& ) {
		assert(false);
		return S_FALSE;
	}
}


HRESULT STDMETHODCALLTYPE StreamToIStream::Revert() { return E_NOTIMPL; }


HRESULT STDMETHODCALLTYPE StreamToIStream::Seek(LARGE_INTEGER offset, DWORD origin, ULARGE_INTEGER* newPosition) {
	try {
		switch(origin) {
			case STREAM_SEEK_SET: _stream->position(offset.QuadPart); break;
			case STREAM_SEEK_CUR: _stream->skip(offset.QuadPart); break;
			case STREAM_SEEK_END: _stream->position(_stream->length() + offset.QuadPart); break;
		}
	} catch (Exception& ) {
		assert(false);
		return S_FALSE;
	}
	if (newPosition) {
		newPosition->QuadPart = _stream->position();
	}
	return S_OK;
}


HRESULT STDMETHODCALLTYPE StreamToIStream::SetSize(ULARGE_INTEGER) { return E_NOTIMPL; }


HRESULT STDMETHODCALLTYPE StreamToIStream::Stat(STATSTG* stat, DWORD ) {
	ZeroMemory(stat, sizeof(*stat));
	stat->type = STGTY_STREAM;
	stat->cbSize.QuadPart = _stream->length();
	stat->clsid = CLSID_NULL;
	if (_stream->readable() && _stream->writable()) {
		stat->grfMode = STGM_READWRITE;
	} else if (_stream->readable()) {
		stat->grfMode = STGM_READ;
	} else if (_stream->writable()) {
		stat->grfMode = STGM_WRITE;
	}
	stat->grfMode |= STGM_SHARE_EXCLUSIVE;
	return S_OK;
}


HRESULT STDMETHODCALLTYPE StreamToIStream::UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) { return E_NOTIMPL; }


HRESULT STDMETHODCALLTYPE StreamToIStream::Write(const void* buffer, ULONG size, ULONG* writeSize) {
	try {
		_stream->write(buffer, 0, size);
		if (writeSize) {
			*writeSize = size; 
		}
		return S_OK;
	} catch (Exception& ) {
		assert(false);
		return S_FALSE;
	}
}



	}
}