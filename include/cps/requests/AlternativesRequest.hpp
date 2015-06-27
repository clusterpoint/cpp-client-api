#ifndef CPS_ALTERNATIVESREQUEST_HPP
#define CPS_ALTERNATIVESREQUEST_HPP

#include <string>
#include <vector>
#include <map>

#include "../Request.hpp"
#include "../Utils.hpp"

namespace CPS
{
class AlternativesRequest: public Request
{
public:
    /**
     * @param query see {@link setQuery(const string&) setQuery}
     * @param cr see {@link setCr(double) setCr}
     * @param idif see {@link setIdif(double) setIdif}
     * @param h see {@link setH(double) setH}
     */
    AlternativesRequest(const string &query, double cr = 2.0, double idif = 3.0, double h = 2.5) :
        Request("alternatives") {
        setQuery(query);
        if (cr != 2.0) setCr(cr);
        if (idif != 3.0) setIdif(idif);
        if (h != 2.5) setH(h);
    }
    virtual ~AlternativesRequest() {
    }

    /**
     * Sets the search query.
     *
     * Example usage:
     * <code>r.setQuery(CPS_Term("predefined_term", "/generated_fields/type/"));</code>
     * @param query The query string.
     * All <, > and & characters that aren't supposed to be XML tags, should be escaped;
     * @see CPS_Term
     */
    void setQuery(const string &query) {
        this->setParam("query", query);
    }

    /**
     * Minimum ratio between the occurrence of the alternative and the occurrence of the search term. If this parameter is increased, less results are returned while performance is improved.
     *
     * @param cr cr value
     */
    void setCr(double cr = 2.0) {
        this->setParam("cr", Utils::toString(cr), true);
    }

    /**
     * A number that limits how much the alternative may differ from the search term, the greater the idif value, the greater the allowed difference. If this parameter is increased, more results are returned while performance is decreased.
     *
     * @param idif idif value
     */
    void setIdif(double idif = 3.0) {
        this->setParam("idif", Utils::toString(idif), true);
    }

    /**
     * A number that limits the overall estimate of the quality of the alternative, the greater the cr value and the smaller the idif value, the greater the h value. If this parameter is increased, less results are returned while performance is improved.
     *
     * @param h h value
     */
    void setH(double h = 2.5) {
        this->setParam("h", Utils::toString(h), true);
    }
};
}

#endif //#ifndef CPS_ALTERNATIVESREQUEST_HPP
