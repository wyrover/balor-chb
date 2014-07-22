#pragma once


namespace balor {
	namespace system {



/**
 * プロセス暫定実装。
 */
class Process {
private:
	typedef void* HANDLE;

public:
	Process();
	~Process();

public:

public:
	int gdiHandleCount() const;
	int userHandleCount() const;

private:
	HANDLE _handle;
};



	}
}