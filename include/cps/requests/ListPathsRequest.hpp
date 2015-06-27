#ifndef CPS_LISTPATHSREQUEST_HPP
#define CPS_LISTPATHSREQUEST_HPP

#include <string>
#include <vector>
#include <map>

#include "../Request.hpp"
#include "../Utils.hpp"

namespace CPS
{
class ListPathsRequest: public Request
{
public:
    /**
     * Constructs an instance of CPSListPathsRequest
     */
    ListPathsRequest() :
        Request("list-paths") {
    }
    virtual ~ListPathsRequest() {
    }
};
}

#endif //#ifndef CPS_LISTPATHSREQUEST_HPP
