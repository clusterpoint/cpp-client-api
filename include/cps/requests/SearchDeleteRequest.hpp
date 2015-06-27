#ifndef CPS_SEARCHDELETEREQUEST_HPP
#define CPS_SEARCHDELETEREQUEST_HPP

#include <string>
#include <vector>
#include <map>

#include "../Request.hpp"
#include "../Utils.hpp"

namespace CPS
{
class SearchDeleteRequest: public Request
{
public:
    /**
     * @param query The query string. see {@link #setQuery(const string &query) setQuery} for more info.
     */
    SearchDeleteRequest(const string &query) :
        Request("search-delete") {
        setQuery(query);
    }
    virtual ~SearchDeleteRequest() {
    }

    /**
     * Sets the search query.
     *
     * Example usage:
     * <code>r.setQuery(CPS::Request::Term("predefined_term", "/generated_fields/type/"));</code>
     * @param query The query string.
     * All <, > and & characters that aren't supposed to be XML tags, should be escaped;
     * @see CPS::Request::Term
     * @see CPS::Request::QueryTerm
     */
    void setQuery(const string &query) {
        this->setParam("query", query);
    }

    /**
     * Sets the stemming language
     * @param stemLang 2-letter language ID
     */
    void setStemLang(const string &stemLang) {
        this->setParam("stem-lang", stemLang);
    }

    /**
     * Sets the exact match option
     * @param exactMatch Exact match option : text, binary or all
     */
    void setExactMatch(const string &exactMatch) {
        this->setParam("exact-match", exactMatch);
    }
};
}

#endif //#ifndef CPS_SEARCHDELETEREQUEST_HPP
