#ifndef CPS_LISTLASTRETRIEVEFIRSTREQUEST_HPP
#define CPS_LISTLASTRETRIEVEFIRSTREQUEST_HPP

#include <string>
#include <vector>
#include <map>

#include "../Request.hpp"
#include "../Utils.hpp"

namespace CPS
{
class ListLastRetrieveFirstRequest: public Request
{
public:
    /**
     * Constructs an instance of the ListLastRetrieveFirstRequest class.
     * @param command command name
     * @param offset number of results to skip
     * @param docs maximum number of documents to return
     * @param list map where key is xpath and value contains listing options (yes, no, snippet or highlight)
     */
    ListLastRetrieveFirstRequest(const std::string &command, int offset = 0, int docs = 10,
                                 const std::map<std::string, std::string> &list = Request::MapStringStringType()) :
        Request(command) {
        if (offset != 0)
            setOffset(offset);
        if (docs != 10)
            setDocs(docs);
        if (!list.empty())
            setList(list);
    }
    virtual ~ListLastRetrieveFirstRequest() {
    }

    /**
     * Sets the number of documents to skip in the results
     * @param offset number of results to skip
     */
    void setOffset(int offset) {
        this->setParam("offset", Utils::toString(offset));
    }

    /**
     * Sets the maximum number of documents to be returned
     * @param docs maximum number of documents
     */
    void setDocs(int docs) {
        this->setParam("docs", Utils::toString(docs));
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
};

class ListLastRequest: public ListLastRetrieveFirstRequest
{
public:
    /**
     * Constructs an instance of the ListLastRequest class.
     * @param offset number of results to skip
     * @param docs maximum number of documents to return
     * @param list map where key is xpath and value contains listing options (yes, no, snippet or highlight)
     */
    ListLastRequest(int offset = 0, int docs = 10, const std::map<std::string, std::string> &list = Request::MapStringStringType()) :
        ListLastRetrieveFirstRequest("list-last", offset, docs, list) {
    }
    virtual ~ListLastRequest() {
    }
};

class ListFirstRequest: public ListLastRetrieveFirstRequest
{
public:
    /**
     * Constructs an instance of the ListFirstRequest class.
     * @param offset number of results to skip
     * @param docs maximum number of documents to return
     * @param list map where key is xpath and value contains listing options (yes, no, snippet or highlight)
     */
    ListFirstRequest(int offset = 0, int docs = 10, const std::map<std::string, std::string> &list = Request::MapStringStringType()) :
        ListLastRetrieveFirstRequest("list-first", offset, docs, list) {
    }
    virtual ~ListFirstRequest() {
    }
};

class RetrieveLastRequest: public ListLastRetrieveFirstRequest
{
public:
    /**
     * Constructs an instance of the RetrieveLastRequest class.
     * @param offset number of results to skip
     * @param docs maximum number of documents to return
     */
    RetrieveLastRequest(int offset = 0, int docs = 10) :
        ListLastRetrieveFirstRequest("retrieve-last", offset, docs) {
    }
    virtual ~RetrieveLastRequest() {
    }
};

class RetrieveFirstRequest: public ListLastRetrieveFirstRequest
{
public:
    /**
     * Constructs an instance of the RetrieveFirstRequest class.
     * @param offset number of results to skip
     * @param docs maximum number of documents to return
     */
    RetrieveFirstRequest(int offset = 0, int docs = 10) :
        ListLastRetrieveFirstRequest("retrieve-first", offset, docs) {
    }
    virtual ~RetrieveFirstRequest() {
    }
};
}

#endif //#ifndef CPS_LISTLASTRETRIEVEFIRSTREQUEST_HPP
