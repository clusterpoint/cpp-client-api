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
    ListFacetsResponse(std::string rawResponse) :
        Response(rawResponse) {
    }
    virtual ~ListFacetsResponse() {}

    /**
     * Returns the map of facets where key is path for facet and value as vector of terms for this facet
     */
    std::map<std::string, std::vector<std::string> > getFacets() {
        if (!_facets.empty())
            return _facets;
        NodeSet ns = doc->FindFast("facet", true);
        for (unsigned int i = 0; i < ns.size(); i++) {
        	std::list<Node *> terms = ns[i]->getChildren("term");
        	std::string path = ns[i]->getAttribute("path")->getValue();
            for (std::list<Node *>::iterator it = terms.begin(); it != terms.end(); it++) {
                _facets[path].push_back((*it)->getContent());
            }
        }
        return _facets;
    }

private:
    std::map<std::string, std::vector<std::string> > _facets;
};
}

#endif /* CPS_LISTFACETSRESPONSE_HPP */
