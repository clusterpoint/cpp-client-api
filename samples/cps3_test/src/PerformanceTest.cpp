#include "PerformanceTest.hpp"
#include "Utils.hpp"

PerformanceTest::PerformanceTest(CPS::Connection& connection)
  : TestCase(connection)
{
}

void PerformanceTest::tear_down()
{
}

void PerformanceTest::run_tests()
{
  RUN_TEST(test_insert_thousand_documents);
}

void PerformanceTest::test_insert_thousand_documents()
{
  static const char* doc_template =
      "<document><id>{ID}</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
  static const char* id_placeholder = "{ID}";

  std::vector<std::string> docs_vector;
  docs_vector.reserve(10);
  size_t doc_count = 0;
  for (size_t i = 0; i < 100; ++i)
  {
    // Generate a list of documents
    docs_vector.clear();
    std::generate_n(std::back_inserter(docs_vector), 10, [&doc_count]()
    {
      std::string doc(doc_template);
      doc.replace(
          doc.find(id_placeholder),
          strlen(id_placeholder),
          make_docid("test_insert_many_documents_and_delete_them", doc_count++));
      return doc;
    });
    // Insert documents
    CPS::InsertRequest insert_req(docs_vector);
    std::unique_ptr<CPS::InsertResponse> insert_resp(
        connection().sendRequest<CPS::InsertResponse>(insert_req));
    auto inserted_ids = insert_resp->getModifiedIds();
    std::sort(inserted_ids.begin(), inserted_ids.end());
    std::cout << "Insert ids: " << CPS::Utils::join(inserted_ids) << std::endl;
    assert(inserted_ids.size() == 10);
  }
  assert(doc_count == 1000);
}
