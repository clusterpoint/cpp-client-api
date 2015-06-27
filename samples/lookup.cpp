#include "cps/CPS_API.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

int main() {
    try
    {
        CPS::Connection *conn = new CPS::Connection("tcp://127.0.0.1:5550", "storage", "user", "password");

        // Set two ids to lookup
        std::vector <std::string> ids;
        ids.push_back("id1");
        ids.push_back("id2");
        // Return just document id and title in found documents
        std::map <std::string, std::string> list;
        list["document/id"] = "yes";
        list["document/title"] = "yes";

        CPS::LookupRequest lookup_req(ids, list);
        CPS::LookupResponse *lookup_resp = conn->sendRequest<CPS::LookupResponse>(lookup_req);

        std::cout << "Found " << lookup_resp->getFound() << std::endl;

        // Get list of documents as strings You can parse with your chosen XML parser
        std::vector <std::string> docStrings = lookup_resp->getDocumentsString();

        // Or get list of documents as XMLDocuments,
        // which is a custom XML class using RapidXML as parser
        std::vector<CPS::XMLDocument*> docXML = lookup_resp->getDocumentsXML();

        // Clean Up
        delete lookup_resp;
        delete conn;
    }
    catch (CPS::Exception&  e)
    {
        std::cerr << e.what() << endl;
        std::cerr << boost::diagnostic_information(e);
    }

    return 0;
}
