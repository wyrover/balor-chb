#pragma once

#include <balor/NonCopyable.hpp>
#include <balor/StringRange.hpp>

struct HINSTANCE__;

namespace balor {
	namespace io {
		class File;
	}
}


namespace balor {
	namespace system {



/**
 * exe ファイルや DLL ファイルを表す。現在のプロセスモジュールのパスを取得したり、DLL をプロセスのアドレス空間内にマップし、関数のアドレスを取得できる。
 */
class Module : private NonCopyable {
public:
	typedef ::HINSTANCE__* HMODULE; // HMODULE == HINSTANCE。
	typedef ::balor::io::File File;

public:
	/// 現在のプロセスモジュールで作成。（＝ヌルハンドルで作成）
	Module();
	///	ハンドルから作成。
	explicit Module(HMODULE handle, bool owned = false);
	/// DLL 名から作成。
	explicit Module(StringRange fileName);
	Module(Module&& value);
	~Module();
	Module& operator=(Module&& value);
	
public:
	/// 現在のプロセスのモジュールを返す。
	static Module current();
	/// モジュールファイルのあるディレクトリを返す。
	File directory() const;
	/// モジュールファイルを返す
	File file() const;
	/// 既にロードされている DLL が見つかれば返す。見つからなければ空の Module を返す。
	static Module find(StringRange fileName);
	/// 関数の型を指定して関数名から DLL に含まれる関数アドレスを返す。関数が見つからなければ nullptr を返す。関数名は Unicode ではない事に注意。
	template<typename Function> Function* getFunction(ByteStringRange functionName) { return reinterpret_cast<Function*>(_getVoidFunction(functionName)); }
	/// デストラクタでハンドルを破棄するかどうか。変更は要注意。
	bool owned() const;
	void owned(bool value);

public:
	/// モジュールハンドルへの自動変換と null チェック用。
	operator HMODULE() const { return _handle; }

private:
	void* _getVoidFunction(ByteStringRange functionName);

	HMODULE _handle;
	bool _owned;
};



	}
}