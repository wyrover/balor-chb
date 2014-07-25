#pragma once


namespace balor {

namespace detail {
void* getSingletonInstance(const type_info& info, void* (*createInstanceFunction)());
}



/**
 * 싱글턴 패턴의 구현을 제공한다.
 *
 * 복제불가능한 글로벌변수를, 멀티쓰레드에서도 DLL을 통해서도 안전하게 관리한다.
 * 템플릿 인수로 넘기는 클래스는 싱글톤 클래스를 friend 선언한 후 컨스트럭터와 디스트럭터를 private으로 선언하는 것이 좋다.
 * DLL 프로젝트를 포함한 복수의 프로젝트에서 balor를 사용하는 경우, 실행파일과 동일한 디렉토리에 balor_singleton.dll 을 놓아 둘 것.
 * 이 DLL을 찾지 못하는 경우에는 DLL을 고려하지 않은 싱글턴이 된다.
 *
 * 주의！ DLL 내부에서 싱글턴에 처음으로 접근할 시, 싱글톤의 인스턴스 등록을 한 경우에는 그 DLL을 언로드하는 타이밍에 주의할 것.
 * 언로드한 시점에서 등록했던 인스턴스의 포인터가 무효화되기 때문에, 인스턴스를 등록했는지 하지 않았는지의 조사도 유효하지 않게 된다.
 *
 * C++ 의 싱글턴 구현에는 몇가지 문제점이 있어서, 각각 해결해야할 필요가 있다.
 * 
 * <h3> 문제１：static 라이브러리를 DLL에 링크하면 DLL마다 글로벌 변수 영역이 복제된다. </h3>
 * DLL마다 링크되기 때문에 이것은 당연해서, 글로벌 변수의 주소뿐만 아니라 함수 포인터도 DLL마다 다르다.
 * 만약 DLL이 한 개 있다고 하더라도 어플리케이션 exe 파일과 그걸 확장하는 플러그인 DLL로 같은 정적 라이브러리를 사용하는 경우 어플리케이션과 DLL에서 제각각 라이브러리가 링크된다.
 * 이 문제는 애초에 정적 라이브러리를 사용하지 않으면 해결되지만,
 * 여기서는 유저가 정적 라이브러리를 사용할 것인지 DLL을 사용할 것인지를 선택하고 싶을시 어떻게 해야할지를 고려했다.
 * 
 * <h3>◎문제２：static 변수의 초기화는 멀티쓰레드에서는 잘되지 않는다.</h3>
 * 스레드1이 초기에 함수를 호출해서 static 변수의 초기화 처리에 들어간 후,
 * 스레드2가 동시에 함수를 호출하면, 처리부분에 있어서 초기화 처리가 중복되어 진행될지, 혹은 초기화되었다고 판단해서 초기화를 스킵해서, 미초기화된 오브젝트가 되버릴 수 있다.
 * 이 문제의 해결책으로써 잘 알려진 것이 DLCP（The Double-Checked Locking Optimization Pattern）이지만
 * 이 방법은 환경에 의존한 방법을 사용하지 않는한 안전하지는 않다.
 *
 * http://www.nwcpp.org/Downloads/2004/DCLP_notes.pdf
 * http://d.hatena.ne.jp/yupo5656/20041011/p1
 *
 * DCLP 는 아래와 같은 코드이지만
 * <pre><code>
 * static T* instance = 0;
 * if (instance == 0) { // 라인 1
 *     mutex::scoped_lock lock(mutex);
 *     if (instance == 0) { // 다시 체크하지 않으면 이중으로 new 및 컨스트럭트될 소지가 있다.
 *         instance = new T(); // 라인 2
 *     }
 * }
 * </code></pre>
 * 문제는 라인2로, 여기서는 (1)T의 메모리 할당으로 (2)T의 컨스트럭터、(3)instance 변수에 대입, 총 3번의 처리를 수행하는데,
 * (1)→(2)→(3)의 순서로 처리가 된다면 문제는 발생하지 않지만 컴파일러의 최적화나 CPU의 병렬 실행에 의해 (1)->(3)->(2)처럼 실행될 수도 있다.
 * 그렇게 되면 라인1의 판단에서 초기화가 끝나지 않았는데 초기화되었다고 판단될 수 있다.
 * 문서에 있듯이 환경에 의존하는 방법으로 회피하는 것은 가능하지만 인스턴스의 초기화를 해결해도 mutex의 초기화에 대해서는 제자리 걸음이 되어 해결되지 않는다. 또한 문제 1도 여전히 남는다.
 *
 * <h3>●해결책:</h3>
 * 문제1：을 해결하기 위해, type_info구조체로의 포인터와 할당된 인스턴스로의 포인터 쌍을 
 * 자그마한 DLL 속에 보존하도록 'static 변수의 초기화 처리가 진행될때마다 이  DLL의 함수를 호출해서
 * type_info::oprator== 가 성립되는지, type_info 구조체의 포인터가 이미 등록되어 있는지를 조사해서,
 * 등록되어 있지 않은 경우는 인스턴스를 작성해서 새롭게 등록하고, 등록되어 있는 경우는 등록된 인스턴스 포인터를 반환하도록 한다.
 * DLL마다 따로 연결된 각각의 코드 내부에서 같은 형에 대해서 &typeid(형명)을 수행하면 제각각 다른 어드레스를 반환하지만,
 * 양쪽에서 typeid::operator== 을 수행하면 제대로 true를 돌려준다.
 * 이걸로 다른 DLL 사이에서도 동일한 어드레스를 돌려주는 것이 보증된다.
 * 또한 이 DLL 함수처리는 boost::mutex로 완전히 락을 거는 '여러번 실행되어도 문제가 없는 처리이기도 해서'
 *  락으로 메모리 배리어를 포함하기 위해 'DLCP같은 방법이 불필요하게 된다.
 * DLL의 글로벌 변수는 프로세스에 Attatch하기 전에 초기화되는 것이 보증되기 때문에,
 * (http://msdn.microsoft.com/ja-jp/library/988ye33t(VS.80).aspx)
 *  mutex의 초기화에는 문제가 없다.
 * 다만 'DLL을 사용하지 않는 프로젝트에서는 이 처리자체가 헛일이기 때문에 작은 DLL을 찾지 못하는 경우에는 이런 처리를 수행하지 않도록 한다.
 * 
 *
 * <h3>결점：</h3>
 * 작은 dll을 exe 파일에 붙이지 않으면 안된다.
 * 글로벌 변수 mutex를 사용하기 위해 모든 형의 초기화 처리가 공통으로 락된다.
 * 또한, type_info 구조체에는 operator== 밖에 없기 때문에 map을 사용하지 않고 vector로 전체 검색을 수행한다.
 * 이를 위해 static 변수의 초기화때만 퍼포먼스가 희생된다.
 * 덧붙여 type_info::name 함수는 형 비교에 사용하는 것은 가능하지 않다.
 * 이름없는 네임스페이스를 사용해서 중복되는 형명을 정의하는 경우, type_info::name 함수로는 양자를 식별할 수 없다.
 *
 * <h3>샘플코드</h3>
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
	/// 싱글턴 인스턴스의 취득
	static T& get() {
		static T* instance = nullptr;

		if (!instance) {
			// 이 함수는 여러번 사용되도 상관없다.
			instance = static_cast<T*>(::balor::detail::getSingletonInstance(typeid(T), Singleton<T>::createInstance));
			// lock에 메모리 배리어가 포함되어 있기 때문에 메모리 최적화는 되지 않는다.
		}
		return *instance;
	}

	// 라이브러리를 멀티쓰레드에서 사용하지 않고, DLL 프로젝트에서도 사용하지 않는 경우는 이 구현체로 좋다. 간단한 어플리케이션 대부분에 해당될 것이다.
	// 또는 C++0x을 완전히 준수한 컴파일러라면 이 구현체로 멀티쓰레드는 문제가 되지 않게 된다.
	//static T& get() {
	//	static T instance;
	//	return instance;
	//}


private: // 일체의 생성, 복사, 파괴를 금지한다.
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