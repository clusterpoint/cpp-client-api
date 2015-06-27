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

        CPS::SearchRequest search_req(query, offset, docs);
        // Retrieve top 10 car counts by make
        search_req.setFacet("car_params/make");
        search_req.setFacetSize(10);

        CPS::SearchResponse *search_resp = conn->sendRequest<CPS::SearchResponse>(search_req);

        if (search_resp->getHits() > 0) {
            std::map<std::string, CPS::SearchFacet> facets = search_resp->getFacets();
            for (std::map<std::string, CPS::SearchFacet>::const_iterator facet = facets.begin(); facet != facets.end(); ++facet) {
                std::vector<CPS::SearchFacet::Term> terms = facet->second.terms;
                for (std::vector<CPS::SearchFacet::Term>::const_iterator term = terms.begin(); term != terms.end(); ++term) {
                    std::cout << "Cars made by " << term->name << " count" << term->hits << std::endl;
                }
            }
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
