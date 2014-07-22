#include "testUniqueAny.hpp"

#include <cassert>

#include <balor/Listener.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace testUniqueAny {



UniqueAny getStringAny() {
	return String(L"getStringAny");
}


UniqueAny getListenerAny() {
	return Listener<String&>([&] (String& value) {
		value = L"getListenerAny";
	});
}


void setStringAny(UniqueAny& any) {
	if (!any.empty()) {
		assert(any_cast<String&>(any) == L"setStringAny");
	}
}


void setListenerAny(UniqueAny& any) {
	String string;
	any_cast<Listener<String&>&>(any)(string);
	assert(string == L"setListenerAny");
}



	}
}