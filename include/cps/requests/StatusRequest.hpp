#ifndef CPS_STATUSREQUEST_HPP
#define CPS_STATUSREQUEST_HPP

#include <string>
#include <vector>
#include <map>

#include "../Request.hpp"
#include "../Utils.hpp"

namespace CPS
{
class StatusRequest: public Request
{
public:
    StatusRequest() :
        Request("status") {
    }
    virtual ~StatusRequest() {
    }
};
}

#endif //#ifndef CPS_STATUSREQUEST_HPP
