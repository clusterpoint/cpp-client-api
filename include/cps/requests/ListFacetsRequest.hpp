#ifndef CPS_LISTFACETSREQUEST_HPP
#define CPS_LISTFACETSREQUEST_HPP

#include <string>
#include <vector>
#include <map>

#include "../Request.hpp"
#include "../Utils.hpp"

namespace CPS
{
class ListFacetsRequest: public Request
{
public:
    /**
     * Constructs an instance of the CPSListFacetsRequest class.
     * @param path facet path to list the facet terms from
     */
    ListFacetsRequest(const string &path) :
        Request("list-facets") {
        setPath(path);
    }
    /**
     * Constructs an instance of the CPSListFacetsRequest class.
     * @param paths an array of paths to list the facet terms from
     */
    ListFacetsRequest(const vector<string> &paths) :
        Request("list-facets") {
        setPaths(paths);
    }
    virtual ~ListFacetsRequest() {
    }

    /**
     * Set array of paths for facets for which values will be returned
     * @param path facet path to list the facet terms from
     */
    void setPath(const string &path) {
        setPaths(vector < string > (1, path));
    }

    /**
     * Set array of paths for facets for which values will be returned
     * @param paths facet path to list the facet terms from
     */
    void setPaths(const vector<string> &paths) {
        setParam("path", paths);
    }
};
}

#endif //#ifndef CPS_LISTFACETSREQUEST_HPP
