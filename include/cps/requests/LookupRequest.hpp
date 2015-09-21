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
    LookupRequest(const std::string &id,
                  const std::map<std::string, std::string> &list = Request::MapStringStringType()) :
        Request("lookup") {
        setId(id);
        if (!list.empty())
            setList(list);
    }
    /**
     * @param ids array of document ids to be looked up
     * @param list map where key is xpath and value contains listing options (yes, no, snippet or highlight)
     */
    LookupRequest(const std::vector<std::string> &ids, const std::map<std::string, std::string> &list =
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
    void setList(const std::map<std::string, std::string> &list) {
    	std::string listString = "";
        for (std::map<std::string, std::string>::const_iterator it = list.begin();
                it != list.end(); ++it) {
            listString += Request::Term(it->second, it->first);
        }
        this->setParam("list", listString);
    }

    /**
     * Set document ids to be looked up
     * @param id document id to be looked up
     */
    void setId(const std::string &id) {
        documentsWithUserId[id] = "";
    }

    /**
     * Set document ids to be looked up
     * @param ids array of document ids to be looked up
     */
    void setIds(const std::vector<std::string> &ids) {
        for (unsigned int i = 0; i < ids.size(); i++) {
            documentsWithUserId[ids[i]] = "";
        }
    }
};
}

#endif //#ifndef CPS_LOOKUPREQUEST_HPP
