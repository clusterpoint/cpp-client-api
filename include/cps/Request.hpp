#ifndef CPS_REQUEST_HPP
#define CPS_REQUEST_HPP

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "Exception.hpp"
#include "Utils.hpp"
#include "Xmldocument.hpp"

using namespace std;
using namespace rapidxml;

namespace CPS
{

class Request
{
public:
	typedef std::map<std::string, std::string> MapStringStringType;
    /**
     * Constructs an instance of the Request class.
     * @param command Specifies the command field for the request
     * @param requestId The request ID. Can be useful for identifying a particular request in a log file when debugging
     */
    Request(string command, string requestId = "")
    {
        this->command = command;
        this->requestId = requestId;
        this->label = "";
        this->requestType = "auto";

        string _textParamNames[] = { "added_external_id", "added_id", "aggregate",
                "case_sensitive", "cr", "deleted_external_id", "deleted_id",
                "description", "docs", "exact-match", "facet", "facet_size",
                "fail_if_exists", "file", "finalize", "for", "force", "from",
                "full", "group", "group_size", "h", "id", "idif", "iterator_id",
                "len", "message", "offset", "path", "persistent", "position",
                "quota", "rate2_ordering", "rate_from", "rate_to", "relevance",
                "return_doc", "return_internal", "sequence_check", "stem-lang",
                "step_size", "text", "transaction_id", "type" };
        textParamNames = vector <string> (_textParamNames, _textParamNames + 44);
        sort(textParamNames.begin(), textParamNames.end());

        string _rawParamNames[] = { "list", "query", "ordering" };
        rawParamNames = vector < string > (_rawParamNames, _rawParamNames + 3);
        sort(rawParamNames.begin(), rawParamNames.end());
    }

    virtual ~Request()
    {
    }

    /**
     * Returns request command name
     */
    string getCommand() const
    {
        return this->command;
    }

    /**
     * Returns the contents of the request as an XML string
     * @param docRootXpath document root xpath
     * @param docIdXpath document ID xpath
     * @param envelopeParams an associative array of CPS envelope parameters
     * @param createXML boolean if XML should created.
     * This is slower but might give You feedback if XML is not valid before sending to server
     *
     * @return string Full request XML as string
     */
    string getRequestXml(const string &docRootXpath, const string &docIdXpath,
            const map<string, vector<string> > &envelopeParams,
            bool createXML = false, long long transactionId = -1) const {

        XMLDocument *doc;
        Node *root, *content;
        string xml_as_string;

        if (createXML == true) {
            doc = XMLDocument::create(new xml_document<>());
            root = doc->createRootNode("request", "www.clusterpoint.com", "cps");
        } else {
            xml_as_string = "<cps:request xmlns:cps=\"www.clusterpoint.com\">";
        }
        for (map<string, vector<string> >::const_iterator it = envelopeParams.begin(); it != envelopeParams.end(); ++it) {
            for (unsigned int i = 0; i < it->second.size(); i++) {
                if (createXML == true) {
                    root->addChild(it->first, "cps")->addChildText(getValidXmlValue(it->second[i]));
                } else {
                    xml_as_string += "<cps:" + it->first + ">";
                    xml_as_string += getValidXmlValue(it->second[i]);
                    xml_as_string += "</cps:" + it->first + ">";
                }
            }
        }
        // Add text fields
        if (createXML == true) {
            content = root->addChild("content", "cps");
        } else {
            xml_as_string += "<cps:content>";
        }
        // Add transaction id if needed
        if (transactionId != -1) {
            if (createXML == true) {
                content->addChild("transaction_id")->addChildText(boost::lexical_cast<string>(transactionId));
            } else {
                xml_as_string += "<transaction_id>";
                xml_as_string += boost::lexical_cast<string>(transactionId);
                xml_as_string += "</transaction_id>";
            }
        }

        for (map<string, vector<string> >::const_iterator it = textParams.begin(); it != textParams.end(); ++it) {
            for (unsigned int i = 0; i < it->second.size(); i++) {
                if (createXML == true) {
                    content->addChild(it->first)->addChildText(Utils::toString(it->second[i]));
                } else {
                    xml_as_string += "<" + it->first + ">";
                    xml_as_string += Utils::toString(it->second[i]);
                    xml_as_string += "</" + it->first + ">";
                }
            }
        }
        // Add special fields: query, list, ordering
        for (map<string, vector<string> >::const_iterator it = rawParams.begin(); it != rawParams.end(); ++it) {
            for (unsigned int i = 0; i < it->second.size(); i++) {
                if (createXML == true) {
                    XMLDocument* fragment = XMLDocument::parseFromMemory("<" + it->first + ">" + it->second[i] + "</" + it->first + ">");
                    content->importNode(fragment->getRootNode());
                    delete fragment;
                } else {
                    xml_as_string += "<" + it->first + ">" + it->second[i] + "</" + it->first + ">";
                }
            }
        }

        // documents, document Ids
        for (map<string, string>::const_iterator it = documentsWithUserId.begin(); it != documentsWithUserId.end(); ++it) {
            // ID tag_name is what is left after removing docRootXpath prefix
            string id_tag_name = docIdXpath.substr(docRootXpath.size());
            string document = xmlUtilCreatePath(id_tag_name.c_str(), it->first.c_str()) + it->second;
            if (createXML == true) {
                XMLDocument* fragment = XMLDocument::parseFromMemory(xmlUtilCreatePath(docRootXpath.c_str(), document.c_str()));
                content->importNode(fragment->getRootNode());
                delete fragment;
            } else {
                xml_as_string += xmlUtilCreatePath(docRootXpath.c_str(), document.c_str());
            }
        }
        for (vector<string>::const_iterator it = documentsWithAutoId.begin(); it != documentsWithAutoId.end(); ++it) {
            string document;
            if (it->find("<" + docRootXpath + " ") != string::npos || it->find("<" + docRootXpath + ">") != string::npos) {
                document = *it;
            } else {
                document = xmlUtilCreatePath(docRootXpath.c_str(), it->c_str());
            }
            if (createXML == true) {
                XMLDocument* fragment = XMLDocument::parseFromMemory(document);
                content->importNode(fragment->getRootNode());
                delete fragment;
            } else {
                xml_as_string += document;
            }
        }

        if (createXML == true) {
            xml_as_string = doc->toString(true);
            delete doc;
        } else {
            xml_as_string += "</cps:content></cps:request>";
        }

        return xml_as_string;
    }

    /**
     * Returns request id
     */
    string getRequestId() const
    {
        return requestId;
    }
    /**
     * Sets request id.
     * @param requestId string of request id
     */
    void setRequestId(const string &requestId)
    {
        this->requestId = requestId;
    }
    /**
     * Returns request type
     */
    string getRequestType() const
    {
        return requestType;
    }
    /**
     * Sets request type.
     * Note that in a cluster setting, specifying "single" will lead to the request being processed only on the
     * single node that you are sending the request to, it and can lead to data inconsistencies
     * @param requestType string request type
     * One of following: auto(default) / single / cluster
     */
    void setRequestType(const string &requestType = "auto")
    {
        this->requestType = requestType;
    }
    /**
     * Returns label for cluster nodeset
     */
    string getClusterLabel() const
    {
        return label;
    }
    /**
     * Sets label for cluster nodeset
     * @param clusterLabel string label for cluster nodeset
     */
    void setClusterLabel(const string &clusterLabel)
    {
        this->label = clusterLabel;
    }

    /**
     * Returns the string with control characters stripped
     * @param src original string
     * @return string
     */
    static string getValidXmlValue(string src)
    {
        for (unsigned int i = 0; i < src.size(); i++) {
            if ((src[i] <= 0x1f)
                    && (src[i] != 0x09 || src[i] != 0x0a || src[i] != 0x0d)) {
                src[i] = ' ';
            }
        }
        return src;
    }

    /**
     * Sets request parameter
     *
     * Each key can have multiple values
     * These values will be added each in different XML element
     *
     * @param name string as parameter name
     * @param values vector of values
     * @param replace should existing value be replaced
     */
    void setParam(const string &name, const vector<string> &values, bool replace = false)
    {
        if (binary_search(textParamNames.begin(), textParamNames.end(), name)) {
        	if (replace) textParams[name] = values;
        	else textParams[name].insert(textParams[name].end(), values.begin(), values.end());
        } else if (binary_search(rawParamNames.begin(), rawParamNames.end(), name)) {
        	if (replace) rawParams[name] = values;
			else rawParams[name].insert(rawParams[name].end(), values.begin(), values.end());
        } else {
            BOOST_THROW_EXCEPTION(Exception("Invalid param name", 9002));
        }
    }

    /**
     * Sets request parameter
     *
     * @param name string as parameter name
     * @param value string as parameter value
     * @param replace should existing value be replaced
     */
    void setParam(const string &name, const string &value, bool replace = false)
    {
        setParam(name, vector < string > (1, value), replace);
    }

    /**
     * Set document to send as string
     * @param document document XML as string
     */
    void setDocument(const string &document) {
        documentsWithAutoId.push_back(document);
    }
    /**
     * Set document to send as string
     * @param id id of document
     * @param document document XML as string
     */
    void setDocument(const string &id, const string &document) {
        documentsWithUserId[id] = document;
    }
    /**
     * Set documents to send as string
     * @param documents array of documents XML as string
     */
    void setDocuments(const vector<string> &documents) {
        documentsWithAutoId.insert(this->documentsWithAutoId.begin(),
                                   documents.begin(), documents.end());
    }
    /**
     * Set documents to send as string
     * @param documents map with key as document id and value as document XML as string
     */
    void setDocuments(const map<string, string> &documents) {
        documentsWithUserId.insert(documents.begin(), documents.end());
    }

    /**
     * Creates a simple document
     *
     * @param doc Map of parameters where key is xpath of parameter and value is value of parameter
     * @param escape Should values be escaped
     */
    static string createSimpleDocument(const map<string, string> &doc,
            bool escape = true)
    {
        string result = "";
        for (map<string, string>::const_iterator it = doc.begin(); it != doc.end(); ++it) {
            result += Term(it->second, it->first, escape);
        }
        return result;
    }

    /**
     * Escapes <, > and & characters in the given term for inclusion into XML (like the search query).
     * Also wraps the term in XML tags if xpath is specified.
     * Note that this function doesn't escape the @, $, " and other symbols that are meaningful in a search query.
     * If You want to escape input that comes directly from the user and that isn't supposed to contain any search operators at all,
     * it's probably better to use {@link Request::QueryTerm}
     * @see Request::QueryTerm
     * @param term string the term to be escaped (e.g. a search query term)
     * @param xpath an optional xpath, to be specified if the search term is to be searched under a specific xpath
     * @param escape an optional parameter - whether to escape the term's XML
     */
    static string Term(string term, string xpath = "", bool escape = true)
    {
        return xmlUtilCreatePath(xpath.c_str(),
                ((escape) ? Utils::xmlspecialchars(term) : term).c_str());
    }

    /**
     * Escapes <, > and & characters, as well as @"{}()=$~+ (search query operators) in the given term for inclusion into the search query.
     * Also wraps the term in XML tags if xpath is specified.
     * @see Request::Term
     * @param term string the term to be escaped (e.g. a search query term)
     * @param xpath an optional xpath, to be specified if the search term is to be searched under a specific xpath
     * @param allowedSymbols a string containing operator symbols that the user is allowed to use (e.g. ")
     */
    static string QueryTerm(const string &term, const string &xpath = "",
            string allowedSymbols = "")
    {
        string invalidSymbols = "@$\"=<>(){}!+";
        string newTerm = "";
        for (unsigned int i = 0; i < term.size(); i++) {
            if (invalidSymbols.find(term[i]) != string::npos &&  allowedSymbols.find(term[i]) == string::npos) {
                newTerm += "\\";
            }
            newTerm += term[i];
        }
        return Term(newTerm, xpath);
    }

protected:
    /** Command name. For example: search, lookup, list-last etc. */
    string command;
    /** message id (any format) - for logging purposes */
    string requestId;
    /** Label for cluster nodeset */
    string label;
    /** Request type: auto(default) / single / cluster - type of request processing. */
    string requestType;
    /** List of text params */
    map<string, vector<string> > textParams;
    /** List of raw params */
    map<string, vector<string> > rawParams;
    /** Documents with user set id */
    map<string, string> documentsWithUserId;
    /** Documents without id (auto increment id) or id already included in document */
    vector<string> documentsWithAutoId;

private:
    /** Params that are of text type */
    vector<string> textParamNames;
    /** Params that are of raw type */
    vector<string> rawParamNames;

};

class Ordering
{
public:
    Ordering()
    {
    }
    virtual ~Ordering()
    {
    }

    /**
     * Should results be sorted in ascending order -> boolean value of false
     * Provided for convenience and readability
     */
    const static bool Ascending = true;
    /**
     * Should results be sorted in ascending order -> boolean value of true
     * Provided for convenience and readability
     */
    const static bool Descending = false;

    /**
     * Returns an ordering string for sorting by relevance
     * @see CPS::SearchRequest::setOrdering()
     * @param ascending parameter to specify ascending/descending order
     */
    static string RelevanceOrdering(bool ascending = true)
    {
        return "<relevance>" + Order(ascending) + "</relevance>";
    }

    /**
     * Returns an ordering string for sorting by a numeric field
     * @see CPS::SearchRequest::setOrdering()
     * @param tag the xpath of the tag by which You wish to perform sorting
     * @param ascending parameter to specify ascending/descending order
     */
    static string NumericOrdering(const string &tag, bool ascending = true)
    {
        return "<numeric>" + Request::Term(Order(ascending), tag) + "</numeric>";
    }

    /**
     * Returns an ordering string for sorting by a date field
     * @see CPS::SearchRequest::setOrdering()
     * @param tag the xpath of the tag by which You wish to perform sorting
     * @param ascending parameter to specify ascending/descending order
     */
    static string DateOrdering(const string &tag, bool ascending = true)
    {
        return "<date>" + Request::Term(Order(ascending), tag) + "</date>";
    }

    /**
     * Returns an ordering string for sorting by a string field
     * @see CPS::SearchRequest::setOrdering()
     * @param tag the xpath of the tag by which You wish to perform sorting
     * @param lang specifies the language (collation) to be used for ordering. E.g. "en"
     * @param ascending parameter to specify ascending/descending order
     */
    static string StringOrdering(const string &tag, const string &lang,
            bool ascending = true)
    {
        return "<string>" + Request::Term(Order(ascending) + "," + lang, tag)
                + "</string>";
    }

    /**
     * Return asending or descending string
     */
    static string Order(bool ascending = true)
    {
        return (ascending) ? "ascending" : "descending";
    }

    /**
     * Returns an ordering string for sorting by distance from a latitude/longitude coordinate pair
     * @see CPS::SearchRequest::setOrdering()
     * @param coords array of maps where key is xpath and value is centerpoint coordinates as values. Should contain exactly two elements - latitude first and longitude second
     * @param ascending parameter to specify ascending/descending order
     */
    static string LatLonDistanceOrdering(
            const vector<map<string, string> > &coords, bool ascending)
    {
        string ordering = "<distance type=\"latlong\" order=\""
                + Order(ascending) + "\"";
        for (unsigned int i = 0; i < coords.size(); i++) {
            for (map<string, string>::const_iterator it = coords[i].begin();
                    it != coords[i].end(); ++it) {
                ordering += Request::Term(it->second, it->first);
            }
        }
        return ordering;
    }

    /**
     * Returns an ordering string for sorting by distance from a latitude/longitude coordinate pair
     * @see CPS::SearchRequest::setOrdering()
     * @param coords array of maps where key is xpath and value is centerpoint coordinates as values.
     * @param ascending parameter to specify ascending/descending order
     */
    static string PlaneDistanceOrdering(
            const vector<map<string, string> > &coords, bool ascending)
    {
        string ordering = "<distance type=\"plane\" order=\"" + Order(ascending)
                + "\"";
        for (unsigned int i = 0; i < coords.size(); i++) {
            for (map<string, string>::const_iterator it = coords[i].begin();
                    it != coords[i].end(); ++it) {
                ordering += Request::Term(it->second, it->first);
            }
        }
        return ordering;
    }
};
}

#endif //#ifndef CPS_REQUEST_HPP 
