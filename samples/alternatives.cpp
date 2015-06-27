#include "cps/CPS_API.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

int main() {
    try
    {
        CPS::Connection *conn = new CPS::Connection("tcp://127.0.0.1:5550", "storage", "user", "password");

        // We would like to find correct words for "aple", "degre" and "teacer"
        CPS::AlternativesRequest alt_req("aple degre teacer");
        CPS::AlternativesResponse *alt_resp = conn->sendRequest<CPS::AlternativesResponse>(alt_req);

        std::map <std::string, CPS::Alternative> alts = alt_resp->getAlternatives();
        for (std::map <std::string, CPS::Alternative>::const_iterator it = alts.begin(); it != alts.end(); ++it) {
            CPS::Alternative alt = it->second;
            std::cout << "Alternatives to \"" << alt.to << "\" (" << alt.count << ")" << std::endl;
            for (std::vector <CPS::Alternative::Word>::const_iterator w = alt.words.begin(); w != alt.words.end(); ++w) {
                std::cout << "\"" << w->content << "\"";
                std::cout << " count=" << w->count;
                std::cout << " idif=" << w->idif;
                std::cout << " cr=" << w->cr;
                std::cout << " h=" << w->h;
                std::cout << std::endl;
            }
        }

        // Clean Up
        delete alt_resp;
        delete conn;
    }
    catch (CPS::Exception&  e)
    {
        std::cerr << e.what() << endl;
        std::cerr << boost::diagnostic_information(e);
    }

    return 0;
}
