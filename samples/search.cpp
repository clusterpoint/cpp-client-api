#include "cps/CPS_API.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

int main() {
    try
    {
        CPS::Connection *conn = new CPS::Connection("tcp://127.0.0.1:5550", "storage", "user", "password");

        // Setting parameters
        // search for items with category == 'cars' and car_params/year >= 2010
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
        bool ascending = CPS::Ordering::Descending;
        string ordering = CPS::Ordering::NumericOrdering("car_params/year", ascending);

        CPS::SearchRequest search_req(query, offset, docs, list);
        search_req.setOrdering(ordering);
        CPS::SearchResponse *search_resp = conn->sendRequest<CPS::SearchResponse>(search_req);

        if (search_resp->getHits() > 0) {
            std::cout << "Found: " << search_resp->getHits() << std::endl;
            // Get list of documents as strings You can parse with your chosen XML parser
            std::vector <std::string> docStrings = search_resp->getDocumentsString();

            // Or get list of documents as XMLDocuments,
            // which is a custom XML class using RapidXML as parser
            std::vector<CPS::XMLDocument*> docXML = search_resp->getDocumentsXML();
        }

        // Clean Up
        delete search_resp;
        delete conn;
    }
    catch (CPS::Exception&  e)
    {
        std::cerr << e.what() << endl;
        std::cerr << boost::diagnostic_information(e);
    }

    return 0;
}
