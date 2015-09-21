#ifndef CPS_LISTPATHSRESPONSE_HPP
#define CPS_LISTPATHSRESPONSE_HPP

#include <string>
#include <vector>
#include <map>

#include "../Response.hpp"
#include "../Utils.hpp"

namespace CPS
{
class ListPathsResponse: public Response
{
public:
    /**
     * Constructs Response object.
     * Response object cannot exist without response text to work on,
     * so no default constructor is provided
     * @param rawResponse string of raw response from CPS server. This should be valid XML
     */
    ListPathsResponse(std::string rawResponse) :
        Response(rawResponse) {
    }
    virtual ~ListPathsResponse() {
    }

    /**
     * Returns paths that exist in this storage
     */
    std::vector <std::string>& getPaths() {
        if (!_paths.empty()) return _paths;
        NodeSet ns = doc->FindFast("cps:reply/cps:content/paths/path", true);
        for (unsigned int i = 0; i < ns.size(); i++) {
            _paths.push_back(ns[i]->getContent());
        }
        return _paths;
    }
private:
    std::vector <std::string> _paths;
};
}

#endif /* CPS_LISTPATHSRESPONSE_HPP */
