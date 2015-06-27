#include "cps/CPS_API.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

int main() {
    try
    {
        CPS::Connection *conn = new CPS::Connection("tcp://127.0.0.1:5550", "storage", "user", "password");

        // We want to return all manufacturers and all available colours for all cars
        std::vector <string> paths;
        paths.push_back("facet/make");
        paths.push_back("facet/colour");

        CPS::ListFacetsRequest lfacets_req(paths);
        CPS::ListFacetsResponse *lfacets_resp = conn->sendRequest<CPS::ListFacetsResponse>(lfacets_req);

        std::map <std::string, std::vector <std::string> > facets = lfacets_resp->getFacets();
        for (std::map <std::string, std::vector <std::string> >::const_iterator it = facets.begin(); it != facets.end(); ++it) {
            std::cout << "Facets for path: " << it->first << std::endl;
            std::cout << CPS::Utils::join(it->second) << std::endl;
        }

        // Clean Up
        delete lfacets_resp;
        delete conn;
    }
    catch (CPS::Exception&  e)
    {
        std::cerr << e.what() << endl;
        std::cerr << boost::diagnostic_information(e);
    }

    return 0;
}
