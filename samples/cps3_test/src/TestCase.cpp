#include "TestCase.hpp"

#include <cassert>
#include <chrono>
#include <memory>
#include <thread>

TestCase::TestCase(CPS::Connection& connection)
	: connection_(connection)
{
}

TestCase::~TestCase() {}

CPS::Connection& TestCase::connection() const {
	return connection_;
}

void TestCase::run() {
	run_tests();
}

void TestCase::set_up() {
}

void TestCase::tear_down() {
	connection().setDebug(false);
	std::unique_ptr<CPS::StatusResponse> status_resp(
			connection().sendRequest<CPS::StatusResponse>(CPS::StatusRequest()));
	assert(status_resp->getRepository().documents == 0);
	connection().setDebug(true);
}
