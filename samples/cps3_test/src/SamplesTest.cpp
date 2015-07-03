#include "SamplesTest.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <thread>

SamplesTest::SamplesTest(CPS::Connection& connection)
	: TestCase(connection)
{
}

void SamplesTest::set_up() {
	connection().setDebug(false);
	std::unique_ptr<CPS::Response> resp(
			connection().sendRequest<CPS::Response>(CPS::Request("clear")));
	assert(!resp->hasFailed());
	// Clear command deletes database. It may take some time until a new
	// database is created. So, commands will return errors for a while.
	// We will use Search command to find out, when the database is ready
	// to receive any further commands.
	for (int i = 0; i < 120; i += 1) {
		try {
			std::unique_ptr<CPS::SearchResponse> search_resp(
					connection().sendRequest<CPS::SearchResponse>(CPS::SearchRequest("*")));
			if (!search_resp->hasFailed() && search_resp->getFound() == 0) {
				break;
			}
		} catch (CPS::Exception& e) {
		}
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}
	connection().setDebug(true);
}

void SamplesTest::tear_down() {
}

void SamplesTest::run_tests() {
	RUN_TEST(test_insert_sample);
	RUN_TEST(test_update_sample);
	RUN_TEST(test_reindex_sample);
	RUN_TEST(test_replace_sample);
	RUN_TEST(test_partial_replace_sample);
	RUN_TEST(test_delete_sample);
	RUN_TEST(test_search_sample);
	RUN_TEST(test_lookup_sample);
	RUN_TEST(test_retrieve_sample);
	RUN_TEST(test_list_last_sample);
	RUN_TEST(test_list_paths_sample);
	RUN_TEST(test_status_sample);
	RUN_TEST(test_clear_sample);

	connection().setDebug(false);
	// Clear command deletes database. It may take some time until a new
	// database is created. So, commands will return errors for a while.
	// We will use Search command to find out, when the database is ready
	// to receive any further commands.
	for (int i = 0; i < 120; i += 1) {
		try {
			std::unique_ptr<CPS::SearchResponse> search_resp(
					connection().sendRequest<CPS::SearchResponse>(CPS::SearchRequest("*")));
			if (!search_resp->hasFailed() && search_resp->getFound() == 0) {
				break;
			}
		} catch (CPS::Exception& e) {
		}
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}
	connection().setDebug(true);
}

void SamplesTest::test_insert_sample() {
	CPS::Connection *conn = &connection();

	// There are four ways documents can be passed:
	// 1) document id and document body -> CPS::InsertRequest(string, string)
	// 2) document string with id included* -> CPS::InsertRequest(string)
	// 3) vector of documents with ids included* -> CPS::InsertRequest(vector<string>)
	// 3) map of documents with key as id and value as document body -> CPS::InsertRequest(map<string, string>)
	// * If policy includes autoincremented id then id can be skipped.

	// Inserting test documents. The same approach could be used also for update/replace/partial-replace commands
	std::vector <std::string> docs_vector;
	docs_vector.push_back("<document><id>id1</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	docs_vector.push_back("<document><id>id2</id><title>Test document 2</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	std::map <std::string, std::string> docs_map;
	docs_map["id3"] = "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
	docs_map["id4"] = "<document><title>Test document 4</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";

	// When creating modify requests documents must be passed to constructor
	CPS::InsertRequest insert_req(docs_vector);
	// More documents can also be added after insert request object is created
	// More document adding allows previously mentioned types
	insert_req.setDocuments(docs_map);
	insert_req.setDocument("<document><id>id5</id><title>Test document 5</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	insert_req.setDocument("id6", "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");

	// Send request
	try {
		std::unique_ptr<CPS::InsertResponse> insert_resp(conn->sendRequest<CPS::InsertResponse>(insert_req));
		// Print out modified ids
		std::cout << "Inserted ids: " << CPS::Utils::join(insert_resp->getModifiedIds()) << std::endl;
	} catch (CPS::Exception &e) {
		if (e.errorCode == 2626 && e.getResponse()) {
			// Print out duplicate ids
			std::vector <CPS::Error> errors = e.getResponse()->getErrors();
			for (std::vector<CPS::Error>::const_iterator it = errors.begin(); it != errors.end(); ++it) {
				if (it->code == "2626") { // Duplicate primary key error
					std::cout << it->text << ": " << CPS::Utils::join(it->documentIds) << std::endl;
				}
			}
		}
	}
}

void SamplesTest::test_update_sample() {
	CPS::Connection *conn = &connection();

	// There are four ways documents can be passed:
	// 1) document id and document body -> CPS::InsertRequest(string, string)
	// 2) document string with id included* -> CPS::InsertRequest(string)
	// 3) vector of documents with ids included* -> CPS::InsertRequest(vector<string>)
	// 3) map of documents with key as id and value as document body -> CPS::InsertRequest(map<string, string>)
	// * If policy includes autoincremented id then id can be skipped.

	// Inserting test documents. The same approach could be used also for update/replace/partial-replace commands
	std::vector <std::string> docs_vector;
	docs_vector.push_back("<document><id>id1</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	docs_vector.push_back("<document><id>id2</id><title>Test document 2</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	std::map <std::string, std::string> docs_map;
	docs_map["id3"] = "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
	docs_map["id4"] = "<document><title>Test document 4</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";

	// When creating modify requests documents must be passed to constructor
	CPS::InsertRequest insert_req(docs_vector);
	// More documents can also be added after insert request object is created
	// More document adding allows previously mentioned types
	insert_req.setDocuments(docs_map);
	insert_req.setDocument("<document><id>id5</id><title>Test document 5</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	insert_req.setDocument("id6", "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");

	std::unique_ptr<CPS::InsertResponse> insert_resp(conn->sendRequest<CPS::InsertResponse>(insert_req));

	// Update single document
	CPS::UpdateRequest update_req("<document><id>id1</id><title>Changed test document 1</title><body>Lorem ipsum dolor sit amet,  consectetur adipiscing elit. Nullam a nisl  magna</body></document>");
	std::unique_ptr<CPS::UpdateResponse> update_resp(conn->sendRequest<CPS::UpdateResponse>(update_req));

	// Print out modified ids
	std::cout << "Updated ids: " << CPS::Utils::join(update_resp->getModifiedIds()) << std::endl;
}

void SamplesTest::test_replace_sample() {
	CPS::Connection *conn = &connection();

	// There are four ways documents can be passed:
	// 1) document id and document body -> CPS::InsertRequest(string, string)
	// 2) document string with id included* -> CPS::InsertRequest(string)
	// 3) vector of documents with ids included* -> CPS::InsertRequest(vector<string>)
	// 3) map of documents with key as id and value as document body -> CPS::InsertRequest(map<string, string>)
	// * If policy includes autoincremented id then id can be skipped.

	// Inserting test documents. The same approach could be used also for update/replace/partial-replace commands
	std::vector <std::string> docs_vector;
	docs_vector.push_back("<document><id>id1</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	docs_vector.push_back("<document><id>id2</id><title>Test document 2</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	std::map <std::string, std::string> docs_map;
	docs_map["id3"] = "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
	docs_map["id4"] = "<document><title>Test document 4</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";

	// When creating modify requests documents must be passed to constructor
	CPS::InsertRequest insert_req(docs_vector);
	// More documents can also be added after insert request object is created
	// More document adding allows previously mentioned types
	insert_req.setDocuments(docs_map);
	insert_req.setDocument("<document><id>id5</id><title>Test document 5</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	insert_req.setDocument("id6", "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");

	std::unique_ptr<CPS::InsertResponse> insert_resp(conn->sendRequest<CPS::InsertResponse>(insert_req));

	// Replace single document
	CPS::ReplaceRequest replace_req("<document><id>id1</id><title>Changed test document 1</title><body>Lorem ipsum dolor sit amet,  consectetur adipiscing elit. Nullam a nisl  magna</body></document>");
	std::unique_ptr<CPS::ReplaceResponse> replace_resp(conn->sendRequest<CPS::ReplaceResponse>(replace_req));
	// Print out modified ids
	std::cout << "Replaced ids: " << CPS::Utils::join(replace_resp->getModifiedIds()) << std::endl;
}

void SamplesTest::test_partial_replace_sample() {
	CPS::Connection *conn = &connection();

	// There are four ways documents can be passed:
	// 1) document id and document body -> CPS::InsertRequest(string, string)
	// 2) document string with id included* -> CPS::InsertRequest(string)
	// 3) vector of documents with ids included* -> CPS::InsertRequest(vector<string>)
	// 3) map of documents with key as id and value as document body -> CPS::InsertRequest(map<string, string>)
	// * If policy includes autoincremented id then id can be skipped.

	// Inserting test documents. The same approach could be used also for update/replace/partial-replace commands
	std::vector <std::string> docs_vector;
	docs_vector.push_back("<document><id>id1</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	docs_vector.push_back("<document><id>id2</id><title>Test document 2</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	std::map <std::string, std::string> docs_map;
	docs_map["id3"] = "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
	docs_map["id4"] = "<document><title>Test document 4</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";

	// When creating modify requests documents must be passed to constructor
	CPS::InsertRequest insert_req(docs_vector);
	// More documents can also be added after insert request object is created
	// More document adding allows previously mentioned types
	insert_req.setDocuments(docs_map);
	insert_req.setDocument("<document><id>id5</id><title>Test document 5</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	insert_req.setDocument("id6", "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");

	std::unique_ptr<CPS::InsertResponse> insert_resp(conn->sendRequest<CPS::InsertResponse>(insert_req));

	// Partial replace single document
	CPS::PartialReplaceRequest partial_replace_req("id1", "<title>Changed test document 1</title>");
	std::unique_ptr<CPS::PartialReplaceResponse> partial_replace_resp(conn->sendRequest<CPS::UpdateResponse>(partial_replace_req));
	// Print out modified ids
	std::cout << "Partial replaced ids: " << CPS::Utils::join(partial_replace_resp->getModifiedIds()) << std::endl;
}

void SamplesTest::test_delete_sample() {
	CPS::Connection *conn = &connection();

	// There are four ways documents can be passed:
	// 1) document id and document body -> CPS::InsertRequest(string, string)
	// 2) document string with id included* -> CPS::InsertRequest(string)
	// 3) vector of documents with ids included* -> CPS::InsertRequest(vector<string>)
	// 3) map of documents with key as id and value as document body -> CPS::InsertRequest(map<string, string>)
	// * If policy includes autoincremented id then id can be skipped.

	// Inserting test documents. The same approach could be used also for update/replace/partial-replace commands
	std::vector <std::string> docs_vector;
	docs_vector.push_back("<document><id>id1</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	docs_vector.push_back("<document><id>id2</id><title>Test document 2</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	std::map <std::string, std::string> docs_map;
	docs_map["id3"] = "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
	docs_map["id4"] = "<document><title>Test document 4</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";

	// When creating modify requests documents must be passed to constructor
	CPS::InsertRequest insert_req(docs_vector);
	// More documents can also be added after insert request object is created
	// More document adding allows previously mentioned types
	insert_req.setDocuments(docs_map);
	insert_req.setDocument("<document><id>id5</id><title>Test document 5</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	insert_req.setDocument("id6", "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");

	std::unique_ptr<CPS::InsertResponse> insert_resp(conn->sendRequest<CPS::InsertResponse>(insert_req));

	// Deleting two documents with id1 and id2
	std::vector <std::string> ids;
	ids.push_back("id1");
	ids.push_back("id2");
	CPS::DeleteRequest delete_req(ids);
	std::unique_ptr<CPS::DeleteResponse> delete_resp(conn->sendRequest<CPS::DeleteResponse>(delete_req));

	// Print out document ids which did not exist
	std::vector <CPS::Error> errors = delete_resp->getErrors();
	for (std::vector<CPS::Error>::const_iterator it = errors.begin(); it != errors.end(); ++it) {
		if (it->code == "2824") { // Requested document does not exist error
			std::cout << it->text << ": " << CPS::Utils::join(it->documentIds) << std::endl;
		}
	}
	// Print out deleted ids
	std::cout << "Deleted ids: " << CPS::Utils::join(delete_resp->getModifiedIds()) << std::endl;

	// You can also write this in single line and do clean up if you are not interested in response
	delete conn->sendRequest<CPS::DeleteResponse>(CPS::DeleteRequest("id3"));
}

void SamplesTest::test_search_sample() {
	CPS::Connection *conn = &connection();

	// There are four ways documents can be passed:
	// 1) document id and document body -> CPS::InsertRequest(string, string)
	// 2) document string with id included* -> CPS::InsertRequest(string)
	// 3) vector of documents with ids included* -> CPS::InsertRequest(vector<string>)
	// 3) map of documents with key as id and value as document body -> CPS::InsertRequest(map<string, string>)
	// * If policy includes autoincremented id then id can be skipped.

	// Inserting test documents. The same approach could be used also for update/replace/partial-replace commands
	std::vector <std::string> docs_vector;
	docs_vector.push_back("<document><id>id1</id><category>cars</category><car_params><make>Ford</make><model>Mustang</model><year>2012</year></car_params></document>");
	docs_vector.push_back("<document><id>id2</id><category>cars</category><car_params><make>Dodge</make><model>Viper</model><year>2014</year></car_params></document>");
	std::map <std::string, std::string> docs_map;
	docs_map["id3"] = "<document><id>id3</id><category>cars</category><car_params><make>Lamborghini</make><model>Diablo</model><year>2010</year></car_params></document>";
	docs_map["id4"] = "<document><id>id4</id><category>cars</category><car_params><make>Ferrary</make><model>Testarossa</model><year>1986</year></car_params></document>";

	// When creating modify requests documents must be passed to constructor
	CPS::InsertRequest insert_req(docs_vector);
	// More documents can also be added after insert request object is created
	// More document adding allows previously mentioned types
	insert_req.setDocuments(docs_map);
	insert_req.setDocument("<document><id>id5</id><category>cars</category><car_params><make>McLaren</make><model>F1</model><year>2010</year></car_params></document>");
	insert_req.setDocument("id6", "<document><id>id6</id><category>cars</category><car_params><make>Volkswagen</make><model>Beetle</model><year>1968</year></car_params></document>");

	std::unique_ptr<CPS::InsertResponse> insert_resp(conn->sendRequest<CPS::InsertResponse>(insert_req));

	// Setting parameters
	// search for items with category == "cars" and car_params/year >= 2010
	std::string query = "<category>cars</category><car_params><year>&gt;=2010</year></car_params>";

	// return documents starting with the first one - offset 0
	int offset = 0;
	// return not more than 5 documents
	int docs = 5;
	// return these fields from the documents
	std::map <std::string, std::string> list;
	list["id"] = "yes";
	list["car_params/make"] = "yes";
	list["car_params/model"] = "yes";
	list["car_params/year"] = "yes";

	// order by year, from largest to smallest
	string ordering = CPS::Ordering::NumericOrdering("car_params/year", CPS::Ordering::Descending);

	CPS::SearchRequest search_req(query, offset, docs, list);
	search_req.setOrdering(ordering);
	std::unique_ptr<CPS::SearchResponse> search_resp(conn->sendRequest<CPS::SearchResponse>(search_req));

	if (search_resp->getHits() > 0) {
		std::cout << "Found: " << search_resp->getHits() << std::endl;
		// Get list of documents as strings You can parse with your chosen XML parser
		std::vector <std::string> docStrings = search_resp->getDocumentsString();

		// Or get list of documents as XMLDocuments,
		// which is a custom XML class using RapidXML as parser
		std::vector<CPS::XMLDocument*> docXML = search_resp->getDocumentsXML();
	}
}

void SamplesTest::test_lookup_sample() {
	CPS::Connection *conn = &connection();

	// There are four ways documents can be passed:
	// 1) document id and document body -> CPS::InsertRequest(string, string)
	// 2) document string with id included* -> CPS::InsertRequest(string)
	// 3) vector of documents with ids included* -> CPS::InsertRequest(vector<string>)
	// 3) map of documents with key as id and value as document body -> CPS::InsertRequest(map<string, string>)
	// * If policy includes autoincremented id then id can be skipped.

	// Inserting test documents. The same approach could be used also for update/replace/partial-replace commands
	std::vector <std::string> docs_vector;
	docs_vector.push_back("<document><id>id1</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	docs_vector.push_back("<document><id>id2</id><title>Test document 2</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	std::map <std::string, std::string> docs_map;
	docs_map["id3"] = "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
	docs_map["id4"] = "<document><title>Test document 4</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";

	// When creating modify requests documents must be passed to constructor
	CPS::InsertRequest insert_req(docs_vector);
	// More documents can also be added after insert request object is created
	// More document adding allows previously mentioned types
	insert_req.setDocuments(docs_map);
	insert_req.setDocument("<document><id>id5</id><title>Test document 5</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	insert_req.setDocument("id6", "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");

	std::unique_ptr<CPS::InsertResponse> insert_resp(conn->sendRequest<CPS::InsertResponse>(insert_req));

	// Set two ids to lookup
	std::vector <std::string> ids;
	ids.push_back("id1");
	ids.push_back("id2");
	// Return just document id and title in found documents
	std::map <std::string, std::string> list;
	list["document/id"] = "yes";
	list["document/title"] = "yes";

	CPS::LookupRequest lookup_req(ids, list);
	std::unique_ptr<CPS::LookupResponse> lookup_resp(conn->sendRequest<CPS::LookupResponse>(lookup_req));

	std::cout << "Found " << lookup_resp->getFound() << std::endl;

	// Get list of documents as strings You can parse with your chosen XML parser
	std::vector <std::string> docStrings = lookup_resp->getDocumentsString();

	// Or get list of documents as XMLDocuments,
	// which is a custom XML class using RapidXML as parser
	std::vector<CPS::XMLDocument*> docXML = lookup_resp->getDocumentsXML();
}

void SamplesTest::test_retrieve_sample() {
	CPS::Connection *conn = &connection();

	// There are four ways documents can be passed:
	// 1) document id and document body -> CPS::InsertRequest(string, string)
	// 2) document string with id included* -> CPS::InsertRequest(string)
	// 3) vector of documents with ids included* -> CPS::InsertRequest(vector<string>)
	// 3) map of documents with key as id and value as document body -> CPS::InsertRequest(map<string, string>)
	// * If policy includes autoincremented id then id can be skipped.

	// Inserting test documents. The same approach could be used also for update/replace/partial-replace commands
	std::vector <std::string> docs_vector;
	docs_vector.push_back("<document><id>id1</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	docs_vector.push_back("<document><id>id2</id><title>Test document 2</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	std::map <std::string, std::string> docs_map;
	docs_map["id3"] = "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
	docs_map["id4"] = "<document><title>Test document 4</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";

	// When creating modify requests documents must be passed to constructor
	CPS::InsertRequest insert_req(docs_vector);
	// More documents can also be added after insert request object is created
	// More document adding allows previously mentioned types
	insert_req.setDocuments(docs_map);
	insert_req.setDocument("<document><id>id5</id><title>Test document 5</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	insert_req.setDocument("id6", "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");

	std::unique_ptr<CPS::InsertResponse> insert_resp(conn->sendRequest<CPS::InsertResponse>(insert_req));

	// Retrieve single document specified by document id
	CPS::RetrieveRequest retr_req("id1");
	std::unique_ptr<CPS::RetrieveResponse> retr_resp(conn->sendRequest<CPS::RetrieveResponse>(retr_req));


	// Get list of documents as strings You can parse with your chosen XML parser
	std::vector <std::string> docStrings = retr_resp->getDocumentsString();

	// Or get list of documents as XMLDocuments,
	// which is a custom XML class using RapidXML as parser
	std::vector<CPS::XMLDocument*> docXML = retr_resp->getDocumentsXML();
}

void SamplesTest::test_list_last_sample() {
	CPS::Connection *conn = &connection();

	// There are four ways documents can be passed:
	// 1) document id and document body -> CPS::InsertRequest(string, string)
	// 2) document string with id included* -> CPS::InsertRequest(string)
	// 3) vector of documents with ids included* -> CPS::InsertRequest(vector<string>)
	// 3) map of documents with key as id and value as document body -> CPS::InsertRequest(map<string, string>)
	// * If policy includes autoincremented id then id can be skipped.

	// Inserting test documents. The same approach could be used also for update/replace/partial-replace commands
	std::vector <std::string> docs_vector;
	docs_vector.push_back("<document><id>id1</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	docs_vector.push_back("<document><id>id2</id><title>Test document 2</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	std::map <std::string, std::string> docs_map;
	docs_map["id3"] = "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
	docs_map["id4"] = "<document><title>Test document 4</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";

	// When creating modify requests documents must be passed to constructor
	CPS::InsertRequest insert_req(docs_vector);
	// More documents can also be added after insert request object is created
	// More document adding allows previously mentioned types
	insert_req.setDocuments(docs_map);
	insert_req.setDocument("<document><id>id5</id><title>Test document 5</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	insert_req.setDocument("id6", "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");

	std::unique_ptr<CPS::InsertResponse> insert_resp(conn->sendRequest<CPS::InsertResponse>(insert_req));

	// Set offset from which to return documents
	int offset = 0;
	// Set documents per reply
	int docs = 10;

	CPS::ListLastRequest retr_req(offset, docs);
	std::unique_ptr<CPS::ListLastResponse> retr_resp(conn->sendRequest<CPS::ListLastResponse>(retr_req));

	// Get list of documents as strings You can parse with your chosen XML parser
	std::vector <std::string> docStrings = retr_resp->getDocumentsString();

	// Or get list of documents as XMLDocuments,
	// which is a custom XML class using RapidXML as parser
	std::vector<CPS::XMLDocument*> docXML = retr_resp->getDocumentsXML();
}

void SamplesTest::test_list_paths_sample() {
	CPS::Connection *conn = &connection();

	// There are four ways documents can be passed:
	// 1) document id and document body -> CPS::InsertRequest(string, string)
	// 2) document string with id included* -> CPS::InsertRequest(string)
	// 3) vector of documents with ids included* -> CPS::InsertRequest(vector<string>)
	// 3) map of documents with key as id and value as document body -> CPS::InsertRequest(map<string, string>)
	// * If policy includes autoincremented id then id can be skipped.

	// Inserting test documents. The same approach could be used also for update/replace/partial-replace commands
	std::vector <std::string> docs_vector;
	docs_vector.push_back("<document><id>id1</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	docs_vector.push_back("<document><id>id2</id><title>Test document 2</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	std::map <std::string, std::string> docs_map;
	docs_map["id3"] = "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
	docs_map["id4"] = "<document><title>Test document 4</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";

	// When creating modify requests documents must be passed to constructor
	CPS::InsertRequest insert_req(docs_vector);
	// More documents can also be added after insert request object is created
	// More document adding allows previously mentioned types
	insert_req.setDocuments(docs_map);
	insert_req.setDocument("<document><id>id5</id><title>Test document 5</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	insert_req.setDocument("id6", "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");

	std::unique_ptr<CPS::InsertResponse> insert_resp(conn->sendRequest<CPS::InsertResponse>(insert_req));

	std::unique_ptr<CPS::ListPathsResponse> lpaths_resp(conn->sendRequest<CPS::ListPathsResponse>(CPS::ListPathsRequest()));

	std::cout << CPS::Utils::join(lpaths_resp->getPaths(), "\n");
}

void SamplesTest::test_status_sample() {
	CPS::Connection *conn = &connection();

	// There are four ways documents can be passed:
	// 1) document id and document body -> CPS::InsertRequest(string, string)
	// 2) document string with id included* -> CPS::InsertRequest(string)
	// 3) vector of documents with ids included* -> CPS::InsertRequest(vector<string>)
	// 3) map of documents with key as id and value as document body -> CPS::InsertRequest(map<string, string>)
	// * If policy includes autoincremented id then id can be skipped.

	// Inserting test documents. The same approach could be used also for update/replace/partial-replace commands
	std::vector <std::string> docs_vector;
	docs_vector.push_back("<document><id>id1</id><title>Test document 1</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	docs_vector.push_back("<document><id>id2</id><title>Test document 2</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	std::map <std::string, std::string> docs_map;
	docs_map["id3"] = "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";
	docs_map["id4"] = "<document><title>Test document 4</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>";

	// When creating modify requests documents must be passed to constructor
	CPS::InsertRequest insert_req(docs_vector);
	// More documents can also be added after insert request object is created
	// More document adding allows previously mentioned types
	insert_req.setDocuments(docs_map);
	insert_req.setDocument("<document><id>id5</id><title>Test document 5</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");
	insert_req.setDocument("id6", "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");

	std::unique_ptr<CPS::InsertResponse> insert_resp(conn->sendRequest<CPS::InsertResponse>(insert_req));

	std::unique_ptr<CPS::StatusResponse> status_resp(conn->sendRequest<CPS::StatusResponse>(CPS::StatusRequest()));

	std::cout << "License: " << status_resp->getLicense() << std::endl;
	std::cout << "Total " << status_resp->getRepository().documents << " documents." << std::endl;
}

void SamplesTest::test_clear_sample() {
	CPS::Connection *conn = &connection();

	std::unique_ptr<CPS::Response> resp(conn->sendRequest<CPS::Response>(CPS::Request("clear")));

	std::cout << "Clear done in: " << resp->getSeconds() << std::endl;
}

void SamplesTest::test_reindex_sample() {
	CPS::Connection *conn = &connection();

	std::unique_ptr<CPS::Response> resp(conn->sendRequest<CPS::Response>(CPS::Request("reindex")));

	std::cout << "Reindex initialized in: " << resp->getSeconds() << std::endl;
}
