#include "cps/CPS_API.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

int main() {
    try
    {
        CPS::Connection *conn = new CPS::Connection("unix:///usr/local/cps2/storages/example/storage.sock", "example", "user", "password");

        // Storage backup
        CPS::Request req("restore");
        // Setting the request type to single - this is mandatory if You are working in a cluster environment
        req.setRequestType("single");
        // Setting the path of the file to restore from
        req.setParam("file", "/tmp/example-storage-backup.tar.gz");

        CPS::Response *resp = conn->sendRequest<CPS::Response>(req);

        if (!resp->getErrors().empty()) {
            // Process errors
        }

        // Clean Up
        delete resp;
        delete conn;
    }
    catch (CPS::Exception&  e)
    {
        std::cerr << e.what() << endl;
        std::cerr << boost::diagnostic_information(e);
    }

    return 0;
}
