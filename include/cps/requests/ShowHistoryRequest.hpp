#ifndef CPS_SHOWHISTORYREQUEST_HPP
#define CPS_SHOWHISTORYREQUEST_HPP

#include <string>
#include <vector>
#include <map>

#include "../Request.hpp"
#include "../Utils.hpp"

namespace CPS
{
class ShowHistoryRequest: public Request
{
public:
    /**
     * @param id id of document to show history for
     * @param returnDocs should full documents be returned
     */
    ShowHistoryRequest(const std::string &id, bool returnDocs = true) :
        Request("show-history") {
        documentsWithUserId[id] = "";
        if (returnDocs == true)
            setParam("return_doc", "yes");
    }
    virtual ~ShowHistoryRequest() {
    }
};
}

#endif //#ifndef CPS_SHOWHISTORYREQUEST_HPP
