#pragma once


namespace balor {

namespace detail {
void* getSingletonInstance(const type_info& info, void* (*createInstanceFunction)());
}



/**
 * シングルトンパ??ンの実装を提供する。
 *
 * 複製不可?なグロ?バル変数を、?ル?スレッドでもDLLを超えても安全に管理する。
 * テンプレ?ト引数に渡すクラスは Singleton クラスに対して friend 宣言した上でコンストラク?とデストラク?を private 宣言すると良い。
 * DLL プロジェクトを含む複数のプロジェクトで balor を使用する場合、実行フ?イルと同じディレクトリに balor_singleton.dll を置くこと。
 * この DLL が見つからない場合は DLL のことを考慮しないシングルトンになる。
 *
 * 注意！ DLL 内でシングルトンに初めてアクセスし、シングルトンのインス?ンス登?をした場合はその DLL をアンロ?ドする?イ?ングに注意すること。
 * アンロ?ドした時?で登?したインス?ンスの?イン?が無効になってしまうばかりか、インス?ンスが登?ずみかどうかの調査も不正な処理で落ちてしまう。
 *
 * C++ におけるシングルトンの実装にはいくつか問題があり、それぞれ解決する必要がある。
 * 
 * <h3>◎問題１：ス?ティックライブラリを DLL にリンクすると DLL ごとにグロ?バル変数領域が複製される。</h3>
 * DLL ごとにリンクされるのでこれは当然であり、グロ?バル変数のアドレスだけでなく、関数?イン?も DLL ごとに異なる。
 * たとえ DLL が一個であったとしても、アプリケ?ションの exe フ?イルとそれを拡張するプラグインの DLL で
 * 同じス?ティックライブラリを使用する場合、アプリケ?ションと DLL で別々にライブラリがリンクされる。
 * この問題はそもそもス?ティックライブラリを使用しなければ解決するが、
 * ここではユ?ザがス?ティックライブラリにするのか DLL にするのか選択できるようにしたい場合どうするかを考える。
 * 
 * <h3>◎問題２：static 変数の初期化は?ル?スレッドではうまくいかない。</h3>
 * スレッド１が最初に関数を呼び出してstatic変数の初期化処理に入った後、
 * スレッド２が同時に関数を呼び出すと、処理系にもよるが初期化処理が重複して走ってしまうか、あるいは
 * 既に初期化済みであると判断して初期化処理をスキップし、未初期化のオブジェクトに触れてしまう。
 * この問題の解決策として良く知られているのが DCLP（The Double-Checked Locking Optimization Pattern）だが
 * この手?は環境に依存した方?を使わない限り安全ではない。
 *
 * http://www.nwcpp.org/Downloads/2004/DCLP_notes.pdf
 * http://d.hatena.ne.jp/yupo5656/20041011/p1
 *
 * DCLP は以下のようなコ?ドになるが
 * <pre><code>
 * static T* instance = 0;
 * if (instance == 0) { // ライン１
 *     mutex::scoped_lock lock(mutex);
 *     if (instance == 0) { // 再度?ェックしなければ二重にnew及びコンストラクトされる恐れがある
 *         instance = new T(); // ライン２
 *     }
 * }
 * </code></pre>
 * 問題はライン２で、ここでは (1)Tのメモリ割り当て、(2)Tのコンストラク?、(3)instanceへのｴ�ﾀﾔ の三つの処理を行うが、
 * (1)→(2)→(3)の順に処理されるならば問題は発生しないがコンパイラの最適化や CPU の並列実行によって(1)→(3)→(2)のように実行されることがありうる。
 * そうなるとライン１の判定で初期化が終わっていないのに初期化されたと判断されうる。
 * ペ?パ?にあるように環境に依存する方?で回避することは可?だがインス?ンスの初期化が解決しても
 * mutex の初期化については堂々巡りとなっており、解決されない。また、問題１：も依然として残る。
 *
 * <h3>●解決策:</h3>
 * 問題１：を解決する為に、type_info?造体への?イン?と割り当てられたインス?ンスへの?イン?の組を
 * 小さなDLLの中に保存する。static変数の初期化処理が走るたびにこの DLL の関数を呼んで
 * type_info::oprator== が成り立つ type_info ?造体の?イン?が既に登?されているかどうか調べ、
 * 登?されていない場合はインス?ンスを作成して新たに登?し、登?されている場合は登?済みのインス?ンスの?イン?を返すようにする。
 * DLL ごとに別々にリンクされたそれぞれのコ?ド内で、同じ?に対して &typeid(?名) を行うとそれぞれ違うアドレスが返ってくるが、
 * 両者で typeid::operator== を行うとちゃんと true が返る挙動に依存している。
 * これで異なるDLL間であっても同じアドレスを返すことが保証される。
 * また、この DLL 関数処理は boost::mutex で完全にロックする。複数回実行されても問題のない処理であり、
 * ロックにメモリバリアが含まれる為、DCLP のような手?が不要となる。
 * DLLのグロ?バル変数はプロセスにア?ッ?する前に初期化されることが保証されている為、
 * (http://msdn.microsoft.com/ja-jp/library/988ye33t(VS.80).aspx)
 * mutex の初期化は問題ない。
 * ただし、DLL を使用しないプロジェクトではこの処理自体が無駄になる為、小さな DLL が見つからない場合にはこれらの処理を行わないようにする。
 * 
 *
 * <h3>◎欠?：</h3>
 * 小さなDLLを exe に付属させなければならない。
 * グロ?バル変数の mutex を使用する為、あらゆる?の初期化処理が共通してロックされる。
 * また、type_info ?造体には operator== しかないので map が使えず、vector による全検索を行う。
 * この為、static 変数の初期化時のみパフォ??ンスが?牲になる。
 * なお type_info::name 関数は?の比較に用いることはできない。
 * 無名名前空間を使って重複する?名を定?した場合、type_info::name 関数では両者を区別できない。
 *
 * <h3>・サンプルコ?ド</h3>
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
	/// シングルトンインス?ンスの取得
	static T& get() {
		static T* instance = nullptr;

		if (!instance) {
			// この関数は複数回実行されてもかまわない
			instance = static_cast<T*>(::balor::detail::getSingletonInstance(typeid(T), Singleton<T>::createInstance));
			// lock にメモリバリアが含まれるのでメモリ最適化はされない
		}
		return *instance;
	}

	// ライブラリを?ル?スレッドで使わないし、DLL プロジェクトも使わないという場合はこの実装で良い。簡単なアプリケ?ションのほとんどに当てはまるだろう。
	// あるいは C++0x に完全に?拠したコンパイラであればこの実装で?ル?スレッドは問題が無くなる。
	//static T& get() {
	//	static T instance;
	//	return instance;
	//}


private: // 一切の生成、コピ?、破棄を禁ずる
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