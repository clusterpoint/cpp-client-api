#ifndef CPS_MODIFYRESPONSE_HPP
#define CPS_MODIFYRESPONSE_HPP

#include <string>
#include <vector>
#include <map>

#include "../Response.hpp"
#include "../Utils.hpp"

namespace CPS
{
class ModifyResponse;
typedef ModifyResponse UpdateResponse;
typedef ModifyResponse DeleteResponse;
typedef ModifyResponse ReplaceResponse;
typedef ModifyResponse PartialReplaceResponse;
typedef ModifyResponse InsertResponse;

class ModifyResponse: public Response
{
public:
    /**
     * Constructs Response object.
     * Response object cannot exist without response text to work on,
     * so no default constructor is provided
     * @param rawResponse string of raw response from CPS server. This should be valid XML
     */
    ModifyResponse(string rawResponse) :
        Response(rawResponse) {
    }
    virtual ~ModifyResponse() {
    }

    /**
     * Returns an array of IDs of documents that have been successfully modified
     */
    vector<string>& getModifiedIds() {
        if (!_modifiedIds.empty()) return _modifiedIds;
        NodeSet ns = doc->FindFast("cps:reply/cps:content/" + documentRootXpath + "/id", true);
        for (unsigned int i = 0; i < ns.size(); i++) {
            _modifiedIds.push_back(ns[i]->getContent());
        }
        return _modifiedIds;
    }
private:
    vector<string> _modifiedIds;
};
}

#endif /* CPS_MODIFYRESPONSE_HPP */
