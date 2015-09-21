#ifndef CPS_LISTWORDSREQUEST_HPP
#define CPS_LISTWORDSREQUEST_HPP

#include <string>
#include <vector>
#include <map>

#include "../Request.hpp"
#include "../Utils.hpp"

namespace CPS
{
class ListWordsRequest: public Request
{
public:
    /**
     * Constructs an instance of CPSListWordsRequest
     * @param query term with wildcards as a string to count occurances
     */
    ListWordsRequest(const std::string &query) :
        Request("list-words") {
        setQuery(query);
    }
    /**
     * Constructs an instance of CPSListWordsRequest
     * @param query vector of terms with wildcards as a string to count occurances
     */
    ListWordsRequest(const std::vector <std::string> &query) :
        Request("list-words") {
        setQuery(query);
    }
    virtual ~ListWordsRequest() {
    }

    /**
     * @param query term with wildcards as a string to count occurances
     */
    void setQuery(const std::string &query) {
        this->setParam("query", query);
    }

    /**
     * @param query vector of terms with wildcards as a string to count occurances
     */
    void setQuery(const std::vector <std::string> &query) {
    	std::string q = "";
        for (unsigned int i = 0; i < query.size(); i++) q += query[i] + " ";
        this->setParam("query", q);
    }
};
}

#endif //#ifndef CPS_LISTWORDSREQUEST_HPP
