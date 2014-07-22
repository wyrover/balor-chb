#pragma once


namespace balor {
	namespace test {



/**
 * コンストラクタ、デストラクタ、代入演算子の呼び出し回数を追跡する。
 * 
 * コンテナ相当のクラスのテストに用いる。
 * グローバル変数を使用しているのでマルチスレッドやDLLでの使用は考えて行うこと。
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