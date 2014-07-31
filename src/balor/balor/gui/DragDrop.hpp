#pragma once

#include <balor/gui/Control.hpp>
#include <balor/system/ComPtr.hpp>
#include <balor/Enum.hpp>
#include <balor/OutOfMemoryException.hpp>
#include <balor/StringRangeArray.hpp>

struct _IMAGELIST;
struct IDataObject;

namespace balor {
	namespace graphics {
		class Bitmap;
	}
	namespace io {
		class MemoryStream;
		class Stream;
	}
}


namespace balor {
	namespace gui {



/**
* ドラッグ＆ドロップ機能を提供する。
*
* ドラッグを開始するコントロールを指定して DragDrop::Source を作成し、Control::onDrag イベントまたは任意のイベントから DragDrop::Source::doDragDrop 関数でドラッグドロップを開始する。
* ドロップするコントロールを指定して DragDrop::Target を作成し、DragDrop::Target::onDrop イベントでドロップされたデータを処理する。
*
* <h3>・サンプルコード</h3>
* <pre><code>
Frame frame(L"DragDrop Sample");

Label label(frame, 20, 10, 0, 0, L"ここに文字列をドラッグ、またはここから文字列をドラッグできる");
label.edge(Label::Edge::client);
label.resize();

DragDrop::Target target(label);
target.onDrop() = [&] (DragDrop::Drop& e) {
if (e.data().containsText()) {
label.text(e.data().getText());
}
};
target.onMove() = [&] (DragDrop::Move& e) {
if (!e.data().containsText()) {
// 文字列のドラッグでなければ受け付けないアイコン表示にする。
e.effect(DragDrop::Effect::none);
}
};

DragDrop::Source source(label);
label.onDrag() = [&] (Control::Drag& e) {
if (e.lButton()) {
Bitmap bitmap(label.size());
label.drawTo(bitmap);
ImageList list(label.size());
list.add(bitmap);
source.doDragDrop(label.text(), DragDrop::Effect::move, list, 0, e.position().x, e.position().y);
}
};

frame.runMessageLoop();
* </code></pre>
*/
class DragDrop {
public:
	typedef ::_IMAGELIST* HIMAGELIST;
	typedef Control::HBITMAP HBITMAP;
	typedef Control::HCURSOR HCURSOR;
	typedef ::balor::graphics::Bitmap Bitmap;
	typedef ::balor::io::MemoryStream MemoryStream;
	typedef ::balor::io::Stream Stream;

	class Data;
	class Source;

private:
	class DropSource;
	class DropTarget;

public:
	/// メモリが足りなかった。
	struct OutOfMemoryException : public ::balor::OutOfMemoryException {};


	/// ドラッグ＆ドロップ操作。組み合わせで指定する。
	struct Effect {
		enum _enum {
			none = 0, /// 何も行われなかった。
			copy = 1, /// コピー操作を行う。
			move = 2, /// 移動操作を行う。
			link = 4, /// ショートカットの作成操作を行う。
			scroll = 0x80000000, ///
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Effect);
	};


	/// ドラッグ中のデータをドロップしたイベント。
	struct Drop : public Control::Event {
		Drop(Control& sender, const Data& data, Effect allowedEffects, int keyState, const Point& position);
		/// ドラッグ元が許可する操作の組み合わせ。
		DragDrop::Effect allowedEffects() const;
		/// ALT キーが押されているかどうか。
		bool alt() const;
		/// CTRL キーが押されているかどうか。
		bool ctrl() const;
		/// ドラッグしているデータ。
		const DragDrop::Data& data() const;
		/// ドラッグ＆ドロップの操作。組み合わせではない値になる。初期値はエクスプローラと同じ挙動。
		DragDrop::Effect effect() const;
		void effect(DragDrop::Effect value);
		/// マウスの左ボタンが押されているかどうか。
		bool lButton() const;
		/// マウスの中央ボタンが押されているかどうか。
		bool mButton() const;
		/// マウスカーソルの位置。
		const Point& position() const;
		/// マウスの右ボタンが押されているかどうか。
		bool rButton() const;
		/// Shift キーが押されているかどうか。
		bool shift() const;

	private:
		const Data& _data;
		Effect _allowedEffects;
		int _keyState;
		Point _position;
		Effect _effect;
	};


	/// ドラッグ中のマウスカーソルがコントロール上に入ったイベント。
	typedef Drop Enter;


	/// ドラッグ＆ドロップの操作 に対して適切なカーソルを設定するイベント。設定しなかった場合はシステムのデフォルトの挙動になる。
	/// DragDrop::Target クラスの onDragEnter や onDragMove イベント等の後に発生する。
	struct Feedback : public Control::Event {
		Feedback(Control& sender, Effect effect);

		/// カーソルを設定する。設定しなかった場合はシステムのデフォルトのカーソルになる。
		void cursor(HCURSOR value);
		/// 現状のドラッグ＆ドロップの操作。
		DragDrop::Effect effect() const;

	private:
		friend DropSource;

		Effect _effect;
		bool _useDefaultCursor;
	};


	/// ドラッグ中のマウスカーソルがコントロール上から出たイベント。
	typedef Control::Event Leave;


	/// ドラッグ中にマウスカーソルがコントロール上を移動したイベント。
	typedef Drop Move;


	/// キー入力状態等からドラッグ＆ドロップを続行するかキャンセルするか決めるイベント。
	/// DragDrop::Target クラスの onDragEnter や onDragMove イベント等の前に発生する。
	struct QueryContinue : public Control::Event {
		QueryContinue(Control& sender, bool esc, int keyState);

		/// ALT キーが押されているかどうか。
		bool alt() const;
		/// ドラッグ＆ドロップをキャンセルするかどうか。
		bool cancelDrag() const;
		void cancelDrag(bool value);
		/// CTRL キーが押されているかどうか。
		bool ctrl() const;
		/// ドロップしてドラッグ＆ドロップを終了するかどうか。
		bool drop() const;
		void drop(bool value);
		/// ESC キーが押されたかどうか。
		bool esc() const;
		/// マウスの左ボタンが押されているかどうか。
		bool lButton() const;
		/// マウスの中央ボタンが押されているかどうか。
		bool mButton() const;
		/// マウスの右ボタンが押されているかどうか。
		bool rButton() const;
		/// Shift キーが押されているかどうか。
		bool shift() const;

	private:
		bool _esc;
		int _keyState;
		bool _cancelDrag;
		bool _drop;
	};


public:
	/// ドラッグ＆ドロップするデータを表す。
	/// ユーザ定義のデータを使用する場合は registerMemoryFormat 関数で一意な名前でメモリフォーマットを登録する。
	class Data : private NonCopyable {
		friend Source;
	public:
		/// 空のデータを作成。
		Data();
		Data(Data&& value);
		/// ビットマップを持つデータを作成。
		Data(HBITMAP bitmap);
		Data(const Bitmap& bitmap);
		/// ユーザ定義のメモリデータを持つデータを作成。
		Data(int memoryFormat, MemoryStream& stream);
		/// 文字列を持つデータを作成。
		Data(const String& text);
		Data(const wchar_t* text);
		Data(const std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >& text);
		Data(::IDataObject* dataObject);
		~Data();
		Data& operator=(Data&& value);

	public:
		/// データに ビットマップが含まれるかどうか。
		bool containsBitmap() const;
		/// データに DIB ビットマップが含まれるかどうか。
		bool containsDIB() const;
		/// データにファイルドロップリストが含まれるかどうか。
		bool containsFileDropList() const;
		/// データにユーザ定義のメモリデータが含まれるかどうか。
		bool containsMemory(int memoryFormat) const;
		/// データに文字列が含まれるかどうか。
		bool containsText() const;
		/// DDB ビットマップを取得する。無い場合はヌルのビットマップを返す。
		Bitmap getBitmap() const;
		/// DIB ビットマップを取得する。無い場合はヌルのビットマップを返す。
		Bitmap getDIB() const;
		/// ファイルドロップリストを取得する。無い場合は空の配列を返す。
		std::vector<String, std::allocator<String> > getFileDropList() const;
		/// ユーザ定義のメモリデータを取得する。無い場合は空のメモリストリームを返す。
		MemoryStream getMemory(int memoryFormat) const;
		/// 文字列を取得する。無い場合は空文字列を返す。
		String getText() const;
		/// ユーザ定義のメモリフォーマット名を登録し、メモリフォーマットを返す。メモリフォーマット名が他のプロセスで既に登録されていたら同じメモリフォーマットを返す。
		static int registerMemoryFormat(StringRange memoryFormatName);
		/// DDB ビットマップを設定する。
		void setBitmap(HBITMAP value);
		/// DIB ビットマップを設定する。
		void setDIB(HBITMAP value);
		/// ファイルドロップリストを設定する。
		void setFileDropList(StringRangeArray value);
		/// ユーザ定義のメモリデータを設定する。
		void setMemory(int memoryFormat, Stream& stream);
		/// 文字列を設定する。
		void setText(StringRange value);

	private:
		::balor::system::ComPtr<::IDataObject> _dataObject;
	};


	/// ドラッグ＆ドロップを開始するコントロールを表す。
	class Source : private NonCopyable {
	public:
		/// 未初期化状態。
		Source();
		Source(Source&& value);
		/// ドラッグ＆ドロップの開始点となるコントロールを指定して作成。
		Source(Control& control);
		~Source();
		Source& operator=(Source&& value);

	public:
		/// ドラッグ＆ドロップを開始する。最終的に行われた操作を返す。allowedEffect には許可するドラッグ＆ドロップ操作の組み合わせを設定する。
		/// また画像リストとその画像インデックス、画像の左上からみたマウスカーソルの位置を指定してマウスカーソルに重ねて画像を表示することができる。
		DragDrop::Effect doDragDrop(const DragDrop::Data& data, DragDrop::Effect allowedEffects = Effect::copy | Effect::move | Effect::link | Effect::scroll
			, HIMAGELIST imageList = nullptr, int imageIndex = 0, int xHotSpot = 0, int yHotSpot = 0);
		/// ドラッグ＆ドロップの操作に対して適切なカーソルを設定するイベント。設定しなかった場合はシステムのデフォルトの挙動になる。
		Listener<DragDrop::Feedback&>& onFeedback();
		/// キー入力状態等からドラッグ＆ドロップを続行するかキャンセルするか決めるイベント。
		Listener<DragDrop::QueryContinue&>& onQueryContinue();

	private:
		::balor::system::ComPtr<DropSource> _dropSource;
	};


	/// ドラッグ＆ロップを受け取るコントロールを表す。
	/// 要注意！このクラスはコンストラクタ引数に渡した Control よりも先に破壊しないとメモリリークする。
	class Target : private NonCopyable {
	public:
		/// 未初期化状態。
		Target();
		Target(Target&& value);
		/// ドラッグ＆ドロップを受け取るコントロールを指定して作成。
		Target(Control& control);
		~Target();
		Target& operator=(Target&& value);

	public:
		/// ドロップしたイベント。
		Listener<DragDrop::Drop&>& onDrop();
		/// ドラッグ中のマウスカーソルがコントロール上に入ったイベント。ここで DragDrop::Enter::data() 関数がどのデータを持っているか調べて
		/// 処理できるデータが無ければ DragDrop::Enter::effect() に DragDrop::Effect::none を指定したりする。
		Listener<DragDrop::Enter&>& onEnter();
		/// ドラッグ中のマウスカーソルがコントロール上から出たイベント。
		Listener<DragDrop::Leave&>& onLeave();
		/// ドラッグ中にマウスカーソルがコントロール上を移動したイベント。ここで DragDrop::Move::data() 関数がどのデータを持っているか調べて
		/// 処理できるデータが無ければ DragDrop::Move::effect() に DragDrop::Effect::none を指定したりする。
		Listener<DragDrop::Move&>& onMove();

	private:
		::balor::system::ComPtr<DropTarget> _dropTarget;
	};


public:
	/// マウスボタンを押しながら移動した時にドラッグ＆ドロップを開始する移動範囲のシステム標準。
	static Size defaultDragSize();
};



	}
}