#include "Version.hpp"


namespace balor {
	namespace system {



Version::Version(int major, int minor, int build, int revision)
	: _major(major), _minor(minor), _build(build), _revision(revision) {
}


int Version::build() const {
	return _build;
}


int Version::major() const {
	return _major;
}


int Version::minor() const {
	return _minor;
}


int Version::revision() const {
	return _revision;
}


bool Version::operator<(const Version& rhs) const {
	if (_major < rhs._major) {
		return true;
	} else if (_major == rhs._major) {
		if (_minor < rhs._minor) {
			return true;
		} else if (_minor == rhs._minor) {
			if (_build < rhs._build) {
				return true;
			} else if (_build == rhs._build) {
				if (_revision < rhs._revision) {
					return true;
				}
			}
		}
	}
	return false;
}


bool Version::operator<=(const Version& rhs) const {
	return !(rhs < *this);
}


bool Version::operator==(const Version& rhs) const {
	return (_major == rhs._major)
		&& (_minor == rhs._minor)
		&& (_build == rhs._build)
		&& (_revision == rhs._revision);
}


bool Version::operator!=(const Version& rhs) const {
	return !(*this == rhs);
}



	}
}