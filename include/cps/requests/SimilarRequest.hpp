#ifndef CPS_SIMILARREQUEST_HPP
#define CPS_SIMILARREQUEST_HPP

#include <string>
#include <vector>
#include <map>

#include "../Request.hpp"
#include "../Utils.hpp"

namespace CPS
{
class SimilarRequest: public Request
{
public:
    /**
     * @param len number of keywords to extract from the source
     * @param quota minimum number of keywords matching in the destination
     * @param offset number of results to skip before returning the following ones
     * @param docs number of documents to retrieve
     * @param query an optional query that all found documents have to match against
     */
    SimilarRequest(int len, int quota, int offset = 0, int docs = 10,
                   const std::string &query = "") :
        Request("similar") {
        setParam("len", Utils::toString(len));
        setParam("quota", Utils::toString(quota));
        if (offset != 0)
            setParam("offset", Utils::toString(offset));
        if (docs != 10)
            setParam("docs", Utils::toString(docs));
        if (!query.empty())
            setQuery(query);
    }
    virtual ~SimilarRequest() {
    }

    /**
     * Defines query that all found documents have to match against
     */
    void setQuery(const std::string &query) {
        this->setParam("query", query);
    }

    /**
     * Defines query that all found documents have to match against
     */
    void setId(const std::string &id) {
        this->setParam("id", id);
    }

    /**
     * Defines query that all found documents have to match against
     */
    void setText(const std::string &text) {
        this->setParam("text", text);
    }
};

class SimilarDocumentRequest: public SimilarRequest
{
public:
    /**
     * @param id ID of the source document - the one that You want to search similar documents to
     * @param len number of keywords to extract from the source
     * @param quota minimum number of keywords matching in the destination
     * @param offset number of results to skip before returning the following ones
     * @param docs number of documents to retrieve
     * @param query an optional query that all found documents have to match against
     */
    SimilarDocumentRequest(const std::string &id, int len, int quota, int offset = 0,
                           int docs = 10, const std::string &query = "") :
        SimilarRequest(len, quota, offset, docs, query) {
        setId(id);

    }
    virtual ~SimilarDocumentRequest() {
    }
};

class SimilarTextRequest: public SimilarRequest
{
public:
    /**
     * @param text A chunk of text that the found documents have to be similar to
     * @param len number of keywords to extract from the source
     * @param quota minimum number of keywords matching in the destination
     * @param offset number of results to skip before returning the following ones
     * @param docs number of documents to retrieve
     * @param query an optional query that all found documents have to match against
     */
    SimilarTextRequest(const std::string &text, int len, int quota, int offset = 0,
                       int docs = 10, const std::string &query = "") :
        SimilarRequest(len, quota, offset, docs, query) {
        setText(text);
    }
    virtual ~SimilarTextRequest() {
    }
};
}

#endif //#ifndef CPS_SIMILARREQUEST_HPP
