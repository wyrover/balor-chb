#pragma once


namespace balor {



/**
* 이 클래스를 상속하는 클래스의 카피를 금지한다.
*
* boost::noncopyable 과 같다. balor 라이브러리로는 카피를 금지하는 클래스라 하더라도 우변값 참조를 인수로 취하는 컨스트럭터, 대입연산자를 가지고 있기에
* 카피를 만들지 않고 함수의 반환값으로 사용하는 것이 가능하고, STL 컨테이너에 보관하는 것이 가능하며, 임의의 시점에 임시 오브젝트를 만들어 대입해서 초기화가 가능하다.
*
* 이런 코드의 구현은 정말로 하고 싶지 않지만, 특정 버전의 boost의 설치를 강제하고 싶지 않다.
*/
class NonCopyable {
protected:
	NonCopyable() {}
	~NonCopyable() {}
private:
	NonCopyable(const NonCopyable& );
	NonCopyable& operator=(const NonCopyable& );
};



}