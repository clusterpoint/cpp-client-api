#include "cps/CPS_API.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

int main() {
    try
    {
        CPS::Connection *conn = new CPS::Connection("tcp://127.0.0.1:5550", "storage", "user", "password");

        // Find all words starting with "apl" and "casd"
        CPS::ListWordsRequest lwords_req("apl* casd*");
        CPS::ListWordsResponse *lwords_resp = conn->sendRequest<CPS::ListWordsResponse>(lwords_req);

        std::map <std::string, std::map <std::string, int> > words = lwords_resp->getWords();
        for (std::map <std::string, std::map <std::string, int> >::const_iterator it = words.begin(); it != words.end(); ++it) {
            std::cout << "List for " << it->first << std::endl;
            for (std::map <std::string, int>::const_iterator w = it->second.begin(); w != it->second.end(); ++w) {
                std::cout << "Word " << w->first << " [" << w->second << "]" << std::endl;
            }
        }

        // Clean Up
        delete lwords_resp;
        delete conn;
    }
    catch (CPS::Exception&  e)
    {
        std::cerr << e.what() << endl;
        std::cerr << boost::diagnostic_information(e);
    }

    return 0;
}
