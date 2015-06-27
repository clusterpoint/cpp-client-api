#include "cps/CPS_API.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

int main() {
    try
    {
        CPS::Connection *conn = new CPS::Connection("tcp://127.0.0.1:5550", "storage", "user", "password");

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
        insert_req.setDocument("id1", "<document><title>Test document 3</title><body>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam a nisl magna</body></document>");

        // Send request
        try {
            CPS::InsertResponse *insert_resp = conn->sendRequest<CPS::InsertResponse>(insert_req);
            // Print out modified ids
            std::cout << "Inserted ids: " << CPS::Utils::join(insert_resp->getModifiedIds()) << std::endl;
            delete insert_resp;
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

        // Update single document
        CPS::UpdateRequest update_req("<document><id>id1</id><title>Changed test document 1</title><body>Lorem ipsum dolor sit amet,  consectetur adipiscing elit. Nullam a nisl  magna</body></document>");
        CPS::UpdateResponse *update_resp = conn->sendRequest<CPS::UpdateResponse>(update_req);

        // Print out modified ids
        std::cout << "Updated ids: " << CPS::Utils::join(update_resp->getModifiedIds()) << std::endl;

        // Clean Up
        delete update_resp;
        delete conn;
    }
    catch (CPS::Exception&  e)
    {
        std::cerr << e.what() << endl;
        std::cerr << boost::diagnostic_information(e);
    }

    return 0;
}
