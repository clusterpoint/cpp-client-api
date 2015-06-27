#include "cps/CPS_API.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

int main() {
    try
    {
        CPS::Connection *conn = new CPS::Connection("tcp://127.0.0.1:5550", "storage", "user", "password");

        // Number of keywords to extract from the source
        int len = 20;
        // Minimum number of keywords matching in the destination
        int quota = 4;

        // Retrieve single document specified by document id
        CPS::SimilarDocumentRequest sim_doc_req("id1", len, quota);
        CPS::SimilarDocumentResponse *sim_doc_resp = conn->sendRequest<CPS::SimilarDocumentResponse>(sim_doc_req);

        if (sim_doc_resp->getHits() > 0) {
            std::cout << "Found: " << sim_doc_resp->getHits() << std::endl;
            // Get list of documents as strings You can parse with your chosen XML parser
            std::vector <std::string> docStrings = sim_doc_resp->getDocumentsString();

            // Or get list of documents as XMLDocuments,
            // which is a custom XML class using RapidXML as parser
            std::vector<CPS::XMLDocument*> docXML = sim_doc_resp->getDocumentsXML();
        }

        // Clean Up
        delete sim_doc_resp;
        delete conn;
    }
    catch (CPS::Exception&  e)
    {
        std::cerr << e.what() << endl;
        std::cerr << boost::diagnostic_information(e);
    }

    return 0;
}
