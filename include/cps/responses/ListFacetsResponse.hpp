#ifndef CPS_LISTFACETSRESPONSE_HPP
#define CPS_LISTFACETSRESPONSE_HPP

#include <string>
#include <vector>
#include <map>

#include "../Response.hpp"
#include "../Utils.hpp"

namespace CPS
{
class ListFacetsResponse: public Response
{
public:
    /**
     * Constructs Response object.
     * Response object cannot exist without response text to work on,
     * so no default constructor is provided
     * @param rawResponse string of raw response from CPS server. This should be valid XML
     */
    ListFacetsResponse(string rawResponse) :
        Response(rawResponse) {
    }
    virtual ~ListFacetsResponse() {}

    /**
     * Returns the map of facets where key is path for facet and value as vector of terms for this facet
     */
    map<string, vector<string> > getFacets() {
        if (!_facets.empty())
            return _facets;
        NodeSet ns = doc->FindFast("facet", true);
        for (unsigned int i = 0; i < ns.size(); i++) {
            list<Node *> terms = ns[i]->getChildren("term");
            string path = ns[i]->getAttribute("path")->getValue();
            for (list<Node *>::iterator it = terms.begin(); it != terms.end(); it++) {
                _facets[path].push_back((*it)->getContent());
            }
        }
        return _facets;
    }

private:
    map<string, vector<string> > _facets;
};
}

#endif /* CPS_LISTFACETSRESPONSE_HPP */
