#ifndef CPS_SEARCHRESPONSE_HPP
#define CPS_SEARCHRESPONSE_HPP

#include <string>
#include <vector>
#include <map>

#include "../Response.hpp"
#include "../Utils.hpp"

namespace CPS
{
class SearchResponse;
typedef SearchResponse SimilarResponse;
typedef SearchResponse SimilarDocumentResponse;
typedef SearchResponse SimilarTextResponse;

class SearchFacet
{
public:
    class Term
    {
    public:
        /**
         * @param name term name
         * @param hits how many documents match this facet
         */
        Term(const string &name, int hits) {
            this->name = name;
            this->hits = hits;
        }
        virtual ~Term() {
        }

        /** Term name */
        string name;
        /** How many documents match this facet */
        int hits;
    };

    /** @param path Facet path */
    SearchFacet(const string &path = "") {
        this->path = path;
    }
    virtual ~SearchFacet() {
    }

    /** Facet path */
    string path;
    /** Vector of terms for this facet */
    vector<Term> terms;
};

class SearchResponse: public Response
{
public:
    /**
     * Constructs Response object.
     * Response object cannot exist without response text to work on,
     * so no default constructor is provided
     * @param rawResponse string of raw response from CPS server. This should be valid XML
     */
    SearchResponse(string rawResponse) :
        Response(rawResponse) {
    }
    virtual ~SearchResponse() {
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
     * Returns the total number of hits - i.e. total amount of results that match the search query
     */
    int getHits() {
        return getParam<int>("hits", 0);
    }

    /**
     * Returns the map of facets where key is path for facet and value as object of type SearchFacet
     */
    map<string, SearchFacet> getFacets() {
        if (!_facets.empty())
            return _facets;
        NodeSet ns = doc->FindFast("cps:reply/cps:content/facet", true);
        for (unsigned int i = 0; i < ns.size(); i++) {
            list<Node *> terms = ns[i]->getChildren("term");
            SearchFacet facet(ns[i]->getAttribute("path")->getValue());
            for (list<Node *>::iterator it = terms.begin(); it != terms.end();
                    it++) {
                string name = (*it)->getContent();
                int hits = atoi((*it)->getAttribute("hits")->getContentPtr());
                facet.terms.push_back(SearchFacet::Term(name, hits));
            }
            _facets[facet.path] = facet;
        }
        return _facets;
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
    map<string, SearchFacet> _facets;
};
}

#endif /* CPS_SEARCHRESPONSE_HPP */
