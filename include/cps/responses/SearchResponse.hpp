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
        Term(const std::string &name, int hits) {
            this->name = name;
            this->hits = hits;
        }
        virtual ~Term() {
        }

        /** Term name */
        std::string name;
        /** How many documents match this facet */
        int hits;
    };

    /** @param path Facet path */
    SearchFacet(const std::string &path = "") {
        this->path = path;
    }
    virtual ~SearchFacet() {
    }

    /** Facet path */
    std::string path;
    /** Vector of terms for this facet */
    std::vector<Term> terms;
};

class SearchAggregate
{
public:
    /** @param query The query of the aggregate */
    SearchAggregate(const std::string &query = "") {
        this->query = query;
    }
    virtual ~SearchAggregate() {
    }

    /** Aggregate query */
    std::string query;
    /** Vector of aggregate results */
    std::vector<std::map<std::string, std::string> > data;
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
    SearchResponse(std::string rawResponse) :
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
    std::map<std::string, SearchFacet> getFacets() {
        if (!_facets.empty())
            return _facets;
        NodeSet ns = doc->FindFast("cps:reply/cps:content/facet", true);
        for (unsigned int i = 0; i < ns.size(); i++) {
        	std::list<Node *> terms = ns[i]->getChildren("term");
            SearchFacet facet(ns[i]->getAttribute("path")->getValue());
            for (std::list<Node *>::iterator it = terms.begin(); it != terms.end();
                    it++) {
            	std::string name = (*it)->getContent();
                int hits = atoi((*it)->getAttribute("hits")->getContentPtr());
                facet.terms.push_back(SearchFacet::Term(name, hits));
            }
            _facets[facet.path] = facet;
        }
        return _facets;
    }

    /**
     * Returns the map of aggregates where key is query for aggregate and value as object of type SearchAggregate
     */
    std::map<std::string, SearchAggregate> getAggregates() {
        if (!_aggregates.empty())
            return _aggregates;
        NodeSet ns = doc->FindFast("cps:reply/cps:content/aggregate", true);
        for (unsigned int i = 0; i < ns.size(); i++) {
        	std::list<Node *> query = ns[i]->getChildren("query");
        	std::list<Node *> data = ns[i]->getChildren("data");
            if (query.empty() || data.empty())
                continue;
            SearchAggregate aggregate(query.front()->getValue());
            for (std::list<Node *>::iterator it = data.begin(); it != data.end();
                    it++) {
            	std::map<std::string, std::string> fields;
                Node *child = (*it)->getFirstChild();
                while (child) {
                    fields[child->getName()] = child->getValue();
                    child = child->getNextSibling();
                }
                aggregate.data.push_back(fields);
            }
            _aggregates[aggregate.query] = aggregate;
        }
        return _aggregates;
    }

    /**
     * Returns the documents from the response as vector of documents represented as strings
     * @param documentTagName string that identifies root of document
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
    std::map<std::string, SearchFacet> _facets;
    std::map<std::string, SearchAggregate> _aggregates;
};

}

#endif /* CPS_SEARCHRESPONSE_HPP */
