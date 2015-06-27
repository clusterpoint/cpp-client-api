#ifndef CPS_LOOKUPRESPONSE_HPP
#define CPS_LOOKUPRESPONSE_HPP

#include <string>
#include <vector>
#include <map>

#include "../Response.hpp"
#include "../Utils.hpp"

namespace CPS
{
class LookupResponse;
typedef LookupResponse RetrieveResponse;
typedef LookupResponse ListLastResponse;
typedef LookupResponse ListFirstResponse;
typedef LookupResponse RetrieveLastResponse;
typedef LookupResponse RetrieveFirstResponse;
typedef LookupResponse ShowHistoryResponse;

class LookupResponse: public Response
{
public:
    /**
     * Constructs Response object.
     * Response object cannot exist without response text to work on,
     * so no default constructor is provided
     * @param rawResponse string of raw response from CPS server. This should be valid XML
     */
    LookupResponse(string rawResponse) :
        Response(rawResponse) {
    }
    virtual ~LookupResponse() {
        _documentsString.clear();
        for (unsigned int i = 0; i < _documentsXML.size(); i++) delete _documentsXML[i];
        _documentsXML.clear();
    }

    /**
     * Returns the number of documents found by lookup command
     */
    int getFound() {
        return getParam<int>("found", 0);
    }

    /**
     * Returns the documents from the response as vector of documents represented as strings
     * @param documentTagName string that identifies root of document
     * @param formatted boolean, true if document should be formatted with spaces for better readability
     */
    vector<string> getDocumentsString(bool formatted = true) {
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
    vector<XMLDocument*> getDocumentsXML() {
        if (!_documentsXML.empty())
            return _documentsXML;
        NodeSet ns = doc->FindFast("cps:reply/cps:content/results/" + documentRootXpath, true);
        for (int i = 0; i < (int) ns.size(); i++) {
            _documentsXML.push_back(XMLDocument::parseFromMemory(ns[i]->toString(false)));
        }
        return _documentsXML;
    }

private:
    vector<string> _documentsString;
    vector<XMLDocument*> _documentsXML;
};
}

#endif /* CPS_LOOKUPRESPONSE_HPP */
