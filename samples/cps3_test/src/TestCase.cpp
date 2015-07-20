#include "TestCase.hpp"

#include <cassert>
#include <chrono>
#include <memory>
#include <thread>

TestCase::TestCase(CPS::Connection& connection)
  : connection_(connection)
{
}

TestCase::~TestCase()
{
}

CPS::Connection& TestCase::connection() const
{
  return connection_;
}

void TestCase::run()
{
  run_tests();
}

void TestCase::set_up()
{
  connection().setDebug(false);
  std::unique_ptr<CPS::SearchResponse> search_resp(
      connection().sendRequest<CPS::SearchResponse>(CPS::SearchRequest("*")));
  if (!search_resp->hasFailed() && search_resp->getFound() != 0)
  {
    std::unique_ptr<CPS::Response> resp(
        connection().sendRequest<CPS::Response>(CPS::Request("clear")));
    assert(!resp->hasFailed());
    // Clear command deletes database. It may take some time until a new
    // database is created. So, commands will return errors for a while.
    // We will use Search command to find out, when the database is ready
    // to receive any further commands.
    for (int i = 0; i < 120; i += 1)
    {
      std::this_thread::sleep_for(std::chrono::seconds(3));
      try
      {
        std::unique_ptr<CPS::SearchResponse> search_resp(
            connection().sendRequest<CPS::SearchResponse>(CPS::SearchRequest("*")));
        if (!search_resp->hasFailed() && search_resp->getFound() == 0)
        {
          break;
        }
      }
      catch (CPS::Exception& e)
      {
      }
    }
  }
  connection().setDebug(true);
}

void TestCase::tear_down()
{
  connection().setDebug(false);
  std::unique_ptr<CPS::StatusResponse> status_resp(
      connection().sendRequest<CPS::StatusResponse>(CPS::StatusRequest()));
  assert(status_resp->getRepository().documents == 0);
  connection().setDebug(true);
}
