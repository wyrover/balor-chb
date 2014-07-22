●動機

C++ でWIN32 API を使った GUI プログラミングを楽にしたい。
具体的にはJava SDK や .NET Framework のように手軽で使いやすく、
MFC のように WIN32 API と連携しやすいライブラリが欲しい。



●balorとは

C++ Windows GUI ライブラリです。

Java や .NET のように手軽で使いやすく、
MFC のように WIN32 API と連携しやすいライブラリを目指します。

○特徴

・.NET や Java ライクなインターフェース。
・WIN32 API や STL と連携しやすい。
・無名関数（ラムダ式）によるイベント記述。
・スタティックリンクライブラリ
・右辺値参照サポートによってインスタンス作成にヒープメモリ割り当てを強制しない。
・WinMain 関数を自分で書ける。
・ライブラリヘッダファイルに windows.h をインクルードしない。
・インテリセンスで関数のコメント表示。



●対応開発環境

Visau C++ 2010 Express Edition 以降



●動作環境

Windows XP 以降
６４ビット版は対応予定。




●ライセンス

balor ライブラリについて著作権は放棄しません（できない）が
配布条件はパブリックドメインソフトウェア (public domain software, PDS)と同等とします 。
非商用、商用を問わず使用にあたり制限はありません。
本ライブラリを使用することで生じたいかなる損害についても責任は全て使用者当人が負います。





●使用方法

まずＰＣに Visual C++ 2010 以降がインストールされていない場合はインストールして下さい。
次にダウンロードした balor の圧縮ファイルを適当なディレクトリに解凍します。
解凍したディレクトリ内の「samples」の中にサンプルプロジェクトがあるので
Visual C++ でそのままコンパイルできることを確認します。

Visual C++ を起動し、「Win32 プロジェクト」を新規作成します。
プロジェクトのプロパティを開いて

構成プロパティ＞ＶＣ＋＋ディレクトリ＞インクルードディレクトリ

に balor ディレクトリ内の「include」ディレクトリを追加します。

構成プロパティ＞ＶＣ＋＋ディレクトリ＞ライブラリディレクトリ

に balor ディレクトリ内の「lib」ディレクトリを追加します。

以上の設定で balor ライブラリが使えるようになりました。
試しに以下の Hello world サンプルコードをコンパイルして見て下さい。

#include <Windows.h>
#include <balor/gui/all.hpp>

using namespace balor::gui;


int APIENTRY WinMain(HINSTANCE , HINSTANCE , LPSTR , int ) {
	Frame frame(L"Hello world");
	frame.onPaint() = [&] (Frame::Paint& e) {
		e.graphics().drawText(L"Hello world", 20, 10);
	};
	frame.runMessageLoop();
	return 0;
}



●製作者

syanji

質問、不具合、要望、意見等は公式サイトのブログコメントにて受け付けます。
http://d.hatena.ne.jp/syanji/
