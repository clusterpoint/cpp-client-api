#include "cps/CPS_API.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

int main() {
    try
    {
        CPS::Connection *conn = new CPS::Connection("tcp://127.0.0.1:5550", "storage", "user", "password");

        CPS::StatusResponse *status_resp = conn->sendRequest<CPS::StatusResponse>(CPS::StatusRequest());

        std::cout << "License: " << status_resp->getLicense() << std::endl;
        std::cout << "Total " << status_resp->getRepository().documents << " documents." << std::endl;

        // Clean Up
        delete status_resp;
        delete conn;
    }
    catch (CPS::Exception&  e)
    {
        std::cerr << e.what() << endl;
        std::cerr << boost::diagnostic_information(e);
    }

    return 0;
}
