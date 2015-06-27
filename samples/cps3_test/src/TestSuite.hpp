#pragma once

#ifndef TESTSUITE_HPP_
#define TESTSUITE_HPP_

#include "cps/CPS_API.hpp"

class TestSuite
{
public:
	TestSuite(CPS::Connection& connection);

	void run();

private:
	CPS::Connection& connection_;
};

#endif /* TESTSUITE_HPP_ */
