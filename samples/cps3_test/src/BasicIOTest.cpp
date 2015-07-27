#include "BasicIOTest.hpp"
#include "Utils.hpp"

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

BasicIOTest::BasicIOTest(CPS::Connection& connection)
  : TestCase(connection)
{
}

void BasicIOTest::run_tests()
{
  RUN_TEST(test_insert_one_document_and_delete_it);
  RUN_TEST(test_insert_many_documents_and_delete_them);
  RUN_TEST(test_insert_one_document_update_it_and_then_delete_it);
  RUN_TEST(test_insert_many_documents_update_them_and_then_delete_them);
  RUN_TEST(test_insert_one_document_retrieve_it_and_then_delete_it);
  RUN_TEST(test_insert_many_documents_retrieve_them_and_then_delete_them);
  RUN_TEST(test_insert_one_document_partially_replace_it_retrieve_it_and_then_delete_it);
  RUN_TEST(test_insert_many_documents_partially_replace_them_retrieve_them_and_then_delete_them);
  RUN_TEST(test_insert_many_documents_list_paths_and_delete_them);
  RUN_TEST(test_insert_two_documents_list_last_and_delete_them);
  RUN_TEST(test_insert_two_documents_lookup_last_and_delete_them);
  RUN_TEST(test_insert_many_documents_search_and_delete_them);
}

void BasicIOTest::test_insert_one_document_and_delete_it()
{
  std::vector<std::string> docs_vector;
  docs_vector.push_back(
      "<document><id>test_insert_one_document_and_delete_it</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
  // Insert document
  CPS::InsertRequest insert_req(docs_vector);
  std::unique_ptr<CPS::InsertResponse> insert_resp(
      connection().sendRequest<CPS::InsertResponse>(insert_req));
  auto inserted_ids = insert_resp->getModifiedIds();
  std::cout << "Insert ids: " << CPS::Utils::join(inserted_ids) << std::endl;
  assert(inserted_ids.size() == 1);
  assert(inserted_ids[0] == __FUNCTION__);
  // Delete document
  CPS::DeleteRequest delete_req(inserted_ids);
  std::unique_ptr<CPS::DeleteResponse> delete_resp(
      connection().sendRequest<CPS::DeleteResponse>(delete_req));
  // Print out errors and related document ids
  print_errors(std::cout, delete_resp->getErrors());
  // Print out deleted document ids
  auto deleted_ids = delete_resp->getModifiedIds();
  std::cout << "Delete ids: " << CPS::Utils::join(deleted_ids) << std::endl;
  assert(deleted_ids.size() == 1);
  assert(deleted_ids[0] == __FUNCTION__);
}

void BasicIOTest::test_insert_many_documents_and_delete_them()
{
  static const char* doc_template =
      "<document><id>{ID}</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
  static const char* id_placeholder = "{ID}";

  // Generate a list of documents
  std::vector<std::string> docs_vector;
  docs_vector.reserve(10);
  int doc_count = 0;
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
  assert(inserted_ids.size() == doc_count);
  // Delete documents
  CPS::DeleteRequest delete_req(inserted_ids);
  std::unique_ptr<CPS::DeleteResponse> delete_resp(
      connection().sendRequest<CPS::DeleteResponse>(delete_req));
  // Print out errors and related document ids
  print_errors(std::cout, delete_resp->getErrors());
  // Print out deleted document ids
  auto deleted_ids = delete_resp->getModifiedIds();
  std::sort(deleted_ids.begin(), deleted_ids.end());
  std::cout << "Delete ids: " << CPS::Utils::join(deleted_ids) << std::endl;
  assert(deleted_ids.size() == doc_count);
  auto mismatch = std::mismatch(inserted_ids.begin(), inserted_ids.end(), deleted_ids.begin());
  assert(mismatch.first == inserted_ids.end());
  assert(mismatch.second == deleted_ids.end());
}

void BasicIOTest::test_insert_one_document_update_it_and_then_delete_it()
{
  std::vector<std::string> docs_vector;
  docs_vector.push_back(
      "<document><id>test_insert_one_document_update_it_and_then_delete_it</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
  // Insert document
  CPS::InsertRequest insert_req(docs_vector);
  std::unique_ptr<CPS::InsertResponse> insert_resp(
      connection().sendRequest<CPS::InsertResponse>(insert_req));
  auto inserted_ids = insert_resp->getModifiedIds();
  std::cout << "Insert ids: " << CPS::Utils::join(inserted_ids) << std::endl;
  assert(inserted_ids.size() == 1);
  assert(inserted_ids[0] == __FUNCTION__);
  // Update document
  CPS::UpdateRequest update_req(
      inserted_ids[0],
      "<document><id>test_insert_one_document_update_it_and_then_delete_it</id><title>Test document 2</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
  std::unique_ptr<CPS::UpdateResponse> update_resp(
      connection().sendRequest<CPS::UpdateResponse>(update_req));
  auto update_ids = update_resp->getModifiedIds();
  std::cout << "Update ids: " << CPS::Utils::join(update_ids) << std::endl;
  assert(update_ids.size() == 1);
  assert(update_ids[0] == __FUNCTION__);
  // Delete document
  CPS::DeleteRequest delete_req(update_ids);
  std::unique_ptr<CPS::DeleteResponse> delete_resp(
      connection().sendRequest<CPS::DeleteResponse>(delete_req));
  // Print out errors and related document ids
  print_errors(std::cout, delete_resp->getErrors());
  // Print out deleted document ids
  auto deleted_ids = delete_resp->getModifiedIds();
  std::cout << "Delete ids: " << CPS::Utils::join(deleted_ids) << std::endl;
  assert(deleted_ids.size() == 1);
  assert(deleted_ids[0] == __FUNCTION__);
}

void BasicIOTest::test_insert_many_documents_update_them_and_then_delete_them()
{
  static const char* doc_template =
      "<document><id>{ID}</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
  static const char* id_placeholder = "{ID}";

  // Generate a list of documents
  std::vector<std::string> docs_vector;
  docs_vector.reserve(10);
  int doc_count = 0;
  std::generate_n(std::back_inserter(docs_vector), 10, [&doc_count]()
  {
    std::string doc(doc_template);
    doc.replace(
        doc.find(id_placeholder),
        strlen(id_placeholder),
        make_docid("test_insert_many_documents_update_them_and_then_delete_them", doc_count++));
    return doc;
  });
  // Insert documents
  CPS::InsertRequest insert_req(docs_vector);
  std::unique_ptr<CPS::InsertResponse> insert_resp(
      connection().sendRequest<CPS::InsertResponse>(insert_req));
  auto inserted_ids = insert_resp->getModifiedIds();
  std::sort(inserted_ids.begin(), inserted_ids.end());
  std::cout << "Insert ids: " << CPS::Utils::join(inserted_ids) << std::endl;
  assert(inserted_ids.size() == doc_count);
  // Update documents
  std::map<std::string, std::string> docs_map;
  int i = 0;
  std::generate_n(std::inserter(docs_map, docs_map.end()), doc_count, [&i]()
  {
    std::string docid = make_docid("test_insert_many_documents_update_them_and_then_delete_them", i++);
    std::string doc(doc_template);
    doc.replace(
        doc.find(id_placeholder),
        strlen(id_placeholder),
        docid);
    return std::make_pair(docid, doc);
  });
  CPS::UpdateRequest update_req(docs_map);
  std::unique_ptr<CPS::UpdateResponse> update_resp(
      connection().sendRequest<CPS::UpdateResponse>(update_req));
  auto update_ids = update_resp->getModifiedIds();
  std::sort(update_ids.begin(), update_ids.end());
  std::cout << "Update ids: " << CPS::Utils::join(update_ids) << std::endl;
  assert(update_ids.size() == doc_count);
  auto update_mismatch = std::mismatch(inserted_ids.begin(), inserted_ids.end(), update_ids.begin());
  assert(update_mismatch.first == inserted_ids.end());
  assert(update_mismatch.second == update_ids.end());
  // Delete documents
  CPS::DeleteRequest delete_req(update_ids);
  std::unique_ptr<CPS::DeleteResponse> delete_resp(
      connection().sendRequest<CPS::DeleteResponse>(delete_req));
  // Print out errors and related document ids
  print_errors(std::cout, delete_resp->getErrors());
  // Print out deleted document ids
  auto deleted_ids = delete_resp->getModifiedIds();
  std::sort(deleted_ids.begin(), deleted_ids.end());
  std::cout << "Delete ids: " << CPS::Utils::join(deleted_ids) << std::endl;
  assert(deleted_ids.size() == doc_count);
  auto delete_mismatch = std::mismatch(update_ids.begin(), update_ids.end(), deleted_ids.begin());
  assert(delete_mismatch.first == update_ids.end());
  assert(delete_mismatch.second == deleted_ids.end());
}

void BasicIOTest::test_insert_one_document_retrieve_it_and_then_delete_it()
{
  std::vector<std::string> docs_vector;
  docs_vector.push_back(
      "<document><id>test_insert_one_document_retrieve_it_and_then_delete_it</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
  // Insert document
  CPS::InsertRequest insert_req(docs_vector);
  std::unique_ptr<CPS::InsertResponse> insert_resp(
      connection().sendRequest<CPS::InsertResponse>(insert_req));
  auto inserted_ids = insert_resp->getModifiedIds();
  std::cout << "Insert ids: " << CPS::Utils::join(inserted_ids) << std::endl;
  assert(inserted_ids.size() == 1);
  assert(inserted_ids[0] == __FUNCTION__);
  // Retrieve document
  CPS::RetrieveRequest retrieve_req(inserted_ids);
  std::unique_ptr<CPS::RetrieveResponse> retrieve_resp(
      connection().sendRequest<CPS::RetrieveResponse>(retrieve_req));
  auto retrieved_docs = retrieve_resp->getDocumentsString();
  std::cout << "Retrieved ids: " << CPS::Utils::join(inserted_ids) << std::endl;
  assert(retrieved_docs.size() == 1);
  assert(retrieved_docs[0] == docs_vector[0]);
  // Delete document
  CPS::DeleteRequest delete_req(inserted_ids);
  std::unique_ptr<CPS::DeleteResponse> delete_resp(
      connection().sendRequest<CPS::DeleteResponse>(delete_req));
  // Print out errors and related document ids
  print_errors(std::cout, delete_resp->getErrors());
  // Print out deleted document ids
  auto deleted_ids = delete_resp->getModifiedIds();
  std::cout << "Delete ids: " << CPS::Utils::join(deleted_ids) << std::endl;
  assert(deleted_ids.size() == 1);
  assert(deleted_ids[0] == __FUNCTION__);
}

void BasicIOTest::test_insert_many_documents_retrieve_them_and_then_delete_them()
{
  static const char* doc_template =
      "<document><id>{ID}</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
  static const char* id_placeholder = "{ID}";

  // Generate a list of documents
  std::vector<std::string> docs_vector;
  docs_vector.reserve(10);
  int doc_count = 0;
  std::generate_n(std::back_inserter(docs_vector), 10, [&doc_count]()
  {
    std::string doc(doc_template);
    doc.replace(
        doc.find(id_placeholder),
        strlen(id_placeholder),
        make_docid("test_insert_many_documents_retrieve_them_and_then_delete_them", doc_count++));
    return doc;
  });
  // Insert documents
  CPS::InsertRequest insert_req(docs_vector);
  std::unique_ptr<CPS::InsertResponse> insert_resp(
      connection().sendRequest<CPS::InsertResponse>(insert_req));
  auto inserted_ids = insert_resp->getModifiedIds();
  std::cout << "Insert ids: " << CPS::Utils::join(inserted_ids) << std::endl;
  assert(inserted_ids.size() == doc_count);
  // Retrieve documents
  CPS::RetrieveRequest retrieve_req(inserted_ids);
  std::unique_ptr<CPS::RetrieveResponse> retrieve_resp(
      connection().sendRequest<CPS::RetrieveResponse>(retrieve_req));
  auto retrieved_docs = retrieve_resp->getDocumentsString();
  std::cout << "Retrieved ids: " << CPS::Utils::join(inserted_ids) << std::endl;
  assert(retrieved_docs.size() == doc_count);
  std::sort(docs_vector.begin(), docs_vector.end());
  std::sort(retrieved_docs.begin(), retrieved_docs.end());
  auto retrieved_mismatch = std::mismatch(docs_vector.begin(), docs_vector.end(), retrieved_docs.begin());
  assert(retrieved_mismatch.first == docs_vector.end());
  assert(retrieved_mismatch.second == retrieved_docs.end());
  // Delete documents
  CPS::DeleteRequest delete_req(inserted_ids);
  std::unique_ptr<CPS::DeleteResponse> delete_resp(
      connection().sendRequest<CPS::DeleteResponse>(delete_req));
  // Print out errors and related document ids
  print_errors(std::cout, delete_resp->getErrors());
  // Print out deleted document ids
  auto deleted_ids = delete_resp->getModifiedIds();
  std::sort(inserted_ids.begin(), inserted_ids.end());
  std::sort(deleted_ids.begin(), deleted_ids.end());
  std::cout << "Delete ids: " << CPS::Utils::join(deleted_ids) << std::endl;
  assert(deleted_ids.size() == doc_count);
  auto delete_mismatch = std::mismatch(inserted_ids.begin(), inserted_ids.end(), deleted_ids.begin());
  assert(delete_mismatch.first == inserted_ids.end());
  assert(delete_mismatch.second == deleted_ids.end());
}

void BasicIOTest::test_insert_one_document_partially_replace_it_retrieve_it_and_then_delete_it()
{
  std::vector<std::string> docs_vector;
  docs_vector.push_back(
      "<document><id>test_insert_one_document_partially_replace_it_retrieve_it_and_then_delete_it</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
  // Insert document
  CPS::InsertRequest insert_req(docs_vector);
  std::unique_ptr<CPS::InsertResponse> insert_resp(
      connection().sendRequest<CPS::InsertResponse>(insert_req));
  auto inserted_ids = insert_resp->getModifiedIds();
  std::cout << "Insert ids: " << CPS::Utils::join(inserted_ids) << std::endl;
  assert(inserted_ids.size() == 1);
  assert(inserted_ids[0] == __FUNCTION__);
  // Partially replace document
  CPS::PartialReplaceRequest partial_replace_req(
      inserted_ids[0], "<title>Test document 2</title>");
  std::unique_ptr<CPS::PartialReplaceResponse> partial_replace_resp(
      connection().sendRequest<CPS::PartialReplaceResponse>(partial_replace_req));
  auto partially_replaced_ids = partial_replace_resp->getModifiedIds();
  std::cout << "Partially replaced ids: " << CPS::Utils::join(partially_replaced_ids) << std::endl;
  assert(partially_replaced_ids.size() == 1);
  assert(partially_replaced_ids[0] == __FUNCTION__);
  // Retrieve document
  CPS::RetrieveRequest retrieve_req(partially_replaced_ids);
  std::unique_ptr<CPS::RetrieveResponse> retrieve_resp(
      connection().sendRequest<CPS::RetrieveResponse>(retrieve_req));
  auto retrieved_docs = retrieve_resp->getDocumentsXML();
  std::cout << "Retrieved ids: " << CPS::Utils::join(partially_replaced_ids) << std::endl;
  assert(retrieved_docs.size() == 1);
  auto title_nodes = retrieved_docs[0]->FindFast("/document/title");
  assert(title_nodes.size() == 1);
  assert(title_nodes[0]->getValue() == "Test document 2");
  // Delete document
  CPS::DeleteRequest delete_req(inserted_ids);
  std::unique_ptr<CPS::DeleteResponse> delete_resp(
      connection().sendRequest<CPS::DeleteResponse>(delete_req));
  // Print out errors and related document ids
  print_errors(std::cout, delete_resp->getErrors());
  // Print out deleted document ids
  auto deleted_ids = delete_resp->getModifiedIds();
  std::cout << "Delete ids: " << CPS::Utils::join(deleted_ids) << std::endl;
  assert(deleted_ids.size() == 1);
  assert(deleted_ids[0] == __FUNCTION__);
}

void BasicIOTest::test_insert_many_documents_partially_replace_them_retrieve_them_and_then_delete_them()
{
  static const char* doc_template =
      "<document><id>{ID}</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
  static const char* id_placeholder = "{ID}";
  static const char* title_placeholder = "<title>Test document 1</title>";

  // Generate a list of documents
  std::vector<std::string> docs_vector;
  docs_vector.reserve(10);
  int doc_count = 0;
  std::generate_n(
      std::back_inserter(docs_vector),
      10,
      [&doc_count]()
      {
        std::string doc(doc_template);
        doc.replace(
            doc.find(id_placeholder),
            strlen(id_placeholder),
            make_docid("test_insert_many_documents_partially_replace_them_retrieve_them_and_then_delete_them", doc_count++));
        return doc;
      });
  // Insert documents
  CPS::InsertRequest insert_req(docs_vector);
  std::unique_ptr<CPS::InsertResponse> insert_resp(
      connection().sendRequest<CPS::InsertResponse>(insert_req));
  auto inserted_ids = insert_resp->getModifiedIds();
  std::sort(inserted_ids.begin(), inserted_ids.end());
  std::cout << "Insert ids: " << CPS::Utils::join(inserted_ids) << std::endl;
  assert(inserted_ids.size() == doc_count);
  // Partially replace documents
  std::vector<std::string> replaced_docs_vector;
  replaced_docs_vector.reserve(docs_vector.size());
  std::transform(docs_vector.begin(), docs_vector.end(), std::back_inserter(replaced_docs_vector), [](std::string doc)
  {
    doc.replace(doc.find(title_placeholder), strlen(title_placeholder), "<title>Test document 2</title>");
    return doc;
  });
  CPS::PartialReplaceRequest partial_replace_req(replaced_docs_vector);
  std::unique_ptr<CPS::PartialReplaceResponse> partial_replace_resp(
      connection().sendRequest<CPS::PartialReplaceResponse>(partial_replace_req));
  auto replaced_ids = partial_replace_resp->getModifiedIds();
  std::sort(replaced_ids.begin(), replaced_ids.end());
  std::cout << "Replaced ids: " << CPS::Utils::join(replaced_ids) << std::endl;
  auto replace_mismatch = std::mismatch(inserted_ids.begin(), inserted_ids.end(), replaced_ids.begin());
  assert(replace_mismatch.first == inserted_ids.end());
  assert(replace_mismatch.second == replaced_ids.end());
  // Retrieve documents
  CPS::RetrieveRequest retrieve_req(replaced_ids);
  std::unique_ptr<CPS::RetrieveResponse> retrieve_resp(
      connection().sendRequest<CPS::RetrieveResponse>(retrieve_req));
  auto retrieved_docs = retrieve_resp->getDocumentsXML();
  std::cout << "Retrieved ids: " << CPS::Utils::join(replaced_ids) << std::endl;
  assert(retrieved_docs.size() == doc_count);
  auto doc_mismatch = std::find_if_not(retrieved_docs.begin(), retrieved_docs.end(), [](CPS::XMLDocument* doc)
  {
    auto title_nodes = doc->FindFast("/document/title");
    return (title_nodes.size() == 1 && title_nodes[0]->getValue() == "Test document 2");
  });
  assert(doc_mismatch == retrieved_docs.end());
  // Delete documents
  CPS::DeleteRequest delete_req(inserted_ids);
  std::unique_ptr<CPS::DeleteResponse> delete_resp(
      connection().sendRequest<CPS::DeleteResponse>(delete_req));
  // Print out errors and related document ids
  print_errors(std::cout, delete_resp->getErrors());
  // Print out deleted document ids
  auto deleted_ids = delete_resp->getModifiedIds();
  std::sort(deleted_ids.begin(), deleted_ids.end());
  std::cout << "Delete ids: " << CPS::Utils::join(deleted_ids) << std::endl;
  assert(deleted_ids.size() == doc_count);
  auto delete_mismatch = std::mismatch(inserted_ids.begin(), inserted_ids.end(), deleted_ids.begin());
  assert(delete_mismatch.first == inserted_ids.end());
  assert(delete_mismatch.second == deleted_ids.end());
}

void BasicIOTest::test_insert_many_documents_list_paths_and_delete_them()
{
  static const char* doc_template =
      "<document><id>{ID}</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
  static const char* id_placeholder = "{ID}";
  static const char* title_placeholder = "<title>Test document 1</title>";

  // Generate a list of documents
  std::vector<std::string> docs_vector;
  docs_vector.reserve(10);
  int doc_count = 0;
  std::generate_n(
      std::back_inserter(docs_vector),
      10,
      [&doc_count]()
      {
        std::string doc(doc_template);
        doc.replace(
            doc.find(id_placeholder),
            strlen(id_placeholder),
            make_docid("test_insert_many_documents_partially_replace_them_retrieve_them_and_then_delete_them", doc_count++));
        return doc;
      });
  // Insert documents
  CPS::InsertRequest insert_req(docs_vector);
  std::unique_ptr<CPS::InsertResponse> insert_resp(
      connection().sendRequest<CPS::InsertResponse>(insert_req));
  auto inserted_ids = insert_resp->getModifiedIds();
  std::sort(inserted_ids.begin(), inserted_ids.end());
  std::cout << "Insert ids: " << CPS::Utils::join(inserted_ids) << std::endl;
  assert(inserted_ids.size() == doc_count);
  // List paths
  std::unique_ptr<CPS::ListPathsResponse> list_paths_resp(
      connection().sendRequest<CPS::ListPathsResponse>(CPS::ListPathsRequest()));
  auto paths = list_paths_resp->getPaths();
  std::cout << CPS::Utils::join(paths, "\n");
  // Delete documents
  CPS::DeleteRequest delete_req(inserted_ids);
  std::unique_ptr<CPS::DeleteResponse> delete_resp(
      connection().sendRequest<CPS::DeleteResponse>(delete_req));
  // Print out errors and related document ids
  print_errors(std::cout, delete_resp->getErrors());
  // Print out deleted document ids
  auto deleted_ids = delete_resp->getModifiedIds();
  std::sort(deleted_ids.begin(), deleted_ids.end());
  std::cout << "Delete ids: " << CPS::Utils::join(deleted_ids) << std::endl;
  assert(deleted_ids.size() == doc_count);
  auto delete_mismatch = std::mismatch(inserted_ids.begin(), inserted_ids.end(), deleted_ids.begin());
  assert(delete_mismatch.first == inserted_ids.end());
  assert(delete_mismatch.second == deleted_ids.end());
}

void BasicIOTest::test_insert_two_documents_list_last_and_delete_them()
{
  static const char* doc_template =
      "<document><id>{ID}</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
  static const char* id_placeholder = "{ID}";

  // Generate a list of documents
  std::vector<std::string> docs_vector;
  docs_vector.reserve(2);
  int doc_count = 0;
  std::generate_n(std::back_inserter(docs_vector), 2, [&doc_count]()
  {
    std::string doc(doc_template);
    doc.replace(
        doc.find(id_placeholder),
        strlen(id_placeholder),
        make_docid("test_insert_two_documents_list_last_and_delete_them", doc_count++));
    return doc;
  });
  // Insert documents
  std::vector<std::string> inserted_ids;
  inserted_ids.reserve(docs_vector.size());
  for (auto &doc : docs_vector)
  {
    std::vector<std::string> docs_to_insert = { doc };
    CPS::InsertRequest insert_req(docs_to_insert);
    std::unique_ptr<CPS::InsertResponse> insert_resp(
        connection().sendRequest<CPS::InsertResponse>(insert_req));
    auto inserted_id = insert_resp->getModifiedIds();
    std::cout << "Insert id: " << inserted_id[0] << std::endl;
    assert(inserted_id.size() == 1);
    inserted_ids.push_back(inserted_id[0]);
  }
  // List last
  // return these fields from the documents
  std::map<std::string, std::string> fields;
  fields["/document/id"] = "yes";
  fields["/document/title"] = "yes";
  fields["/document/body"] = "yes";
  CPS::ListLastRequest list_last_req(0, 1, fields);
  std::unique_ptr<CPS::ListLastResponse> list_last_resp(
      connection().sendRequest<CPS::ListLastResponse>(list_last_req));
  auto last_docs = list_last_resp->getDocumentsString();
  assert(last_docs.size() == 1);
  std::cout << "Last doc: " << last_docs[0] << std::endl;
  assert(last_docs[0] == docs_vector.back());
  // Delete documents
  CPS::DeleteRequest delete_req(inserted_ids);
  std::unique_ptr<CPS::DeleteResponse> delete_resp(
      connection().sendRequest<CPS::DeleteResponse>(delete_req));
  // Print out errors and related document ids
  print_errors(std::cout, delete_resp->getErrors());
  // Print out deleted document ids
  auto deleted_ids = delete_resp->getModifiedIds();
  std::sort(deleted_ids.begin(), deleted_ids.end());
  std::cout << "Delete ids: " << CPS::Utils::join(deleted_ids) << std::endl;
  assert(deleted_ids.size() == doc_count);
  std::sort(inserted_ids.begin(), inserted_ids.end());
  auto delete_mismatch = std::mismatch(inserted_ids.begin(), inserted_ids.end(), deleted_ids.begin());
  assert(delete_mismatch.first == inserted_ids.end());
  assert(delete_mismatch.second == deleted_ids.end());
}

void BasicIOTest::test_insert_two_documents_lookup_last_and_delete_them()
{
  static const char* doc_template =
      "<document><id>{ID}</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
  static const char* id_placeholder = "{ID}";

  // Generate a list of documents
  std::vector<std::string> docs_vector;
  docs_vector.reserve(2);
  int doc_count = 0;
  std::generate_n(std::back_inserter(docs_vector), 2, [&doc_count]()
  {
    std::string doc(doc_template);
    doc.replace(
        doc.find(id_placeholder),
        strlen(id_placeholder),
        make_docid("test_insert_two_documents_lookup_last_and_delete_them", doc_count++));
    return doc;
  });
  // Insert documents
  CPS::InsertRequest insert_req(docs_vector);
  std::unique_ptr<CPS::InsertResponse> insert_resp(
      connection().sendRequest<CPS::InsertResponse>(insert_req));
  auto inserted_ids = insert_resp->getModifiedIds();
  std::sort(inserted_ids.begin(), inserted_ids.end());
  std::cout << "Insert ids: " << CPS::Utils::join(inserted_ids) << std::endl;
  assert(inserted_ids.size() == doc_count);
  // Lookup last document
  std::vector<std::string> lookup_ids;
  lookup_ids.push_back(inserted_ids[1]);
  std::map<std::string, std::string> paths;
  paths["/document/id"] = "yes";
  paths["/document/title"] = "yes";
  CPS::LookupRequest lookup_req(lookup_ids, paths);
  std::unique_ptr<CPS::LookupResponse> lookup_resp(
      connection().sendRequest<CPS::LookupResponse>(lookup_req));
  std::cout << "Found " << lookup_resp->getFound() << std::endl;
  assert(lookup_resp->getFound() == 1);
  auto retrieved_docs = lookup_resp->getDocumentsXML();
  assert(retrieved_docs.size() == 1);
  auto title_nodes = retrieved_docs[0]->FindFast("/document/title");
  assert(title_nodes.size() == 1);
  assert(title_nodes[0]->getValue() == "Test document 1");
  // Delete documents
  CPS::DeleteRequest delete_req(inserted_ids);
  std::unique_ptr<CPS::DeleteResponse> delete_resp(
      connection().sendRequest<CPS::DeleteResponse>(delete_req));
  // Print out errors and related document ids
  print_errors(std::cout, delete_resp->getErrors());
  // Print out deleted document ids
  auto deleted_ids = delete_resp->getModifiedIds();
  std::sort(deleted_ids.begin(), deleted_ids.end());
  std::cout << "Delete ids: " << CPS::Utils::join(deleted_ids) << std::endl;
  assert(deleted_ids.size() == doc_count);
  auto delete_mismatch = std::mismatch(inserted_ids.begin(), inserted_ids.end(), deleted_ids.begin());
  assert(delete_mismatch.first == inserted_ids.end());
  assert(delete_mismatch.second == deleted_ids.end());
}

void BasicIOTest::test_insert_many_documents_search_and_delete_them()
{
  static const char* doc_template =
      "<document><id>{ID}</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
  static const char* id_placeholder = "{ID}";

  // Generate a list of documents
  std::vector<std::string> docs_vector;
  docs_vector.reserve(10);
  int doc_count = 0;
  std::generate_n(std::back_inserter(docs_vector), 10, [&doc_count]()
  {
    std::string doc(doc_template);
    doc.replace(
        doc.find(id_placeholder),
        strlen(id_placeholder),
        make_docid("test_insert_many_documents_search_and_delete_them", doc_count++));
    return doc;
  });
  // Insert documents
  CPS::InsertRequest insert_req(docs_vector);
  std::unique_ptr<CPS::InsertResponse> insert_resp(
      connection().sendRequest<CPS::InsertResponse>(insert_req));
  auto inserted_ids = insert_resp->getModifiedIds();
  std::sort(inserted_ids.begin(), inserted_ids.end());
  std::cout << "Insert ids: " << CPS::Utils::join(inserted_ids) << std::endl;
  assert(inserted_ids.size() == doc_count);
  // Search documents by title
  std::string query = "<title>Test document 1</title>";
  std::map<std::string, std::string> fields;
  fields["/document/id"] = "yes";
  fields["/document/title"] = "yes";
  CPS::SearchRequest search_req(query, 0, 100, fields);
  std::unique_ptr<CPS::SearchResponse> search_resp(
      connection().sendRequest<CPS::SearchResponse>(search_req));
  std::cout << "Hits " << search_resp->getHits() << std::endl;
  assert(search_resp->getHits() == doc_count);
  auto retrieved_docs = search_resp->getDocumentsXML();
  assert(retrieved_docs.size() == doc_count);
  auto doc_mismatch = std::find_if_not(retrieved_docs.begin(), retrieved_docs.end(), [](CPS::XMLDocument* doc)
  {
    auto title_nodes = doc->FindFast("/document/title");
    return (title_nodes.size() == 1 && title_nodes[0]->getValue() == "Test document 1");
  });
  assert(doc_mismatch == retrieved_docs.end());
  // Delete documents
  CPS::DeleteRequest delete_req(inserted_ids);
  std::unique_ptr<CPS::DeleteResponse> delete_resp(
      connection().sendRequest<CPS::DeleteResponse>(delete_req));
  // Print out errors and related document ids
  print_errors(std::cout, delete_resp->getErrors());
  // Print out deleted document ids
  auto deleted_ids = delete_resp->getModifiedIds();
  std::sort(deleted_ids.begin(), deleted_ids.end());
  std::cout << "Delete ids: " << CPS::Utils::join(deleted_ids) << std::endl;
  assert(deleted_ids.size() == doc_count);
  auto delete_mismatch = std::mismatch(inserted_ids.begin(), inserted_ids.end(), deleted_ids.begin());
  assert(delete_mismatch.first == inserted_ids.end());
  assert(delete_mismatch.second == deleted_ids.end());
}
