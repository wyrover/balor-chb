#pragma once


namespace balor {
	namespace test {



/**
 * コンストラク?、デストラク?、ｴ・ﾔ演算子の呼び出し回数を追跡する。
 * 
 * コンテナ相当のクラスのテストに用いる。
 * グロ?バル変数を使用しているので?ル?スレッドやDLLでの使用は考えて行うこと。
 */
class InstanceTracer {
public:
	InstanceTracer();
	InstanceTracer(const InstanceTracer& value);
	InstanceTracer(InstanceTracer&& value);
	~InstanceTracer();

	InstanceTracer& operator=(const InstanceTracer& value);
	InstanceTracer& operator=(InstanceTracer&& value);

public:
	static int defaultConstructCount();
	static int copyConstructCount();
	static int rvalueConstructCount();
	static int destructCount();
	static int operatorEqualCount();
	static int rvalueOperatorEqualCount();

public:
	static void clearAllCount();
	static bool checkAllCount(int defaultConstructCount
							 ,int copyConstructCount
							 ,int rvalueConstructCount
							 ,int destructCount
							 ,int operatorEqualCount = 0
							 ,int rvalueOperatorEqualCount = 0);
};



	}
}