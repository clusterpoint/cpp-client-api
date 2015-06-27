#ifndef CPS_SEARCHDELETERESPONSE_HPP
#define CPS_SEARCHDELETERESPONSE_HPP

#include <string>
#include <vector>
#include <map>

#include "../Response.hpp"
#include "../Utils.hpp"

namespace CPS
{
class SearchDeleteResponse: public Response
{
public:
    /**
     * Constructs Response object.
     * Response object cannot exist without response text to work on,
     * so no default constructor is provided
     * @param rawResponse string of raw response from CPS server. This should be valid XML
     */
    SearchDeleteResponse(string rawResponse) :
        Response(rawResponse) {
    }
    virtual ~SearchDeleteResponse() {
    }

    /**
     * Returns the total number of hits - i.e. the number of documents erased
     */
    int getHits() {
        return getParam<int>("hits", 0);
    }
};
}

#endif /* CPS_SEARCHDELETERESPONSE_HPP */
