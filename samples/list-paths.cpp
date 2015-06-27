#include "cps/CPS_API.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

int main() {
    try
    {
        CPS::Connection *conn = new CPS::Connection("tcp://127.0.0.1:5550", "storage", "user", "password");

        CPS::ListPathsResponse *lpaths_resp = conn->sendRequest<CPS::ListPathsResponse>(CPS::ListPathsRequest());

        std::cout << CPS::Utils::join(lpaths_resp->getPaths(), "\n");

        // Clean Up
        delete lpaths_resp;
        delete conn;
    }
    catch (CPS::Exception&  e)
    {
        std::cerr << e.what() << endl;
        std::cerr << boost::diagnostic_information(e);
    }

    return 0;
}
