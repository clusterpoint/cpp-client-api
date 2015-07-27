#include "TestSuite.hpp"
#include "BasicIOTest.hpp"
#include "PerformanceTest.hpp"
#include "SamplesTest.hpp"

#include <iostream>

/* DB configuration:

<policy>
  <rule>
    <xpath>//document</xpath>
    <property>document=yes</property>
    <property>index=xml&amp;text&amp;facet</property>
    <property>index-numbers=yes</property>
    <property>exact-match=binary</property>
    <property>index-empty=yes</property>
    <property>list=yes</property>
  </rule>
  <rule>
    <xpath>//document/id</xpath>
    <property>id=yes</property>
    <property>index=-facet</property>
  </rule>
  <rule>
    <xpath>//document/title</xpath>
    <property>index=xml&amp;text</property>
    <property>index-numbers=no</property>
    <property>index-dates=no</property>
    <property>index-empty=no</property>
    <property>exact-match=text</property>
    <property>list=no</property>
  </rule>
  <rule>
    <xpath>//document/body</xpath>
    <property>index=text</property>
    <property>index-numbers=no</property>
    <property>index-dates=no</property>
    <property>index-empty=no</property>
    <property>exact-match=text</property>
    <property>list=no</property>
  </rule>
  <rule>
    <xpath>//document/category</xpath>
    <property>index=xml&amp;text&amp;facet</property>
    <property>index-numbers=no</property>
    <property>index-dates=no</property>
    <property>index-empty=no</property>
    <property>exact-match=text</property>
    <property>list=no</property>
  </rule>
  <rule>
    <xpath>//document/car_params</xpath>
    <property>index=xml</property>
    <property>index-numbers=no</property>
    <property>index-dates=no</property>
    <property>index-empty=no</property>
    <property>exact-match=none</property>
    <property>list=no</property>
  </rule>
  <rule>
    <xpath>//document/car_params/make</xpath>
    <property>index=xml&amp;text&amp;facet</property>
    <property>index-numbers=no</property>
    <property>index-dates=no</property>
    <property>index-empty=no</property>
    <property>exact-match=text</property>
    <property>list=no</property>
  </rule>
  <rule>
    <xpath>//document/car_params/model</xpath>
    <property>index=xml&amp;text&amp;facet</property>
    <property>index-numbers=no</property>
    <property>index-dates=no</property>
    <property>index-empty=no</property>
    <property>exact-match=text</property>
    <property>list=no</property>
  </rule>
  <rule>
    <xpath>//document/car_params/year</xpath>
    <property>index=xml&amp;text&amp;facet</property>
    <property>index-numbers=int</property>
    <property>index-dates=no</property>
    <property>index-empty=no</property>
    <property>exact-match=text</property>
    <property>list=no</property>
  </rule>
</policy>

*/

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
