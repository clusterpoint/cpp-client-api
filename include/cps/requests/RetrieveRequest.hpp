#ifndef CPS_RETRIEVEREQUEST_HPP
#define CPS_RETRIEVEREQUEST_HPP

#include <string>
#include <vector>
#include <map>

#include "../Request.hpp"
#include "../Utils.hpp"

namespace CPS
{
class RetrieveRequest: public Request
{
public:
    /**
     * @param id ID of document to retrieve
     */
    RetrieveRequest(const string &id) :
        Request("retrieve") {
        setIds(vector < string > (1, id));
    }
    /**
     * @param ids array of IDs of document to retrieve
     */
    RetrieveRequest(const vector<string> &ids) :
        Request("retrieve") {
        setIds(ids);
    }
    virtual ~RetrieveRequest() {
    }

    /**
     * @param id ID of document to retrieve
     */
    void setId(const string &id) {
        documentsWithUserId[id] = "";
    }
    /**
     * @param ids array of IDs of document to retrieve
     */
    void setIds(const vector<string> &ids) {
        for (unsigned int i = 0; i < ids.size(); i++) {
            documentsWithUserId[ids[i]] = "";
        }
    }
};
}

#endif //#ifndef CPS_RETRIEVEREQUEST_HPP
