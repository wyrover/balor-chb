#pragma once

#include <balor/Enum.hpp>
#include <balor/NonCopyable.hpp>

struct _IMAGELIST;
struct HBITMAP__;
struct HICON__;

namespace balor {
	class Size;
}


namespace balor {
	namespace graphics {

class Bitmap;
class Color;
class Icon;



/**
* 画像リスト。
*
* 同じ大きさ、フォーマットの画像をまとめて管理し、透過カラーキーやアルファチャンネルによる半透明合成をサポートする。
*/
class ImageList : private NonCopyable {
public:
	typedef ::_IMAGELIST* HIMAGELIST;
	typedef ::HBITMAP__* HBITMAP;
	typedef ::HICON__* HICON;

	/// 画像リストのフォーマット。
	struct Format {
		enum _enum {
			palette4bpp = 0x00000004, /// １６色パレット。
			palette8bpp = 0x00000008, /// ２５６色パレット。
			rgb16bpp = 0x00000010, /// １６ビットカラー画像。
			rgb24bpp = 0x00000018, /// ２４ビットカラー画像。
			argb32bpp = 0x00000020, /// アルファチャンネル付き３２ビットカラー画像。
		};
		BALOR_NAMED_ENUM_MEMBERS(Format);
	};

	/// 画像の描画状態。Graphics::drawImageList 関数の引数に使う。
	struct State {
		enum _enum {
			normal = 0x00000000, /// マスクを使って描画。
			drawMask = 0x00000010, /// マスク画像を描画。
			unmasked = 0x00000020, /// マスクを使わずに描画。ただしマスクを持たないアルファチャンネル画像には無効。
			focused = 0x00000002, /// フォーカス状態で描画。
			selected = 0x00000004, /// 選択状態で描画。
		};
		BALOR_NAMED_ENUM_MEMBERS(State);
	};

	/// システムリソースの画像リストの種類。fromSystemResource 関数の引数に使う。
	struct SystemResource {
		enum _enum {
			smallStandardImages = 0, /// 標準的なアプリケーションで使用される小さなアイコンの画像リスト。
			largeStandardImages = 1, /// 標準的なアプリケーションで使用される大きなアイコンの画像リスト。
			smallViewImages = 4, /// リストビューの操作を表す小さなアイコンの画像リスト。
			largeViewImages = 5, /// リストビューの操作を表す大きなアイコンの画像リスト。
			smallExplorerImages = 8, /// ウインドウズエクスプローラで使用される小さなアイコンの画像リスト。
			largeExplorerImages = 9, /// ウインドウズエクスプローラで使用される小さなアイコンの画像リスト。
		};
		BALOR_NAMED_ENUM_MEMBERS(SystemResource);
	};

	/// システムリソースの画像リスト（smallStandardImages、largeStandardImages）の画像インデックス。
	struct StandardImage {
		enum _enum {
			cut = 0, /// 切り取り。
			copy = 1, /// コピー。
			paste = 2, /// 貼り付け。
			undo = 3, /// 元に戻す。
			redo = 4, /// やり直し。
			remove = 5, /// 削除。
			fileNew = 6, /// ファイルの新規作成。
			fileOpen = 7, /// ファイルを開く。
			fileSave = 8, /// ファイルの保存。
			printPreview = 9, /// 印刷プレビュー。
			properties = 10, /// プロパティ。
			help = 11, /// ヘルプ。
			find = 12, /// 検索。
			replace = 13, /// 置換。
			print = 14, /// 印刷。
		};
		BALOR_NAMED_ENUM_MEMBERS(StandardImage);
	};

	/// システムリソースの画像リスト（smallViewImages、largeViewImages）の画像インデックス。
	struct ViewImage {
		enum _enum {
			largeIcons = 0, /// 大きなアイコン。
			smallIcons = 1, /// 小さなアイコン。
			list = 2, /// 一覧。
			details = 3, /// 詳細。
			sortName = 4, /// 名前順でソート。
			sortSize = 5, /// 大きさ順でソート。
			sortDate = 6, /// 日付順でソート。
			sortType = 7, /// 種類でソート。
			parentFolder = 8, /// 親フォルダへ移動。
			netConnect = 9, /// ネットワークドライブに接続。
			netDisconnect = 10, /// ネットワークドライブから切断。
			newFolder = 11, /// 新しいフォルダ。
			viewMenu = 12, /// メニュー。
		};
		BALOR_NAMED_ENUM_MEMBERS(ViewImage);
	};

	/// システムリソースの画像リスト（smallExplorerImages、largeExplorerImages）の画像インデックス。
	struct ExplorerImage {
		enum _enum {
			back = 0, /// 戻る。
			forward = 1, /// 進む。
			favorites = 2, /// お気に入り。
			addToFavorites = 3, /// お気に入りに追加。
			viewTree = 4, /// ツリー表示。
		};
		BALOR_NAMED_ENUM_MEMBERS(ExplorerImage);
	};

public:
	/// ヌルハンドルで作成。
	ImageList();
	ImageList(ImageList&& value);
	/// ハンドルから作成。owned が true ならばデストラクタでハンドルを破棄する。
	explicit ImageList(HIMAGELIST handle, bool owned = false);
	/// 大きさと画像フォーマットから作成。masked はモノクロのマスク画像を使うかどうか。
	explicit ImageList(const Size& imageSize, ImageList::Format format = Format::argb32bpp, bool masked = true);
	ImageList(int width, int height, ImageList::Format format = Format::argb32bpp, bool masked = true);
	/// システムリソースから画像リストを作成する。
	explicit ImageList(ImageList::SystemResource systemResource);
	~ImageList();
	ImageList& operator=(ImageList&& value);

public:
	/// 画像をリストの末尾に追加する。画像の幅は imageSize().width 以上でなければならない。画像の幅が imageSize().width の n 倍である場合は n 個の画像を追加する。
	/// マスク画像はモノクロビットマップで、白い部分が透過する。カラーキーを指定した場合はカラーキーの色の部分を透過する。
	/// ビットマップが３２ビット画像でなおかつ１ドットでも０ではないアルファを持つ場合、マスク画像もカラーキーも無視されてアルファチャンネルで透過される。
	/// 追加後にビットマップハンドルは参照されない。
	void add(HBITMAP bitmap, HBITMAP mask = nullptr);
	void add(HBITMAP bitmap, const Color& colorKey);
	/// アイコンをリストの末尾に追加する。
	/// リソースアイコンであった場合、ロードされた時のサイズがなんであろうと、imageSize() に合うサイズのアイコンが選択される。そうでない場合は imageSize() に合わせて拡大縮小される。
	/// 追加後にアイコンハンドルは参照されない。
	void add(HICON icon);
	/// 画像リストを空にする。
	void clear();
	/// 画像リストを複製する。
	ImageList clone() const;
	static ImageList clone(HIMAGELIST handle);
	/// 画像の数。
	int count() const;
	/// 画像をリストから削除する。
	void erase(int index);
	/// 画像フォーマット。
	ImageList::Format format();
	/*/// システムアイコンの画像リスト。編集したりハンドルを削除すると大変な事になるので注意。
	//static ImageList fromSystemIcons();*//**/
	/// １ピクセルあたりのビット数から画像リストのフォーマットに変換する。
	static ImageList::Format getFormatFromBitsPerPixel(int bitsPerPixel);
	/// 画像をアイコンにして取得する。
	Icon getIcon(int index) const;
	/// リスト内の画像のサイズ。
	Size imageSize() const;
	/// デストラクタでハンドルを破棄するかどうか。変更は要注意。
	bool owned() const;
	void owned(bool value);
	/// 指定したインデックスの画像を置き換える。インデックス以外の引数は add 関数と同じ。置き換え後に引数のビットマップハンドル、アイコンハンドルは参照されない。
	void replace(int index, HBITMAP bitmap, HBITMAP mask = nullptr);
	void replace(int index, HBITMAP bitmap, const Color& colorKey);
	void replace(int index, HICON icon);

public:
	/// HIMAGELIST への自動変換 ＆ null チェック用
	operator HIMAGELIST() const { return _handle; }

private:
	HIMAGELIST _handle;
	bool _owned;
};



	}
}