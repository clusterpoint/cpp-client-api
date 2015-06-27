#include "cps/CPS_API.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

int main() {
    try
    {
        CPS::Connection *conn = new CPS::Connection("tcp://127.0.0.1:5550", "storage", "user", "password");

        // Retrieve single document specified by document id
        CPS::RetrieveRequest retr_req("id1");
        CPS::RetrieveResponse *retr_resp = conn->sendRequest<CPS::RetrieveResponse>(retr_req);


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
