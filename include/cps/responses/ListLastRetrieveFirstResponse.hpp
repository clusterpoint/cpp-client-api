#ifndef CPS_LISTLASTRETRIEVEFIRSTRESPONSE_HPP
#define CPS_LISTLASTRETRIEVEFIRSTRESPONSE_HPP

#include <string>
#include <vector>
#include <map>

#include "../Response.hpp"
#include "../Utils.hpp"

namespace CPS
{
class ListLastRetrieveFirstResponse;
typedef ListLastRetrieveFirstResponse RetrieveResponse;
typedef ListLastRetrieveFirstResponse ListLastResponse;
typedef ListLastRetrieveFirstResponse ListFirstResponse;
typedef ListLastRetrieveFirstResponse RetrieveLastResponse;
typedef ListLastRetrieveFirstResponse RetrieveFirstResponse;
typedef ListLastRetrieveFirstResponse ShowHistoryResponse;

class ListLastRetrieveFirstResponse: public Response
{
public:
    /**
     * Constructs Response object.
     * Response object cannot exist without response text to work on,
     * so no default constructor is provided
     * @param rawResponse string of raw response from CPS server. This should be valid XML
     */
    ListLastRetrieveFirstResponse(std::string rawResponse) :
        Response(rawResponse) {
    }
    virtual ~ListLastRetrieveFirstResponse() {
        _documentsString.clear();
        for (unsigned int i = 0; i < _documentsXML.size(); i++) delete _documentsXML[i];
        _documentsXML.clear();
    }

    /**
     * Returns the position of the first document that was returned
     */
    int getFrom() {
        return getParam<int>("from", 0);
    }

    /**
     * Returns the position of the last document that was returned
     */
    int getTo() {
        return getParam<int>("to", 0);
    }

    /**
     * Returns the documents from the response as vector of documents represented as strings
     * @param formatted boolean, true if document should be formatted with spaces for better readability
     */
    std::vector<std::string> getDocumentsString(bool formatted = true) {
        if (!_documentsString.empty())
            return _documentsString;
        NodeSet ns = doc->FindFast("cps:reply/cps:content/results/" + documentRootXpath, true);
        for (unsigned int i = 0; i < ns.size(); i++) {
            _documentsString.push_back(ns[i]->toString(!formatted));
        }
        return _documentsString;
    }

    /**
     * Returns the documents from the response as vector of documents represented as XMLDocument
     * @param documentTagName string that identifies root of document
     * @see XMLDocument
     */
    std::vector<XMLDocument*> getDocumentsXML() {
        if (!_documentsXML.empty())
            return _documentsXML;
        NodeSet ns = doc->FindFast("cps:reply/cps:content/results/" + documentRootXpath, true);
        for (int i = 0; i < (int) ns.size(); i++) {
            _documentsXML.push_back(XMLDocument::parseFromMemory(ns[i]->toString(false)));
        }
        return _documentsXML;
    }

private:
    std::vector<std::string> _documentsString;
    std::vector<XMLDocument*> _documentsXML;
};
}

#endif /* CPS_LISTLASTRETRIEVEFIRSTRESPONSE_HPP */
