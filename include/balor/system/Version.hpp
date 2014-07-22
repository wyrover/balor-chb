#pragma once


namespace balor {
	namespace system {



/**
 * ファイルやモジュールのバージョンを表す。
 */
class Version {
public:
	explicit Version(int major = 0, int minor = 0, int build = 0, int revision = 0);

public:
	int build() const;
	int major() const;
	int minor() const;
	int revision() const;

public:
	bool operator<(const Version& rhs) const;
	bool operator<=(const Version& rhs) const;
	bool operator==(const Version& rhs) const;
	bool operator!=(const Version& rhs) const;

private:
	int _major;
	int _minor;
	int _build;
	int _revision;
};



	}
}