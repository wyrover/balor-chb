#pragma once


namespace balor {

namespace detail {
void* getSingletonInstance(const type_info& info, void* (*createInstanceFunction)());
}



/**
 * シングルトンパターンの実装を提供する。
 *
 * 複製不可能なグローバル変数を、マルチスレッドでもDLLを超えても安全に管理する。
 * テンプレート引数に渡すクラスは Singleton クラスに対して friend 宣言した上でコンストラクタとデストラクタを private 宣言すると良い。
 * DLL プロジェクトを含む複数のプロジェクトで balor を使用する場合、実行ファイルと同じディレクトリに balor_singleton.dll を置くこと。
 * この DLL が見つからない場合は DLL のことを考慮しないシングルトンになる。
 *
 * 注意！ DLL 内でシングルトンに初めてアクセスし、シングルトンのインスタンス登録をした場合はその DLL をアンロードするタイミングに注意すること。
 * アンロードした時点で登録したインスタンスのポインタが無効になってしまうばかりか、インスタンスが登録ずみかどうかの調査も不正な処理で落ちてしまう。
 *
 * C++ におけるシングルトンの実装にはいくつか問題があり、それぞれ解決する必要がある。
 * 
 * <h3>◎問題１：スタティックライブラリを DLL にリンクすると DLL ごとにグローバル変数領域が複製される。</h3>
 * DLL ごとにリンクされるのでこれは当然であり、グローバル変数のアドレスだけでなく、関数ポインタも DLL ごとに異なる。
 * たとえ DLL が一個であったとしても、アプリケーションの exe ファイルとそれを拡張するプラグインの DLL で
 * 同じスタティックライブラリを使用する場合、アプリケーションと DLL で別々にライブラリがリンクされる。
 * この問題はそもそもスタティックライブラリを使用しなければ解決するが、
 * ここではユーザがスタティックライブラリにするのか DLL にするのか選択できるようにしたい場合どうするかを考える。
 * 
 * <h3>◎問題２：static 変数の初期化はマルチスレッドではうまくいかない。</h3>
 * スレッド１が最初に関数を呼び出してstatic変数の初期化処理に入った後、
 * スレッド２が同時に関数を呼び出すと、処理系にもよるが初期化処理が重複して走ってしまうか、あるいは
 * 既に初期化済みであると判断して初期化処理をスキップし、未初期化のオブジェクトに触れてしまう。
 * この問題の解決策として良く知られているのが DCLP（The Double-Checked Locking Optimization Pattern）だが
 * この手法は環境に依存した方法を使わない限り安全ではない。
 *
 * http://www.nwcpp.org/Downloads/2004/DCLP_notes.pdf
 * http://d.hatena.ne.jp/yupo5656/20041011/p1
 *
 * DCLP は以下のようなコードになるが
 * <pre><code>
 * static T* instance = 0;
 * if (instance == 0) { // ライン１
 *     mutex::scoped_lock lock(mutex);
 *     if (instance == 0) { // 再度チェックしなければ二重にnew及びコンストラクトされる恐れがある
 *         instance = new T(); // ライン２
 *     }
 * }
 * </code></pre>
 * 問題はライン２で、ここでは (1)Tのメモリ割り当て、(2)Tのコンストラクタ、(3)instanceへの代入 の三つの処理を行うが、
 * (1)→(2)→(3)の順に処理されるならば問題は発生しないがコンパイラの最適化や CPU の並列実行によって(1)→(3)→(2)のように実行されることがありうる。
 * そうなるとライン１の判定で初期化が終わっていないのに初期化されたと判断されうる。
 * ペーパーにあるように環境に依存する方法で回避することは可能だがインスタンスの初期化が解決しても
 * mutex の初期化については堂々巡りとなっており、解決されない。また、問題１：も依然として残る。
 *
 * <h3>●解決策:</h3>
 * 問題１：を解決する為に、type_info構造体へのポインタと割り当てられたインスタンスへのポインタの組を
 * 小さなDLLの中に保存する。static変数の初期化処理が走るたびにこの DLL の関数を呼んで
 * type_info::oprator== が成り立つ type_info 構造体のポインタが既に登録されているかどうか調べ、
 * 登録されていない場合はインスタンスを作成して新たに登録し、登録されている場合は登録済みのインスタンスのポインタを返すようにする。
 * DLL ごとに別々にリンクされたそれぞれのコード内で、同じ型に対して &typeid(型名) を行うとそれぞれ違うアドレスが返ってくるが、
 * 両者で typeid::operator== を行うとちゃんと true が返る挙動に依存している。
 * これで異なるDLL間であっても同じアドレスを返すことが保証される。
 * また、この DLL 関数処理は boost::mutex で完全にロックする。複数回実行されても問題のない処理であり、
 * ロックにメモリバリアが含まれる為、DCLP のような手法が不要となる。
 * DLLのグローバル変数はプロセスにアタッチする前に初期化されることが保証されている為、
 * (http://msdn.microsoft.com/ja-jp/library/988ye33t(VS.80).aspx)
 * mutex の初期化は問題ない。
 * ただし、DLL を使用しないプロジェクトではこの処理自体が無駄になる為、小さな DLL が見つからない場合にはこれらの処理を行わないようにする。
 * 
 *
 * <h3>◎欠点：</h3>
 * 小さなDLLを exe に付属させなければならない。
 * グローバル変数の mutex を使用する為、あらゆる型の初期化処理が共通してロックされる。
 * また、type_info 構造体には operator== しかないので map が使えず、vector による全検索を行う。
 * この為、static 変数の初期化時のみパフォーマンスが犠牲になる。
 * なお type_info::name 関数は型の比較に用いることはできない。
 * 無名名前空間を使って重複する型名を定義した場合、type_info::name 関数では両者を区別できない。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	class MySingleton {
		friend Singleton<MySingleton>;
		MySingleton() {}
		~MySingleton() {}
	public:
		String text;
	};

	String text = Singleton<MySingleton>::get().text;
 * </code></pre>
 */
template<typename T>
class Singleton {
public:
	/// シングルトンインスタンスの取得
	static T& get() {
		static T* instance = nullptr;

		if (!instance) {
			// この関数は複数回実行されてもかまわない
			instance = static_cast<T*>(::balor::detail::getSingletonInstance(typeid(T), Singleton<T>::createInstance));
			// lock にメモリバリアが含まれるのでメモリ最適化はされない
		}
		return *instance;
	}

	// ライブラリをマルチスレッドで使わないし、DLL プロジェクトも使わないという場合はこの実装で良い。簡単なアプリケーションのほとんどに当てはまるだろう。
	// あるいは C++0x に完全に準拠したコンパイラであればこの実装でマルチスレッドは問題が無くなる。
	//static T& get() {
	//	static T instance;
	//	return instance;
	//}


private: // 一切の生成、コピー、破棄を禁ずる
	Singleton();
	Singleton(const Singleton& );
	~Singleton();
	Singleton& operator=(const Singleton& );

private:
	static void* createInstance() {
		static T instance;
		return &instance;
	}
};



}