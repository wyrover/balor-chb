#include <balor/io/Registry.hpp>


#include <algorithm>
#include <cstdint>
#include <vector>
#pragma warning(push, 3)
#include <boost/assign/std/vector.hpp>
#pragma warning(pop)

#include <balor/system/EnvironmentVariable.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/HandleLeakChecker.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/scopeExit.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace io {
		namespace testRegistry {

using std::vector;
using std::move;
using namespace boost::assign;
using namespace balor::system;


namespace {
const wchar_t testKeyName[] = L"testBalor_io_Registry_a6rui97onfk1678kklhu8i";


Registry getTestKeyParent() {
	return Registry::currentUser().openKey(L"Software");
}


void testKeyDeleteFunction() {
	getTestKeyParent().removeKey(testKeyName, true);
};


Registry getTestKey() {
	testKeyDeleteFunction();
	return getTestKeyParent().createKey(testKeyName, true);
}


bool findExistKeyAndValue(const Registry& key) {
	if (0 < key.valueCount()) {
		auto names = key.valueNamesIterator();
		if (names) {
			key.getBinary(*names);
			return true;
		}
	}
	if (0 < key.keyCount()) {
		auto names = key.keyNamesIterator();
		for (; names; ++names) {
			auto subKey = key.openKey(*names);
			if (findExistKeyAndValue(subKey)) {
				return true;
			}
		}
	}
	return false;
}
} // namespace


testCase(rootKeys) {
	testAssert(0 < Registry::classesRoot().keyCount());
	testAssert(0 < Registry::currentConfig().keyCount());
	testAssert(0 < Registry::currentUser().keyCount());
	testAssert(0 < Registry::localMachine().keyCount());
	testAssert(0 < Registry::users().keyCount());
}


testCase(constructAndAssignment) {
	{// デフォルトコンストラクタ
		Registry emptyKey;
		testAssert(!emptyKey);
	}
	{// rvalue コンストラクタ
		Registry sourceKey = Registry::currentUser();
		Registry rvalueConstructKey = move(sourceKey);
		testAssert(!sourceKey);
		testAssert(rvalueConstructKey.keyCount() == Registry::currentUser().keyCount());
	}
	{// rvalue 代入演算子
		Registry sourceKey = Registry::currentUser();
		Registry rvalueAssignmentKey;
		rvalueAssignmentKey = move(sourceKey);
		testAssert(!sourceKey);
		testAssert(rvalueAssignmentKey.keyCount() == Registry::currentUser().keyCount());
	}
	{// コンテナ格納
		vector<Registry> keys;
		keys.push_back(Registry::classesRoot());
		keys.push_back(Registry::currentConfig());
		keys.push_back(Registry::currentUser());
		keys.pop_back();
		keys.insert(++keys.begin(), Registry::localMachine());
		keys.erase(++++keys.begin());
		testAssert((HKEY)keys[0] == (HKEY)Registry::classesRoot());
		testAssert((HKEY)keys[1] == (HKEY)Registry::localMachine());
	}
}


testCase(constructWithName) {
	{ // 無効なルートキー名
		testAssertionFailed(Registry(L"\\"));
		testAssertionFailed(Registry(L""));
	}
	{ // 存在しないサブキー名
		testAssert(!Registry(L"HKEY_CLASSES_ROOT\\a\\b\\c\\d\\e0123456"));
	}
	// それぞれのルートキーから値の取得
	testAssert(findExistKeyAndValue(Registry::classesRoot()));
	testAssert(findExistKeyAndValue(Registry::currentConfig()));
	testAssert(findExistKeyAndValue(Registry::currentUser()));
	testAssert(findExistKeyAndValue(Registry::localMachine()));
	testAssert(findExistKeyAndValue(Registry::users()));

	{// 環境変数で取得した値の内容をテスト
		String path;
		testNoThrow(path = EnvironmentVariable::get(L"PATH"));
		auto value = Registry(L"HKEY_LOCAL_MACHINE\\System\\CurrentControlSet\\Control\\Session Manager\\Environment").getString(L"PATH");
		value = EnvironmentVariable::expandStrings(value);
		testAssert(path.startsWith(value));
	}
}


testCase(destruct) {
	balor::test::HandleLeakChecker checker;
	{
		auto key = Registry::currentUser().openKey(L"Software");
		testAssert(checker.getSystemHandleChange() == 1);
	}
	testAssert(checker.getSystemHandleChange() == 0);
}


testCase(createKey) {
	{// 空のレジストリキー
		Registry emptyKey;
		testAssertionFailed(emptyKey.createKey(L" "));
	}
	// サブキーの作成を許されないレジストリキー
	testThrow(Registry::localMachine().createKey(testKeyName), Registry::InvalidParameterException);

	// テスト用レジストリキーの準備
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 無効なパラメータ
	testAssertionFailed(testKey.createKey(L""));
	// サブキーの作成
	Registry subKey;
	testNoThrow(subKey = testKey.createKey(L"subKey"));
	testAssert(subKey);
	// デフォルト引数では書き込み権限無し
	testThrow(subKey.setDword(L"dwordValue", std::uint32_t(0)), Registry::AccessDeniedException);
	// 既に存在するキーはそのまま開く。ついでに書き込み権限付与。
	testNoThrow(subKey = testKey.createKey(L"subKey", true));
	testAssert(subKey);
	// 書き込み権限有り
	testNoThrow(subKey.setDword(L"dwordValue", std::uint32_t(2)));
	testAssert(subKey.getDword(L"dwordValue") == 2);
	// 既に削除されたレジストリキーで作成しようとした
	testKey.removeKey(L"subKey");
	testAssert(!testKey.openKey(L"subKey"));
	testThrow(subKey.createKey(L"subsubKey"), Registry::KeyDeletedException);
	// 複数の階層を一気に作成
	testNoThrow(subKey = testKey.createKey(L"subKey\\subsubKey\\subsubsubKey"));
	testAssert(testKey.openKey(L"subKey").openKey(L"subsubKey").openKey(L"subsubsubKey"));
	// 途中まで存在する複数の階層を一気に作成
	testNoThrow(subKey = testKey.createKey(L"subKey\\hogeKey\\hogehogeKey"));
	testAssert(testKey.openKey(L"subKey").openKey(L"hogeKey").openKey(L"hogehogeKey"));

	// Caution: RegistryOptions::volatileSubTree のテストはPCのログオフや再起動が必要な為、
	//          下記のサブキーが作成され、ログオフ後に消えることを目視で確認する。
	//Registry::currentUser().openKey(L"Software").createKey(L"testBalor", true, RegistryOptions::volatileSubTree);
}


testCase(flush) {
	{// 空のレジストリキー
		Registry emptyKey;
		testAssertionFailed(emptyKey.flush());
	}

	// テスト用レジストリキーの準備
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	testKey.setString(L"value", L"test");
	testNoThrow(testKey.flush()); // 効果をどう判定すれば良いのか判らない
}


testCase(getValue) {
	{// 空のレジストリキー
		Registry emptyKey;
		testAssertionFailed(emptyKey.getString(L" "));
	}

	// テスト用レジストリキーの準備
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	testKey.setString(L"string0", L"test");
	testKey.setDword(L"dword0", 0);

	{// binaryの取得
		testThrow(testKey.getBinary(L"test"), Registry::ValueNotFoundException);
		vector<std::uint8_t> binary;
		testNoThrow(testKey.setBinary(L"binary", binary));
		testAssert(testKey.getBinary(L"binary") == binary);
		testAssert(testKey.getValueKind(L"binary") == Registry::ValueKind::binary);
		binary += 0, 1, 2, 3;
		testNoThrow(testKey.setBinary(L"binary", binary));
		testAssert(testKey.getBinary(L"binary") == binary);
		binary.clear();
		const wchar_t text[] = L"test";
		binary.assign(reinterpret_cast<const unsigned char*>(text), reinterpret_cast<const unsigned char*>(text + 5));
		testAssert(testKey.getBinary(L"string0") == binary);
	}
	{// dwordの取得
		testThrow(testKey.getDword(L"test"), Registry::ValueNotFoundException);
		testThrow(testKey.getDword(L"string0"), Registry::ValueKindMismatchException);
		std::uint32_t dword = 0;
		testNoThrow(testKey.setDword(L"dword", dword));
		testAssert(testKey.getDword(L"dword") == dword);
		testAssert(testKey.getValueKind(L"dword") == Registry::ValueKind::dword);
		testNoThrow(testKey.setDword(L"dword2", dword));
		testAssert(testKey.getDword(L"dword2") == dword);
		testAssert(testKey.getValueKind(L"dword2") == Registry::ValueKind::dword);
		dword = 1024;
		testNoThrow(testKey.setDword(L"dword", dword));
		testAssert(testKey.getDword(L"dword") == dword);
		testNoThrow(testKey.setDword(L"dword2", dword));
		testAssert(testKey.getDword(L"dword2") == dword);
	}
	{// stringの取得
		testThrow(testKey.getString(L"test"), Registry::ValueNotFoundException);
		testThrow(testKey.getString(L"dword0"), Registry::ValueKindMismatchException);
		String string;
		testNoThrow(testKey.setString(L"string", string));
		testAssert(testKey.getString(L"string") == string);
		testAssert(testKey.getValueKind(L"string") == Registry::ValueKind::string);
		string = L"abc";
		testNoThrow(testKey.setString(L"string", string));
		testAssert(testKey.getString(L"string") == string);
	}
	{// expandStringの取得
		String expandString;
		testNoThrow(testKey.setString(L"expandString", expandString, Registry::ValueKind::expandString));
		testAssert(testKey.getString(L"expandString") == expandString);
		testAssert(testKey.getValueKind(L"expandString") == Registry::ValueKind::expandString);
		expandString = L"%PATH%";
		testNoThrow(testKey.setString(L"expandString", expandString, Registry::ValueKind::expandString));
		testAssert(testKey.getString(L"expandString") == expandString);
	}
	{// multiStringの取得
		String multiString;
		testNoThrow(testKey.setString(L"multiString", multiString, Registry::ValueKind::multiString));
		testAssert(testKey.getString(L"multiString") == multiString);
		testAssert(testKey.getValueKind(L"multiString") == Registry::ValueKind::multiString);
		multiString = String(L"a\0b\0c", 5);
		testNoThrow(testKey.setString(L"multiString", multiString, Registry::ValueKind::multiString));
		testAssert(testKey.getString(L"multiString") == multiString);
	}
	{// qwordの取得
		testThrow(testKey.getQword(L"test"), Registry::ValueNotFoundException);
		testThrow(testKey.getQword(L"string0"), Registry::ValueKindMismatchException);
		std::uint64_t qword = 0;
		testNoThrow(testKey.setQword(L"qword", qword));
		testAssert(testKey.getQword(L"qword") == qword);
		testAssert(testKey.getValueKind(L"qword") == Registry::ValueKind::qword);
		testNoThrow(testKey.setQword(L"qword2", qword));
		testAssert(testKey.getQword(L"qword2") == qword);
		testAssert(testKey.getValueKind(L"qword2") == Registry::ValueKind::qword);
		qword = 2048;
		testNoThrow(testKey.setQword(L"qword", qword));
		testAssert(testKey.getQword(L"qword") == qword);
		testNoThrow(testKey.setQword(L"qword2", qword));
		testAssert(testKey.getQword(L"qword2") == qword);
	}
	{// 既定値の取得
		String string = L"test";
		testNoThrow(testKey.setString(L"", string));
		testAssert(testKey.getString(L"") == string);
		testAssert(testKey.getValueKind(L"") == Registry::ValueKind::string);
	}
	{// サポートしない型の値の取得
		DWORD value = 1;
		testAssert(RegSetValueExW(testKey, L"unknown", 0, REG_DWORD_BIG_ENDIAN, reinterpret_cast<const BYTE*>(&value), sizeof(value)) == ERROR_SUCCESS);
		vector<std::uint8_t> binary;
		binary += 1, 0, 0, 0;
		testAssert(testKey.getBinary(L"unknown") == binary);
		testAssert(testKey.getValueKind(L"unknown") == Registry::ValueKind::unknown);
	}
	{// 削除されたキーから値の取得
		Registry subKey = testKey.createKey(L"subKey", true);
		subKey.setString(L"value", L"testValue");
		testKey.removeKey(L"subKey");
		testThrow(subKey.getString(L"value"), Registry::KeyDeletedException);
		testThrow(subKey.getValueKind(L"value"), Registry::KeyDeletedException);
	}
	{// 値の読み取り権限のないキーから値を取得
		testKey.createKey(L"subKey", true).setString(L"value", L"test");
		HKEY handle;
		testAssert(RegOpenKeyExW(testKey, L"subKey", 0, STANDARD_RIGHTS_READ, &handle) == ERROR_SUCCESS);
		Registry subKey;
		*(HKEY*)&subKey = handle;
		testThrow(subKey.getString(L"value"), Registry::AccessDeniedException);
		testThrow(subKey.getValueKind(L"value"), Registry::AccessDeniedException);
	}
}


testCase(getValueKind) {
	{// 空のレジストリキー
		Registry emptyKey;
		testAssertionFailed(emptyKey.getValueKind(L" "));
	}

	// テスト用レジストリキーの準備
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 存在しない値
	testAssert(testKey.getValueKind(L"value") == Registry::ValueKind::notFound);
	// 各型のテストは testCase(getValue) にてテスト済み

	// 削除されたキーから値の型の取得は testCase(getValue) にてテスト済み

	// 値の読み取り権限のないキーから型の取得は testCase(getValue) にてテスト済み
}


testCase(name) {
	// testCase(createKey)にてテスト済み
}


testCase(keyCount) {
	{// 空のレジストリキー
		Registry emptyKey;
		testAssertionFailed(emptyKey.keyCount());
	}

	// テスト用レジストリキーの準備
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// サブキーの数
	testAssert(testKey.keyCount() == 0);
	testKey.createKey(L"subKey0");
	testKey.createKey(L"subKey1");
	testKey.createKey(L"subKey2");
	testAssert(testKey.keyCount() == 3);

	{// 削除されたレジストリキーからサブキーの数を取得
		Registry subKey = testKey.createKey(L"subKey");
		testKey.removeKey(L"subKey");
		testThrow(subKey.keyCount(), Registry::KeyDeletedException);
	}
	{// 権限の無いキーでのサブキーの数の取得
		testKey.createKey(L"subKey");
		HKEY handle;
		testAssert(RegOpenKeyExW(testKey, L"subKey", 0, STANDARD_RIGHTS_READ, &handle) == ERROR_SUCCESS);
		Registry subKey;
		*(HKEY*)&subKey = handle;
		testThrow(subKey.keyCount(), Registry::AccessDeniedException);
	}
}


testCase(keyNamesIterator) {
	{// 空のレジストリキー
		Registry emptyKey;
		testAssertionFailed(emptyKey.keyNamesIterator());
	}

	// テスト用レジストリキーの準備
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 空のサブキー名リストの取得
	testAssert(!testKey.keyNamesIterator());
	{// サブキー名リストの取得
		wchar_t names[][8] = {L"subKey0", L"subKey1", L"subKey2"};
		for (int i = 0; i < 3; ++i) {
			testKey.createKey(names[i]);
		}
		auto subKeyNames = testKey.keyNamesIterator();
		for (int i = 0; i < 3; ++i, ++subKeyNames) {
			bool find = false;
			for (int j = 0; j < 3; ++j) {
				if (String::equals(names[j], *subKeyNames)) {
					find = true;
					break;
				}
			}
			testAssert(find);
		}
		testAssert(!subKeyNames);
	}
	{// 削除されたレジストリキーからサブキー名リストの取得
		Registry subKey = testKey.createKey(L"subKey");
		testKey.removeKey(L"subKey");
		testThrow(subKey.keyNamesIterator(), Registry::KeyDeletedException);
	}
	{// サブキー名の列挙権限の無いキーでの列挙
		testKey.createKey(L"subKey");
		HKEY handle;
		testAssert(RegOpenKeyExW(testKey, L"subKey", 0, STANDARD_RIGHTS_READ, &handle) == ERROR_SUCCESS);
		Registry subKey;
		*(HKEY*)&subKey = handle;
		testThrow(subKey.keyNamesIterator(), Registry::AccessDeniedException);
	}
}


testCase(openKey) {
	{// 空のレジストリキー
		Registry emptyKey;
		testAssertionFailed(emptyKey.openKey(L" "));
	}

	// テスト用レジストリキーの準備
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 無効なパラメータ
	testAssertionFailed(testKey.openKey(L""));
	// 存在しないサブキー
	testAssert(!testKey.openKey(L"subKey"));
	{// 存在するサブキー
		Registry createdSubKey = testKey.createKey(L"subKey");
		Registry subKey = testKey.openKey(L"subKey");
		testAssert(subKey);
	}
	// 書き込み権限なし
	testThrow(testKey.openKey(L"subKey").setString(L"value", L"test"), Registry::AccessDeniedException);
	// 書き込み権限あり
	testNoThrow(testKey.openKey(L"subKey", true).setString(L"value", L"test"));
	{// 削除されたキーでサブキーをオープン
		Registry subKey = testKey.openKey(L"subKey");
		testKey.removeKey(L"subKey");
		testThrow(subKey.openKey(L"subsubKey"), Registry::KeyDeletedException);
	}
	// Caution: 現在のユーザの読み取りを拒否したキーを作成し、オープンするとRegistry::AccessDeniedExceptionを投げることを目視で確認
	//Registry::currentUser().openKey(L"Software\\balor_test");
}


testCase(removeKey) {
	{// 空のレジストリキー
		Registry emptyKey;
		testAssertionFailed(emptyKey.removeKey(L" "));
	}

	// 削除が許されないキー
	testThrow(Registry::localMachine().removeKey(L"SECURITY"), Registry::AccessDeniedException);

	// テスト用レジストリキーの準備
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 無効なパラメータ
	testAssertionFailed(testKey.removeKey(L""));
	// 存在しないサブキーの削除
	testNoThrow(testKey.removeKey(L"subKey"));
	// 存在するサブキーの削除
	testKey.createKey(L"subKey");
	testNoThrow(testKey.removeKey(L"subKey"));
	testAssert(!testKey.openKey(L"subKey"));
	// サブキーが存在するキーの削除
	Registry subKey = testKey.createKey(L"subKey");
	Registry subsubKey = subKey.createKey(L"subsubKey", true);
	testThrow(testKey.removeKey(L"subKey"), Registry::AccessDeniedException);
	testAssert(testKey.openKey(L"subKey"));
	// 値はあっても削除できる
	subsubKey.setString(L"value", L"test");
	testNoThrow(subKey.removeKey(L"subsubKey"));
	testAssert(!subKey.openKey(L"subsubKey"));
	// 削除されたキーで削除を実行
	subsubKey = subKey.createKey(L"subsubKey");
	testKey.removeKey(L"subKey", true);
	testAssert(!testKey.openKey(L"subKey"));
	testThrow(subKey.removeKey(L"subsubKey"), Registry::KeyDeletedException);
}


testCase(removeKeyRecursive) {
	// テスト用レジストリキーの準備
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 存在しないサブキーの削除
	testNoThrow(testKey.removeKey(L"subKey", true));
	// 存在するサブキーの削除
	Registry subKey = testKey.createKey(L"subKey");
	subKey.createKey(L"subsubKey");
	subKey.createKey(L"subsubKey2");
	Registry subKey2 = testKey.createKey(L"subKey2");
	subKey2.createKey(L"subsubKey2");
	subKey2.createKey(L"subsubKey22");
	testNoThrow(testKey.removeKey(L"subKey", true));
	testAssert(!testKey.openKey(L"subKey"));
}


testCase(removeValue) {
	{// 空のレジストリキー
		Registry emptyKey;
		testAssertionFailed(emptyKey.removeValue(L" "));
	}

	// テスト用レジストリキーの準備
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 存在しない値の削除
	testNoThrow(testKey.removeValue(L"value"));
	// 存在する値の削除
	Registry subKey = testKey.createKey(L"subKey", true);
	subKey.setString(L"value", L"test");
	testAssert(subKey.getString(L"value") == L"test");
	testNoThrow(subKey.removeValue(L"value"));
	testThrow(subKey.getString(L"value"), Registry::ValueNotFoundException);
	// 既定値の削除
	subKey.setString(L"", L"default");
	testAssert(subKey.getString(L"") == L"default");
	testNoThrow(subKey.removeValue(L""));
	testThrow(subKey.getString(L""), Registry::ValueNotFoundException);
	// 書き込み権限の無いキーで値の削除
	subKey.setString(L"value", L"test");
	subKey = testKey.openKey(L"subKey");
	testThrow(subKey.removeValue(L"value"), Registry::AccessDeniedException);
	testAssert(subKey.getString(L"value") == L"test");
	// 削除されたキーで値の削除
	subKey = testKey.openKey(L"subKey", true);
	testKey.removeKey(L"subKey");
	testThrow(subKey.removeValue(L"value"), Registry::KeyDeletedException);
}


testCase(setValue) {
	{// 空のレジストリキー
		Registry emptyKey;
		testAssertionFailed(emptyKey.setString(L"", L""));
		testAssertionFailed(emptyKey.setBinary(L"", vector<std::uint8_t>()));
		testAssertionFailed(emptyKey.setDword(L"", uint32_t()));
		testAssertionFailed(emptyKey.setQword(L"", uint64_t()));
	}

	// テスト用レジストリキーの準備
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 存在しない値は作成される
	testKey.setString(L"value", L"test");
	testAssert(testKey.getString(L"value") == L"test");
	// 存在する値は上書きされる
	testKey.setString(L"value", L"test2");
	testAssert(testKey.getString(L"value") == L"test2");

	// 値と型のミスマッチ
	testAssertionFailed(testKey.setString(L"value", String(), Registry::ValueKind::binary));

	// これ以外の各型の値の設定のテストは testCase(getValue) にてテスト済み

	{// 削除されたキーに値の設定
		Registry subKey = testKey.createKey(L"subKey", true);
		testKey.removeKey(L"subKey");
		testThrow(subKey.setString(L"value", L""), Registry::KeyDeletedException);
		testThrow(subKey.setBinary(L"value", vector<std::uint8_t>()), Registry::KeyDeletedException);
		testThrow(subKey.setDword(L"value", uint32_t()), Registry::KeyDeletedException);
		testThrow(subKey.setQword(L"value", uint64_t()), Registry::KeyDeletedException);
	}

	{// 書き込み権限のないキーに値の設定
		Registry subKey = testKey.createKey(L"subKey");
		testThrow(subKey.setString(L"value", L""), Registry::AccessDeniedException);
		testThrow(subKey.setBinary(L"value", vector<std::uint8_t>()), Registry::AccessDeniedException);
		testThrow(subKey.setDword(L"value", uint32_t()), Registry::AccessDeniedException);
		testThrow(subKey.setQword(L"value", uint64_t()), Registry::AccessDeniedException);
	}
}


testCase(valueCount) {
	{// 空のレジストリキー
		Registry emptyKey;
		testAssertionFailed(emptyKey.valueCount());
	}

	// テスト用レジストリキーの準備
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 値の数
	testAssert(testKey.valueCount() == 0);
	testKey.setString(L"value0", L"test");
	testKey.setString(L"value1", L"test");
	testKey.setString(L"value2", L"test");
	testAssert(testKey.valueCount() == 3);
	// 規定値
	testKey.setString(L"", L"test");
	testAssert(testKey.valueCount() == 4);

	{// 削除されたレジストリキーから値の数を取得
		Registry subKey = testKey.createKey(L"subKey");
		testKey.removeKey(L"subKey");
		testThrow(subKey.valueCount(), Registry::KeyDeletedException);
	}
	{// 権限の無いキーでの値の取得
		testKey.createKey(L"subKey");
		HKEY handle;
		testAssert(RegOpenKeyExW(testKey, L"subKey", 0, STANDARD_RIGHTS_READ, &handle) == ERROR_SUCCESS);
		Registry subKey;
		*(HKEY*)&subKey = handle;
		testThrow(subKey.valueCount(), Registry::AccessDeniedException);
	}
}


testCase(valueNamesIterator) {
	{// 空のレジストリキー
		Registry emptyKey;
		testAssertionFailed(emptyKey.valueNamesIterator());
	}

	// テスト用レジストリキーの準備
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 空の値名リストの取得
	testAssert(!testKey.valueNamesIterator());
	{// 値名リストの取得
		wchar_t names[][8] = {L"value0", L"value1", L""};
		for (int i = 0; i < 3; ++i) {
			testKey.setString(names[i], L"test");
		}
		auto valueNames = testKey.valueNamesIterator();
		for (int i = 0; i < 3; ++i, ++valueNames) {
			bool find = false;
			for (int j = 0; j < 3; ++j) {
				if (String::equals(names[j], *valueNames)) {
					find = true;
					break;
				}
			}
			testAssert(find);
		}
		testAssert(!valueNames);
	}
	{// 削除されたレジストリキーから値名リストの取得
		Registry subKey = testKey.createKey(L"subKey");
		testKey.removeKey(L"subKey");
		testThrow(subKey.valueNamesIterator(), Registry::KeyDeletedException);
	}
	{// サブキー名の列挙権限の無いキーでの列挙
		testKey.createKey(L"subKey");
		HKEY handle;
		testAssert(RegOpenKeyExW(testKey, L"subKey", 0, STANDARD_RIGHTS_READ, &handle) == ERROR_SUCCESS);
		Registry subKey;
		*(HKEY*)&subKey = handle;
		testThrow(subKey.valueNamesIterator(), Registry::AccessDeniedException);
	}
}




		}
	}
}