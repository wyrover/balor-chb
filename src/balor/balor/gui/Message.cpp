#include "Message.hpp"

#include <type_traits>

#include <balor/system/windows.hpp>


namespace balor {
	namespace gui {

static_assert(std::is_same<WPARAM, Message::WPARAM>::value, "Invalid typedef");
static_assert(std::is_same<LPARAM, Message::LPARAM>::value, "Invalid typedef");
static_assert(std::is_same<LRESULT, Message::LRESULT>::value, "Invalid typedef");
static_assert(sizeof(Message) == sizeof(MSG) + sizeof(LRESULT), "Invalid typedef");


Message::Message() {
}


Message::Message(HWND handle, unsigned int message, WPARAM wparam, LPARAM lparam)
	: handle(handle), message(message), wparam(wparam), lparam(lparam), result(0) {
}



	}
}