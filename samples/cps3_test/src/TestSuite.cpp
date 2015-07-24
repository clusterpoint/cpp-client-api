#include "TestSuite.hpp"
#include "BasicIOTest.hpp"
#include "PerformanceTest.hpp"
#include "SamplesTest.hpp"

#include <iostream>

TestSuite::TestSuite(CPS::Connection& connection)
  : connection_(connection)
{
}

void TestSuite::run()
{
  BasicIOTest(connection_).run();
  SamplesTest(connection_).run();
  PerformanceTest(connection_).run();

  std::cout << "*** ALL TESTS PASSED ***" << std::endl;
}
