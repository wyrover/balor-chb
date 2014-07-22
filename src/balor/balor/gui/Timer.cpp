#include "Timer.hpp"

#include <utility>

#include <balor/gui/Control.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>


namespace balor {
	namespace gui {

using std::move;


namespace {
VOID CALLBACK procedure(HWND //handle
					  , UINT //message
					  , UINT_PTR id
					  , DWORD //time
					  ) {
	Timer* timer = reinterpret_cast<Timer*>(id);
	assert(timer);
	Timer::Run event(*timer);
	timer->onRun()(event);
}
} // namespace



Timer::Timer() : _interval(1000), _started(false) {
}


Timer::Timer(Timer&& value)
	: _executor(move(value._executor))
	, _interval(move(value._interval))
	, _onRun(move(value._onRun))
	, _started(move(value._started)) {
	assert("Invalid rvalue" && !_started); // this ポインタを ID にしているので start 中に move はダメ。
}


Timer::Timer(Control& executor, int interval, Listener<Timer::Run&> onRun)
	: _executor(&executor)
	, _interval(interval)
	, _onRun(move(onRun))
	, _started(false) {
	assert("Invalid control" && executor);
	assert("Invalid interval" && 0 < interval);
}


Timer::~Timer() {
	if (_executor) {
		stop();
	}
}


Timer& Timer::operator=(Timer&& value) {
	if (&value != this) {
		this->~Timer();
		new (this) Timer(move(value));
	}
	return *this;
}


Control* Timer::executor() const {
	return _executor;
}


int Timer::interval() const {
	return _interval;
}


void Timer::interval(int value) {
	assert("Invalid interval" && 0 < value);
	if (value != interval()) {
		_interval = value;
		if (started()) {
			stop();
			start();
		}
	}
}


Listener<Timer::Run&>& Timer::onRun() {
	return _onRun;
}


void Timer::start() {
	assert("Null executor" && executor());
	if (!_started) {
		SetTimer(*executor(), reinterpret_cast<UINT_PTR>(this), _interval, &procedure);
		_started = true;
	}
}


bool Timer::started() const {
	return _started;
}


void Timer::stop() {
	if (_started) {
		if (executor()) {
			KillTimer(*executor(), reinterpret_cast<UINT_PTR>(this));
		}
		_started = false;
	}
}



	}
}