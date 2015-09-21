#ifndef CPS_RESPONSE_HPP
#define CPS_RESPONSE_HPP

#include "Xmldocument.hpp"

#include <iostream>
#include <string>

#include <boost/lexical_cast.hpp>

namespace CPS
{

class Response
{
public:
    /**
     * Constructs Response object.
     * Response object cannot exist without response text to work on,
     * so no default constructor is provided
     * @param rawResponse string of raw response from CPS server. This should be valid XML
     */
    Response(std::string rawResponse, std::string documentRootXpath = "document", std::string documentIdXpath = "document/id"): failed(false) {
        this->documentRootXpath = documentRootXpath;
        this->documentIdXpath = documentIdXpath;
        try {
            doc = XMLDocument::parseFromMemory(rawResponse);
        } catch (std::exception &e) {
            BOOST_THROW_EXCEPTION(CPS::Exception("Invalid response", 9001));
        }
    }
    virtual ~Response() {
        delete doc;
    }

    /**
     * Returns the time that it took to process the request in the CPS engine
     */
    float getSeconds() {
        NodeSet ns = doc->FindFast("cps:reply/cps:seconds", false);
        if (ns.size() == 1) {
            return atof(ns[0]->getContent().c_str());
        }
        return 0.0;
    }

    /**
     * Returns executed command name
     */
    std::string getCommand() {
        NodeSet ns = doc->FindFast("cps:reply/cps:command", false);
        if (ns.size() == 1) {
            return ns[0]->getContent().c_str();
        }
        return "";
    }

    /**
     * Returns storage name for what request was executed
     */
    std::string getStorage() {
        NodeSet ns = doc->FindFast("cps:reply/cps:storage", false);
        if (ns.size() == 1) {
            return ns[0]->getContent().c_str();
        }
        return "";
    }

    /**
     * Returns vector of errors encountered
     */
    const std::vector<Error>& getErrors() {
        if (!errors.empty()) return errors;
        NodeSet ns = doc->FindFast("cps:reply/cps:error", true);
        for (unsigned int i = 0; i < ns.size(); i++) {
            Error err;
            Node *el = ns[i]->getFirstChild();
            while (el != NULL) {
            	std::string name = el->getName();
            	std::string value = el->getContent();
                if (name == "code")
                    err.code = value;
                else if (name == "text")
                    err.text = value;
                else if (name == "message")
                    err.message = value;
                else if (name == "level") {
                    err.level = value;
                    failed |= (value == "REJECTED");
                    failed |= (value == "FAILED");
                    failed |= (value == "ERROR");
                    failed |= (value == "FATAL");
                } else if (name == "source")
                    err.source = value;
                else if (name == "document_id")
                    err.documentIds.push_back(value);
                el = el->getNextSibling();
            }
            errors.push_back(err);
        }
        return errors;
    }

    /**
     * Returns parsed XML as string
     */
    std::string toString() {
        return doc->toString(true);
    }

    /**
     * Returns parameter value from reply's content
     */
    template<class T> T getParam(std::string key, T def = T()) {
        NodeSet ns = doc->FindFast("cps:reply/cps:content/" + key, false);
        if (ns.size() == 1) {
            return boost::lexical_cast<T>(ns[0]->getContent());
        }
        return def;
    }

    bool hasFailed() {
    	return failed;
    }

public:
    /** Parsed reply as XMLDocument */
    XMLDocument *doc;
    /** Vector of errors returned from CPS */
    std::vector<Error> errors;
    bool failed; // Did the response contain errors about failing

    std::string documentRootXpath;
    std::string documentIdXpath;
};
}

#endif //#ifndef CPS_RESPONSE_HPP 
