#include "cps/CPS_API.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

int main() {
    try
    {
        CPS::Connection *conn = new CPS::Connection("tcp://127.0.0.1:5550", "storage", "user", "password");

        // deletes all documents with an "expired" tag that contains a 1
        CPS::SearchDeleteRequest sdelete_req("<expired>1</expired>");
        CPS::SearchDeleteResponse *sdelete_resp = conn->sendRequest<CPS::SearchDeleteResponse>(sdelete_req);

        // Print out deleted ids
        std::cout << "Total documents deleted: " << sdelete_resp->getHits() << std::endl;

        // Clean Up
        delete sdelete_resp;
        delete conn;
    }
    catch (CPS::Exception&  e)
    {
        std::cerr << e.what() << endl;
        std::cerr << boost::diagnostic_information(e);
    }

    return 0;
}
