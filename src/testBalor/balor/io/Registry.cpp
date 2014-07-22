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
	{// 기본 생성자
		Registry emptyKey;
		testAssert(!emptyKey);
	}
	{// rvalue 생성자
		Registry sourceKey = Registry::currentUser();
		Registry rvalueConstructKey = move(sourceKey);
		testAssert(!sourceKey);
		testAssert(rvalueConstructKey.keyCount() == Registry::currentUser().keyCount());
	}
	{// rvalue 대입 연산자
		Registry sourceKey = Registry::currentUser();
		Registry rvalueAssignmentKey;
		rvalueAssignmentKey = move(sourceKey);
		testAssert(!sourceKey);
		testAssert(rvalueAssignmentKey.keyCount() == Registry::currentUser().keyCount());
	}
	{// 컨테이너 저장
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
	{ // 무효한 루트 키 이름
		testAssertionFailed(Registry(L"\\"));
		testAssertionFailed(Registry(L""));
	}
	{ // 존재하지 않은 루트 키 이름
		testAssert(!Registry(L"HKEY_CLASSES_ROOT\\a\\b\\c\\d\\e0123456"));
	}
	// 각각 루티 키에서 값을 취득
	testAssert(findExistKeyAndValue(Registry::classesRoot()));
	testAssert(findExistKeyAndValue(Registry::currentConfig()));
	testAssert(findExistKeyAndValue(Registry::currentUser()));
	testAssert(findExistKeyAndValue(Registry::localMachine()));
	testAssert(findExistKeyAndValue(Registry::users()));

	{// 환경 변수에서 취득한 내용을 테스트
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
	{// 빈 레지스트키
		Registry emptyKey;
		testAssertionFailed(emptyKey.createKey(L" "));
	}
	// 서브 키 작성을 허용하지 않는 레지스트 키
	testThrow(Registry::localMachine().createKey(testKeyName), Registry::InvalidParameterException);

	// 테스트 용 레지스트 키 준비
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 무효한 파라미터
	testAssertionFailed(testKey.createKey(L""));
	// 서브 키 작성
	Registry subKey;
	testNoThrow(subKey = testKey.createKey(L"subKey"));
	testAssert(subKey);
	// 기본 이순에서는 쓰기 권한 없음
	testThrow(subKey.setDword(L"dwordValue", std::uint32_t(0)), Registry::AccessDeniedException);
	// 이미 존재하는 키를 그 대로 열고 바로 쓰기 권한 부여
	testNoThrow(subKey = testKey.createKey(L"subKey", true));
	testAssert(subKey);
	// 쓰기 권한만 있음
	testNoThrow(subKey.setDword(L"dwordValue", std::uint32_t(2)));
	testAssert(subKey.getDword(L"dwordValue") == 2);
	// 이미 삭제된 레지스트 키로 작성해본다
	testKey.removeKey(L"subKey");
	testAssert(!testKey.openKey(L"subKey"));
	testThrow(subKey.createKey(L"subsubKey"), Registry::KeyDeletedException);
	// 복수의 계층을 한번에 작성
	testNoThrow(subKey = testKey.createKey(L"subKey\\subsubKey\\subsubsubKey"));
	testAssert(testKey.openKey(L"subKey").openKey(L"subsubKey").openKey(L"subsubsubKey"));
	// 도중까지 존재하는 복수의 계층을 한번에 작성
	testNoThrow(subKey = testKey.createKey(L"subKey\\hogeKey\\hogehogeKey"));
	testAssert(testKey.openKey(L"subKey").openKey(L"hogeKey").openKey(L"hogehogeKey"));

	
}


testCase(flush) {
	{// 빈 레지스트 키
		Registry emptyKey;
		testAssertionFailed(emptyKey.flush());
	}

	// 테스트 용 레지스트 키 준비
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	testKey.setString(L"value", L"test");
	testNoThrow(testKey.flush()); 
}


testCase(getValue) {
	{// 빈 레지스트 키
		Registry emptyKey;
		testAssertionFailed(emptyKey.getString(L" "));
	}

	// 테스트 용 레지스트 키 준비
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	testKey.setString(L"string0", L"test");
	testKey.setDword(L"dword0", 0);

	{// binary 취득
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
	{// dword 취득
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
	{// string 취득
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
	{// expandString 취득
		String expandString;
		testNoThrow(testKey.setString(L"expandString", expandString, Registry::ValueKind::expandString));
		testAssert(testKey.getString(L"expandString") == expandString);
		testAssert(testKey.getValueKind(L"expandString") == Registry::ValueKind::expandString);
		expandString = L"%PATH%";
		testNoThrow(testKey.setString(L"expandString", expandString, Registry::ValueKind::expandString));
		testAssert(testKey.getString(L"expandString") == expandString);
	}
	{// multiString 취득
		String multiString;
		testNoThrow(testKey.setString(L"multiString", multiString, Registry::ValueKind::multiString));
		testAssert(testKey.getString(L"multiString") == multiString);
		testAssert(testKey.getValueKind(L"multiString") == Registry::ValueKind::multiString);
		multiString = String(L"a\0b\0c", 5);
		testNoThrow(testKey.setString(L"multiString", multiString, Registry::ValueKind::multiString));
		testAssert(testKey.getString(L"multiString") == multiString);
	}
	{// qword 취득
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
	{// 기본 값 취득
		String string = L"test";
		testNoThrow(testKey.setString(L"", string));
		testAssert(testKey.getString(L"") == string);
		testAssert(testKey.getValueKind(L"") == Registry::ValueKind::string);
	}
	{// 지원하지 않는 형의 값 취득
		DWORD value = 1;
		testAssert(RegSetValueExW(testKey, L"unknown", 0, REG_DWORD_BIG_ENDIAN, reinterpret_cast<const BYTE*>(&value), sizeof(value)) == ERROR_SUCCESS);
		vector<std::uint8_t> binary;
		binary += 1, 0, 0, 0;
		testAssert(testKey.getBinary(L"unknown") == binary);
		testAssert(testKey.getValueKind(L"unknown") == Registry::ValueKind::unknown);
	}
	{// 삭제된 키에서 값을 취득
		Registry subKey = testKey.createKey(L"subKey", true);
		subKey.setString(L"value", L"testValue");
		testKey.removeKey(L"subKey");
		testThrow(subKey.getString(L"value"), Registry::KeyDeletedException);
		testThrow(subKey.getValueKind(L"value"), Registry::KeyDeletedException);
	}
	{// 값 일기 권한이 없는 키에서 값 취득
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
	{// 빈 레지스트 키
		Registry emptyKey;
		testAssertionFailed(emptyKey.getValueKind(L" "));
	}

	// 테스트 용 레지스트 키 준비
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 존재하지 않는 값
	testAssert(testKey.getValueKind(L"value") == Registry::ValueKind::notFound);
	
}


testCase(name) {
	// testCase(createKey) 테스트 완료
}


testCase(keyCount) {
	{// 빈 레지스트 키
		Registry emptyKey;
		testAssertionFailed(emptyKey.keyCount());
	}

	// 테스트 용 레지스트 키 준비
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 긖긳긌?궻릶
	testAssert(testKey.keyCount() == 0);
	testKey.createKey(L"subKey0");
	testKey.createKey(L"subKey1");
	testKey.createKey(L"subKey2");
	testAssert(testKey.keyCount() == 3);

	{// 삭제된 레지스트 키에서 서브 키 수를 취득
		Registry subKey = testKey.createKey(L"subKey");
		testKey.removeKey(L"subKey");
		testThrow(subKey.keyCount(), Registry::KeyDeletedException);
	}
	{// 권한 없는 키에서 서브 키 수 취득
		testKey.createKey(L"subKey");
		HKEY handle;
		testAssert(RegOpenKeyExW(testKey, L"subKey", 0, STANDARD_RIGHTS_READ, &handle) == ERROR_SUCCESS);
		Registry subKey;
		*(HKEY*)&subKey = handle;
		testThrow(subKey.keyCount(), Registry::AccessDeniedException);
	}
}


testCase(keyNamesIterator) {
	{// 빈 레지스트 키
		Registry emptyKey;
		testAssertionFailed(emptyKey.keyNamesIterator());
	}

	// 테스트 용 레지스트 키 준비
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 빈 서브 키 이름 리스트 취득
	testAssert(!testKey.keyNamesIterator());
	{// 서브 키 이름 리스트 취득
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
	{// 삭제된 레지스트 키에서 서비 키 이름 리스트 취득
		Registry subKey = testKey.createKey(L"subKey");
		testKey.removeKey(L"subKey");
		testThrow(subKey.keyNamesIterator(), Registry::KeyDeletedException);
	}
	{// 서브 키 이름 열거 권한 없는 키로 열거
		testKey.createKey(L"subKey");
		HKEY handle;
		testAssert(RegOpenKeyExW(testKey, L"subKey", 0, STANDARD_RIGHTS_READ, &handle) == ERROR_SUCCESS);
		Registry subKey;
		*(HKEY*)&subKey = handle;
		testThrow(subKey.keyNamesIterator(), Registry::AccessDeniedException);
	}
}


testCase(openKey) {
	{// 빈 레지스트 키
		Registry emptyKey;
		testAssertionFailed(emptyKey.openKey(L" "));
	}

	// 테스트 용 레지스트 키 준비
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 무효한 파라미터
	testAssertionFailed(testKey.openKey(L""));
	// 존재하지 않는 서브 키
	testAssert(!testKey.openKey(L"subKey"));
	{// 존재하는 서브 키
		Registry createdSubKey = testKey.createKey(L"subKey");
		Registry subKey = testKey.openKey(L"subKey");
		testAssert(subKey);
	}
	// 쓰기 권한 없음
	testThrow(testKey.openKey(L"subKey").setString(L"value", L"test"), Registry::AccessDeniedException);
	// 쓰기 권한 있음
	testNoThrow(testKey.openKey(L"subKey", true).setString(L"value", L"test"));
	{// 삭제된 키에서 서브 키를 오픈
		Registry subKey = testKey.openKey(L"subKey");
		testKey.removeKey(L"subKey");
		testThrow(subKey.openKey(L"subsubKey"), Registry::KeyDeletedException);
	}
	// Caution: 현재 유저의 읽기를 거부한 키를 만들고, 오픈하면 Registry::AccessDeniedException을 던지는 것을 눈으로 확인
	//Registry::currentUser().openKey(L"Software\\balor_test");
}


testCase(removeKey) {
	{// 빈 레지스트 키
		Registry emptyKey;
		testAssertionFailed(emptyKey.removeKey(L" "));
	}

	// 삭제가 허락되지 않는 키
	testThrow(Registry::localMachine().removeKey(L"SECURITY"), Registry::AccessDeniedException);

	// 테스트 용 레지스트 키 준비
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 무효한 파라미터
	testAssertionFailed(testKey.removeKey(L""));
	// 존재하지 않는 서브 키 삭제
	testNoThrow(testKey.removeKey(L"subKey"));
	// 존재하는 서브 키 삭제
	testKey.createKey(L"subKey");
	testNoThrow(testKey.removeKey(L"subKey"));
	testAssert(!testKey.openKey(L"subKey"));
	// 서브 키가 존재하는 키 삭제
	Registry subKey = testKey.createKey(L"subKey");
	Registry subsubKey = subKey.createKey(L"subsubKey", true);
	testThrow(testKey.removeKey(L"subKey"), Registry::AccessDeniedException);
	testAssert(testKey.openKey(L"subKey"));
	// 값이 있어도 삭제할 수 있다
	subsubKey.setString(L"value", L"test");
	testNoThrow(subKey.removeKey(L"subsubKey"));
	testAssert(!subKey.openKey(L"subsubKey"));
	// 삭제된 키에서 삭제를 실행 
	subsubKey = subKey.createKey(L"subsubKey");
	testKey.removeKey(L"subKey", true);
	testAssert(!testKey.openKey(L"subKey"));
	testThrow(subKey.removeKey(L"subsubKey"), Registry::KeyDeletedException);
}


testCase(removeKeyRecursive) {
	// 테스트 용 레지스트 키 준비
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 존재하지 않는 서브 키 삭제
	testNoThrow(testKey.removeKey(L"subKey", true));
	// 존재하는 서브 키 삭제
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
	{// 빈 레지스트 키
		Registry emptyKey;
		testAssertionFailed(emptyKey.removeValue(L" "));
	}

	// 테스트 용 레지스트 키 준비
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 존재하지 않는 값 삭제
	testNoThrow(testKey.removeValue(L"value"));
	// 존재하는 값 삭제
	Registry subKey = testKey.createKey(L"subKey", true);
	subKey.setString(L"value", L"test");
	testAssert(subKey.getString(L"value") == L"test");
	testNoThrow(subKey.removeValue(L"value"));
	testThrow(subKey.getString(L"value"), Registry::ValueNotFoundException);
	// 기본 값 삭제
	subKey.setString(L"", L"default");
	testAssert(subKey.getString(L"") == L"default");
	testNoThrow(subKey.removeValue(L""));
	testThrow(subKey.getString(L""), Registry::ValueNotFoundException);
	// 쓰기 권한 없는 키로 값 삭제
	subKey.setString(L"value", L"test");
	subKey = testKey.openKey(L"subKey");
	testThrow(subKey.removeValue(L"value"), Registry::AccessDeniedException);
	testAssert(subKey.getString(L"value") == L"test");
	// 삭제된 키로 값 삭제
	subKey = testKey.openKey(L"subKey", true);
	testKey.removeKey(L"subKey");
	testThrow(subKey.removeValue(L"value"), Registry::KeyDeletedException);
}


testCase(setValue) {
	{// 빈 레지스트 키
		Registry emptyKey;
		testAssertionFailed(emptyKey.setString(L"", L""));
		testAssertionFailed(emptyKey.setBinary(L"", vector<std::uint8_t>()));
		testAssertionFailed(emptyKey.setDword(L"", uint32_t()));
		testAssertionFailed(emptyKey.setQword(L"", uint64_t()));
	}

	// 테스트 용 레지스트 키 준비
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 존재하지 않은 값을 만든다
	testKey.setString(L"value", L"test");
	testAssert(testKey.getString(L"value") == L"test");
	// 존재하는 값을 덮어쒸운다
	testKey.setString(L"value", L"test2");
	testAssert(testKey.getString(L"value") == L"test2");

	// 값과 형을 미스매치
	testAssertionFailed(testKey.setString(L"value", String(), Registry::ValueKind::binary));

	// 이 이상 각 형의 값을 설정 테스트는 testCase(getValue) 에서 테스트 완료

	{// 삭제된 키에 값을 설정
		Registry subKey = testKey.createKey(L"subKey", true);
		testKey.removeKey(L"subKey");
		testThrow(subKey.setString(L"value", L""), Registry::KeyDeletedException);
		testThrow(subKey.setBinary(L"value", vector<std::uint8_t>()), Registry::KeyDeletedException);
		testThrow(subKey.setDword(L"value", uint32_t()), Registry::KeyDeletedException);
		testThrow(subKey.setQword(L"value", uint64_t()), Registry::KeyDeletedException);
	}

	{// 쓰기 권한이 없는 키에 값 설정 
		Registry subKey = testKey.createKey(L"subKey");
		testThrow(subKey.setString(L"value", L""), Registry::AccessDeniedException);
		testThrow(subKey.setBinary(L"value", vector<std::uint8_t>()), Registry::AccessDeniedException);
		testThrow(subKey.setDword(L"value", uint32_t()), Registry::AccessDeniedException);
		testThrow(subKey.setQword(L"value", uint64_t()), Registry::AccessDeniedException);
	}
}


testCase(valueCount) {
	{// 빈 레지스트 키
		Registry emptyKey;
		testAssertionFailed(emptyKey.valueCount());
	}

	// 테스트 용 레지스트 키 준비
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 값 수
	testAssert(testKey.valueCount() == 0);
	testKey.setString(L"value0", L"test");
	testKey.setString(L"value1", L"test");
	testKey.setString(L"value2", L"test");
	testAssert(testKey.valueCount() == 3);
	// 규정 값
	testKey.setString(L"", L"test");
	testAssert(testKey.valueCount() == 4);

	{// 삭제된 레지스트리 키에서 값 수를 취득
		Registry subKey = testKey.createKey(L"subKey");
		testKey.removeKey(L"subKey");
		testThrow(subKey.valueCount(), Registry::KeyDeletedException);
	}
	{// 권한 없는 키로 값 취득
		testKey.createKey(L"subKey");
		HKEY handle;
		testAssert(RegOpenKeyExW(testKey, L"subKey", 0, STANDARD_RIGHTS_READ, &handle) == ERROR_SUCCESS);
		Registry subKey;
		*(HKEY*)&subKey = handle;
		testThrow(subKey.valueCount(), Registry::AccessDeniedException);
	}
}


testCase(valueNamesIterator) {
	{// 빈 레지스트 키
		Registry emptyKey;
		testAssertionFailed(emptyKey.valueNamesIterator());
	}

	// 테스트 용 레지스트 키 준비
	Registry testKey = getTestKey();
	scopeExit(&testKeyDeleteFunction);

	// 빈 값 이름 리스트 취득
	testAssert(!testKey.valueNamesIterator());
	{// 값 이름 리스트 취득
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
	{// 삭제된 레지스트 키에서 값 이름 리스트 취득
		Registry subKey = testKey.createKey(L"subKey");
		testKey.removeKey(L"subKey");
		testThrow(subKey.valueNamesIterator(), Registry::KeyDeletedException);
	}
	{// 서브키 이름 열거 권한 없는 키로 열거 
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