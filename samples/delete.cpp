#include "cps/CPS_API.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>

int main() {
  try 
  {
      CPS::Connection *conn = new CPS::Connection("tcp://127.0.0.1:5550", "storage", "user", "password");

      // Deleting two documents with id1 and id2
      std::vector <std::string> ids;
      ids.push_back("id1");
      ids.push_back("id2");
      CPS::DeleteRequest delete_req(ids);
      CPS::DeleteResponse *delete_resp = conn->sendRequest<CPS::DeleteResponse>(delete_req);

      // Print out document ids which did not exist
      std::vector <CPS::Error> errors = delete_resp->getErrors();
      for (std::vector<CPS::Error>::const_iterator it = errors.begin(); it != errors.end(); ++it) {
          if (it->code == "2824") { // Requested document does not exist error
              std::cout << it->text << ": " << CPS::Utils::join(it->documentIds) << std::endl;
          }
      }
      // Print out deleted ids
      std::cout << "Deleted ids: " << CPS::Utils::join(delete_resp->getModifiedIds()) << std::endl;

      // You can also write this in single line and do clean up if you are not interested in response
      delete conn->sendRequest<CPS::DeleteResponse>(CPS::DeleteRequest("id3"));

      // Clean Up
      delete delete_resp;
    delete conn;
  }
  catch (CPS::Exception&  e)
  {
      std::cerr << e.what() << endl;
      std::cerr << boost::diagnostic_information(e);
  }
  
  return 0;
}
