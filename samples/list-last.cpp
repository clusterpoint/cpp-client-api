#include "cps/CPS_API.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

int main() {
    try
    {
        CPS::Connection *conn = new CPS::Connection("tcp://127.0.0.1:5550", "storage", "user", "password");

        // Set offset from which to return documents
        int offset = 0;
        // Set documents per reply
        int docs = 10;

        CPS::ListLastRequest retr_req(offset, docs);
        CPS::ListLastResponse *retr_resp = conn->sendRequest<CPS::ListLastResponse>(retr_req);

        // Get list of documents as strings You can parse with your chosen XML parser
        std::vector <std::string> docStrings = retr_resp->getDocumentsString();

        // Or get list of documents as XMLDocuments,
        // which is a custom XML class using RapidXML as parser
        std::vector<CPS::XMLDocument*> docXML = retr_resp->getDocumentsXML();

        // Clean Up
        delete retr_resp;
        delete conn;
    }
    catch (CPS::Exception&  e)
    {
        std::cerr << e.what() << endl;
        std::cerr << boost::diagnostic_information(e);
    }

    return 0;
}
