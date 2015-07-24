#include "TestCase.hpp"
#include "Utils.hpp"

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
  bool first_time = true;
  while (!search_resp->hasFailed() && search_resp->getFound() != 0)
  {
    if (first_time)
    {
      std::cout << "Clearing DB";
      first_time = false;
    }
    std::cout << ".";
    std::cout.flush();
    // Delete found documents
    auto docs = search_resp->getDocumentsXML();
    std::vector<std::string> doc_ids;
    doc_ids.reserve(docs.size());
    std::transform(docs.begin(), docs.end(), std::back_inserter(doc_ids), &get_docid);
    CPS::DeleteRequest delete_req(doc_ids);
    std::unique_ptr<CPS::DeleteResponse> delete_resp(
        connection().sendRequest<CPS::DeleteResponse>(delete_req));
    search_resp.reset(connection().sendRequest<CPS::SearchResponse>(CPS::SearchRequest("*")));
  }
  if (!first_time)
  {
    std::cout << std::endl;
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
