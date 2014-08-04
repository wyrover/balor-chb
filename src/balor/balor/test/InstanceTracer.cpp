#include "InstanceTracer.hpp"


namespace balor {
	namespace test {



namespace {
int defaultConstruct = 0;
int copyConstruct = 0;
int rvalueConstruct = 0;
int destruct = 0;
int operatorEqual = 0;
int rvalueOperatorEqual = 0;
} // namespace



InstanceTracer::InstanceTracer() {
	++defaultConstruct;
}


InstanceTracer::InstanceTracer(const InstanceTracer& ) {
	++copyConstruct;
}


InstanceTracer::InstanceTracer(InstanceTracer&& ) {
	++rvalueConstruct;
}


InstanceTracer::~InstanceTracer() {
	++destruct;
}


InstanceTracer& InstanceTracer::operator=(const InstanceTracer& ) {
	++operatorEqual;
	return *this;
}


InstanceTracer& InstanceTracer::operator=(InstanceTracer&& ) {
	++rvalueOperatorEqual;
	return *this;
}


int InstanceTracer::defaultConstructCount() {
	return defaultConstruct;
}


int InstanceTracer::copyConstructCount() {
	return copyConstruct;
}


int InstanceTracer::rvalueConstructCount() {
	return rvalueConstruct;
}


int InstanceTracer::destructCount() {
	return destruct;
}


int InstanceTracer::operatorEqualCount() {
	return operatorEqual;
}


int InstanceTracer::rvalueOperatorEqualCount() {
	return rvalueOperatorEqual;
}


void InstanceTracer::clearAllCount() {
	defaultConstruct = 0;
	copyConstruct = 0;
	rvalueConstruct = 0;
	destruct = 0;
	operatorEqual = 0;
	rvalueOperatorEqual = 0;
}


bool InstanceTracer::checkAllCount(int defaultConstructCount
								  ,int copyConstructCount
								  ,int rvalueConstructCount
								  ,int destructCount
								  ,int operatorEqualCount
								  ,int rvalueOperatorEqualCount) {
	int defaultConstructDebug    = defaultConstruct   ;
	int copyConstructDebug       = copyConstruct      ;
	int rvalueConstructDebug     = rvalueConstruct    ;
	int destructDebug            = destruct           ;
	int operatorEqualDebug       = operatorEqual      ;
	int rvalueOperatorEqualDebug = rvalueOperatorEqual;

	if ( defaultConstructDebug    != defaultConstructCount   ) return false;
	if ( copyConstructDebug       != copyConstructCount      ) return false;
	if ( rvalueConstructDebug     != rvalueConstructCount    ) return false;
	if ( destructDebug            != destructCount           ) return false;
	if ( operatorEqualDebug       != operatorEqualCount      ) return false;
	if ( rvalueOperatorEqualDebug != rvalueOperatorEqualCount) return false;
	return true;
}



	}
}