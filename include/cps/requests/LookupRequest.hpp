#ifndef CPS_LOOKUPREQUEST_HPP
#define CPS_LOOKUPREQUEST_HPP

#include <string>
#include <vector>
#include <map>

#include "../Request.hpp"
#include "../Utils.hpp"

namespace CPS
{
class LookupRequest: public Request
{
public:
    /**
     * @param id document id to be looked up
     * @param list map where key is xpath and value contains listing options (yes, no, snippet or highlight)
     */
    LookupRequest(const string &id,
                  const map<string, string> &list = Request::MapStringStringType()) :
        Request("lookup") {
        setId(id);
        if (!list.empty())
            setList(list);
    }
    /**
     * @param ids array of document ids to be looked up
     * @param list map where key is xpath and value contains listing options (yes, no, snippet or highlight)
     */
    LookupRequest(const vector<string> &ids, const map<string, string> &list =
    			Request::MapStringStringType()) :
        Request("lookup") {
        setIds(ids);
        if (!list.empty())
            setList(list);
    }
    virtual ~LookupRequest() {
    }

    /**
     * Defines which tags of the search results should be listed in the response
     * @param list map where key is xpath and value contains listing options (yes, no, snippet or highlight)
     */
    void setList(const map<string, string> &list) {
        string listString = "";
        for (map<string, string>::const_iterator it = list.begin();
                it != list.end(); ++it) {
            listString += Request::Term(it->second, it->first);
        }
        this->setParam("list", listString);
    }

    /**
     * Set document ids to be looked up
     * @param id document id to be looked up
     */
    void setId(const string &id) {
        documentsWithUserId[id] = "";
    }

    /**
     * Set document ids to be looked up
     * @param ids array of document ids to be looked up
     */
    void setIds(const vector<string> &ids) {
        for (unsigned int i = 0; i < ids.size(); i++) {
            documentsWithUserId[ids[i]] = "";
        }
    }
};
}

#endif //#ifndef CPS_LOOKUPREQUEST_HPP
