#include "TestSuite.hpp"
#include "BasicIOTest.hpp"
#include "SamplesTest.hpp"

TestSuite::TestSuite(CPS::Connection& connection)
	: connection_(connection)
{
}

void TestSuite::run() {
	BasicIOTest(connection_).run();
	SamplesTest(connection_).run();
}
