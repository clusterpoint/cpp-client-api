#include "TestCase.hpp"

#include <cassert>
#include <memory>

TestCase::TestCase(CPS::Connection& connection)
	: connection_(connection)
{
}

TestCase::~TestCase() {}

CPS::Connection& TestCase::connection() const {
	return connection_;
}

void TestCase::run() {
	set_up();
	run_tests();
	tear_down();
}

void TestCase::set_up() {
	std::unique_ptr<CPS::Response> resp(
			connection().sendRequest<CPS::Response>(CPS::Request("clear")));
	assert(!resp->hasFailed());
}

void TestCase::tear_down() {
	std::unique_ptr<CPS::StatusResponse> status_resp(
			connection().sendRequest<CPS::StatusResponse>(CPS::StatusRequest()));
	assert(status_resp->getRepository().documents == 0);
}
