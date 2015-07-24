#pragma once

#ifndef PERFORMANCETEST_HPP_
#define PERFORMANCETEST_HPP_

#include "TestCase.hpp"

class PerformanceTest : public TestCase
{
public:
  PerformanceTest(CPS::Connection& connection);

protected:
  virtual void tear_down();
  virtual void run_tests();

private:
  void test_insert_thousand_documents();
};

#endif /* PERFORMANCETEST_HPP_ */
