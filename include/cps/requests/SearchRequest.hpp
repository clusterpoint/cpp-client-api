#ifndef CPS_SEARCHREQUEST_HPP
#define CPS_SEARCHREQUEST_HPP

#include <string>
#include <vector>
#include <map>

#include "../Request.hpp"
#include "../Utils.hpp"

namespace CPS
{
class SearchRequest: public Request
{
public:
    /**
     * Constructs an instance of the SearchRequest class.
     * @param query The query string. see {@link #setQuery(const string &query) setQuery} for more info.
     * @param offset Defines the number of documents to skip before including them in the results
     * @param docs Maximum document count to retrieve
     * @param list map where key is xpath and value contains listing options (yes, no, snippet or highlight)
     */
    SearchRequest(string query, int offset = 0, int docs = 10,
                  const map<string, string> &list = Request::MapStringStringType()) :
        Request("search") {
        this->setQuery(query);
        if (offset != 0)
            this->setOffset(offset);
        if (docs != 10)
            this->setDocs(docs);
        if (list.size() != 0)
            this->setList(list);
    }

    virtual ~SearchRequest() {
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
     * Sets the maximum number of documents to be returned
     * @param docs maximum number of documents
     */
    void setDocs(int docs) {
        this->setParam("docs", Utils::toString(docs), true);
    }

    /**
     * Sets the number of documents to skip in the results
     * @param offset number of results to skip
     */
    void setOffset(int offset) {
        this->setParam("offset", Utils::toString(offset), true);
    }

    /**
     * Sets the paths for facets
     * @param facet path of facet
     */
    void setFacet(const string &facet) {
        this->setFacets(vector < string > (1, facet));
    }

    /**
     * Sets the paths for facets
     * @param facets array of paths of facets
     */
    void setFacets(const vector<string> &facets) {
        this->setParam("facet", facets);
    }

    /**
     * Sets how many facets should be returned
     * @param facetSize int amount of facets to show
     */
    void setFacetSize(int facetSize) {
        this->setParam("facet_size", Utils::toString(facetSize), true);
    }

    /**
     * Sets aggregate functions
     * @param aggregate The aggregate function
     */
    void setAggregate(const string &aggregate) {
        this->setAggregate(vector < string > (1, aggregate));
    }

    /**
     * Sets aggregate functions
     * @param aggregates The list of aggregate functions
     */
    void setAggregate(const vector<string> &aggregates) {
        this->setParam("aggregate", aggregates);
    }

    /**
     * Sets the stemming language
     * @param stemLang 2-letter language ID
     */
    void setStemLang(const string &stemLang) {
        this->setParam("stem-lang", stemLang, true);
    }

    /**
     * Sets the exact match option
     * @param exactMatch Exact match option : text, binary or all
     */
    void setExactMatch(const string &exactMatch) {
        this->setParam("exact-match", exactMatch, true);
    }

    /**
     * Sets grouping options
     * @param tagName name of the grouping tag
     * @param count maximum number of documents to return from each group
     */
    void setGroup(const string &tagName, int count) {
        this->setParam("group", tagName);
        this->setParam("group_size", Utils::toString(count), true);
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
     * Defines the order in which results should be returned.
     * @param order sorting string
     * @see CPS::Ordering
     */
    void setOrdering(const string &order) {
        this->setParam("ordering", order);
    }

    /**
     * Defines the order in which results should be returned.
     * @param order array of sorting strings
     * @see CPS::Ordering
     */
    void setOrdering(const vector<string> &order) {
        this->setParam("ordering", CPS::Utils::join(order, ""));
    }
};
}

#endif //#ifndef CPS_SEARCHREQUEST_HPP
