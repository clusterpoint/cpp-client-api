#pragma once

#ifndef TESTCASE_HPP_
#define TESTCASE_HPP_

#include "cps/CPS_API.hpp"

class TestCase
{
public:
	TestCase(CPS::Connection& connection);
	virtual ~TestCase();

	CPS::Connection& connection() const;
	void run();

protected:
	virtual void set_up();
	virtual void tear_down();
	virtual void run_tests() = 0;

private:
	CPS::Connection& connection_;
};

#define RUN_TEST_XNAME(s) RUN_TEST_NAME(s)
#define RUN_TEST_NAME(s) #s

#define RUN_TEST(test_name) \
	do { \
		std::cout << "***** " << RUN_TEST_XNAME(test_name) << " *****" << std::endl; \
		set_up(); \
		test_name (); \
		tear_down(); \
		std::cout << std::endl; \
	} while (0)

#endif /* TESTCASE_HPP_ */
