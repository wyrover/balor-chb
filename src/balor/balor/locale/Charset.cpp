#include "Charset.hpp"

#include <vector>
#include <string>
#include <boost/scoped_array.hpp>
#include <MLang.h>

#include <balor/test/verify.hpp>
#include <balor/system/Com.hpp>
#include <balor/system/ComPtr.hpp>
#include <balor/system/windows.hpp>
#include <balor/Flag.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace locale {

using std::move;
using std::string;
using std::vector;
using boost::scoped_array;
using namespace balor::system;



Charset::Charset(int codePage, bool isWindowsCodePage)
	: _codePage(codePage), _isWindowsCodePage(isWindowsCodePage), _throwable(false), _fallbackChar(L'?') {
	assert("Invalid codePage" && 0 <= codePage);
	assert("Invalid codePage" && codePage < 65536);
	assert("Invalid windows codePage" && (!isWindowsCodePage || IsValidCodePage(codePage)));

	_fallbackBytes[0] = '?'; // この文字コードの意味が異なるキャラクターセットもあるかもしれないがとりあえず ASCII コードで設定。
	_fallbackBytes[1] = '\0';

	if (!isWindowsCodePage) {
		HRESULT result = _getMultiLanguage()->IsConvertible(codePage, 1200); // Unicode に変換不能なら存在しないとみなす
		if (FAILED(result) || result == S_FALSE) {
			throw NotFoundException();
		}
	}
}


Charset::Charset(StringRange name)
	: _isWindowsCodePage(false), _throwable(false), _fallbackChar(L'?') {
	_fallbackBytes[0] = '?'; // この文字コードの意味が異なるキャラクターセットもあるかもしれないがとりあえず ASCII コードで設定。
	_fallbackBytes[1] = '\0';
	MIMECSETINFO info;
	if (FAILED(_getMultiLanguage()->GetCharsetInfo(const_cast<wchar_t*>(name.c_str()), &info))) {
		throw NotFoundException();
	}
	_codePage = info.uiInternetEncoding;
}


Charset::Charset(Charset&& value)
	: _codePage(value._codePage)
	, _isWindowsCodePage(value._isWindowsCodePage)
	, _throwable(value._throwable)
	, _fallbackChar(value._fallbackChar)
	, _multiLanguage(move(value._multiLanguage))
	{
	memcpy(_fallbackBytes, value._fallbackBytes, sizeof(_fallbackBytes));
}


Charset::~Charset() {
}


Charset& Charset::operator=(Charset&& value) {
	if (this != &value) {
		_codePage = value._codePage;
		_isWindowsCodePage = value._isWindowsCodePage;
		_throwable = value._throwable;
		_fallbackChar = value._fallbackChar;
		std::memcpy(_fallbackBytes, value._fallbackBytes, sizeof(_fallbackBytes));
		_multiLanguage = move(value._multiLanguage);
	}
	return *this;
}


Charset Charset::ascii() {
	return Charset(20127);
}


Charset Charset::bigEndianUnicode() {
	return Charset(1201);
}


vector<Charset> Charset::charsets() {
	ComPtr<IEnumCodePage> enumCodePage;
	verify(SUCCEEDED(default()._getMultiLanguage()->EnumCodePages(MIMECONTF_VALID, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), &enumCodePage)));
	MIMECPINFO info;
	ULONG fetched = 0;
	HRESULT result;
	vector<Charset> charsets;
	while ((result = enumCodePage->Next(1, &info, &fetched)) == S_OK) {
		assert(fetched == 1);
		charsets.push_back(Charset(info.uiCodePage));
	}
	assert(result == S_FALSE);
	return charsets;
}


int Charset::codePage() const {
	return _codePage;
}


Charset Charset::default() {
	return Charset(GetACP(), true);
}


String Charset::decode(ByteStringRange src) const {
	const int srcSize = src.length();
	const UINT bufferSize = srcSize * 4 + 6;
	scoped_array<wchar_t> buffer(new wchar_t[bufferSize]);
	return String(buffer.get(), decode(src, srcSize, ArrayRange<wchar_t>(buffer.get(), bufferSize)));
}


String Charset::decode(ByteStringRange src, int count) const {
	assert("count out of range" && 0 <= count);
	assert("count out of range" && count <= src.length());
	return decode(ByteStringRange(src.c_str(), count));
}


int Charset::decode(ByteStringRange src, int count, ArrayRange<wchar_t> dst) const {
	assert("count out of range" && 0 <= count);
	assert("count out of range" && count <= src.length());
	assert("Empty dst" && !dst.empty());

	UINT bufferSize = dst.length() - 1;
	if (_isWindowsCodePage) {
		bufferSize = MultiByteToWideChar(codePage(), 0, src.c_str(), count, dst.begin(), bufferSize);
		if (bufferSize == 0 && 0 < count) {
			const DWORD errorCode = GetLastError();
			if (errorCode == ERROR_INSUFFICIENT_BUFFER) {
				throw InsufficientBufferExceptoin();
			}
			assert(L"failed to WideCharToMultiByte" && false);
		}
	} else {
		DWORD mode = 0;
		UINT srcSize = count;
		//wchar_t fallbackString[] = {_fallbackChar, L'\0'};
		HRESULT result = _getMultiLanguage()->ConvertStringToUnicodeEx(&mode, codePage(), const_cast<char*>(src.c_str()), &srcSize
			, dst.begin(), &bufferSize,  0, nullptr); //MLCONVCHARF_USEDEFCHAR, fallbackString); // 後２引数は現状無視されるらしい
		if (FAILED(result)) {
			const DWORD errorCode = GetLastError();
			if (errorCode == ERROR_INSUFFICIENT_BUFFER) {
				throw InsufficientBufferExceptoin();
			}
			assert(L"failed to IMultiLanguage2::ConvertStringFromUnicodeEx" && false);
		} else if (result == S_FALSE && throwable()) {
			throw ConversionFailedException();
		}
	}
	*(dst.begin() + bufferSize) = L'\0';
	return bufferSize;
}


String Charset::displayName() const {
	MIMECPINFO info;
	verify(SUCCEEDED(_getMultiLanguage()->GetCodePageInfo(_codePage, GetUserDefaultUILanguage(), &info)));
	return info.wszDescription;
}


string Charset::encode(ByteStringRange src, const Charset& srcCharset) const {
	const int srcSize = src.length();
	const UINT bufferSize = srcSize * 6 + 6;
	scoped_array<char> buffer(new char[bufferSize]);
	return string(buffer.get(), encode(src, srcSize, ArrayRange<char>(buffer.get(), bufferSize), srcCharset));
}


string Charset::encode(ByteStringRange src, int count, const Charset& srcCharset) const {
	assert("count out of range" && 0 <= count);
	assert("count out of range" && count <= src.length());
	return encode(ByteStringRange(src.c_str(), count), srcCharset);
}


int Charset::encode(ByteStringRange src, int count, ArrayRange<char> dst, const Charset& srcCharset) const {
	assert("count out of range" && 0 <= count);
	assert("count out of range" && count <= src.length());
	assert("Empty dst" && !dst.empty());

	if (_getMultiLanguage()->IsConvertible(codePage(), srcCharset.codePage()) == S_OK) {
		UINT bufferSize = dst.length() - 1;
		DWORD mode = 0;
		UINT srcSize = count;
		HRESULT result = _getMultiLanguage()->ConvertString(&mode, srcCharset.codePage(), codePage()
			, const_cast<BYTE*>(reinterpret_cast<const BYTE*>(src.c_str())), &srcSize
			, reinterpret_cast<BYTE*>(dst.begin()), &bufferSize);
		if (FAILED(result)) {
			if (result == E_FAIL) {
				throw InsufficientBufferExceptoin();
			}
			assert("Failed to IMultiLanguage2::ConvertString" && false);
		} else if (result == S_FALSE && throwable()) {
			throw ConversionFailedException();
		}
		*(dst.begin() + bufferSize) = '\0';
		return bufferSize;
	} else {
		String temp = srcCharset.decode(src, count);
		return encode(temp, temp.length(), dst);
	}
}


string Charset::encode(StringRange src) const {
	const int srcSize = src.length();
	const UINT bufferSize = srcSize * 6 + 6;
	scoped_array<char> buffer(new char[bufferSize]);
	return string(buffer.get(), encode(src, srcSize, ArrayRange<char>(buffer.get(), bufferSize)));
}


string Charset::encode(StringRange src, int count) const {
	assert("count out of range" && 0 <= count);
	assert("count out of range" && count <= src.length());
	return encode(StringRange(src.c_str(), count));
}


int Charset::encode(StringRange src, int count, ArrayRange<char> dst) const {
	assert("count out of range" && 0 <= count);
	assert("count out of range" && count <= src.length());
	assert("Empty dst" && !dst.empty());

	UINT bufferSize = dst.length() - 1;
	if (_isWindowsCodePage) {
		BOOL failed = FALSE;
		if (_codePage == 65000 || _codePage == 65001) {
			bufferSize = ::WideCharToMultiByte(codePage(), 0, src.c_str(), count, dst.begin(), bufferSize, nullptr, nullptr); // こうしないと失敗する
		} else {
			bufferSize = ::WideCharToMultiByte(codePage(), WC_NO_BEST_FIT_CHARS, src.c_str(), count, dst.begin(), bufferSize, _fallbackBytes, &failed);
		}
		if (bufferSize == 0 && 0 < count) {
			const DWORD errorCode = GetLastError();
			if (errorCode == ERROR_INSUFFICIENT_BUFFER) {
				throw InsufficientBufferExceptoin();
			}
			assert(L"failed to WideCharToMultiByte" && false);
		}
		if (failed && throwable()) {
			throw ConversionFailedException();
		}
	} else {
		DWORD mode = 0;
		UINT srcSize = count;
		wchar_t fallbackString[] = {_fallbackChar, L'\0'};
		HRESULT result = _getMultiLanguage()->ConvertStringFromUnicodeEx(&mode, codePage(), const_cast<wchar_t*>(src.c_str()), &srcSize
			, dst.begin(), &bufferSize,  MLCONVCHARF_USEDEFCHAR, fallbackString);
		if (FAILED(result)) {
			const DWORD errorCode = GetLastError();
			if (errorCode == ERROR_INSUFFICIENT_BUFFER) {
				throw InsufficientBufferExceptoin();
			}
			assert(L"failed to IMultiLanguage2::ConvertStringFromUnicodeEx" && false);
		} else if (result == S_FALSE && throwable()) {
			throw ConversionFailedException();
		}
	}
	*(dst.begin() + bufferSize) = '\0';
	return bufferSize;
}


Charset eucJp() {
	return Charset(51932);
}


wchar_t Charset::fallbackChar() const {
	return _fallbackChar;
}


void Charset::fallbackChar(wchar_t value) {
	_fallbackChar = value;
	verify(0 < encode(&_fallbackChar, 1, _fallbackBytes));
}


String Charset::name() const {
	MIMECPINFO info;
	verify(SUCCEEDED(_getMultiLanguage()->GetCodePageInfo(_codePage, GetUserDefaultUILanguage(), &info)));
	return info.wszWebCharset;
}


Charset shiftJis() {
	return Charset(932);
}


bool Charset::throwable() const {
	return _throwable;
}


void Charset::throwable(bool value) {
	_throwable = value;
}


Charset Charset::unicode() {
	return Charset(1200);
}


Charset Charset::utf7() {
	return Charset(65000);
}


Charset Charset::utf8() {
	return Charset(65001);
}


const ComPtr<IMultiLanguage2>& Charset::_getMultiLanguage() const {
	const_cast<Charset*>(this)->_multiLanguage = ComPtr<IMultiLanguage2>(CLSID_CMultiLanguage);
	return _multiLanguage;
}



	}
}