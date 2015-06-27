#ifndef CPS_ALTERNATIVESRESPONSE_HPP
#define CPS_ALTERNATIVESRESPONSE_HPP

#include <string>
#include <vector>
#include <map>

#include "../Response.hpp"
#include "../Utils.hpp"

namespace CPS
{

class Alternative
{
public:
    class Word
    {
    public:
        Word() {}
        virtual ~Word() {}

        /** number of times the alternative occurs in the Storage */
        int count;
        /** h value of the alternative */
        double h;
        /** idif value of the alternative */
        double idif;
        /** cr value of the alternative */
        double cr;
        /** Actual word (content) that is alternative */
        string content;
    };

    Alternative() {}
    virtual ~Alternative() {}

    /** alternative search term */
    string to;
    /** number of times the alternative search term occurs in the Storage */
    int count;
    /** list of alternative words */
    vector<Word> words;
};

class AlternativesResponse: public Response
{
public:
    /**
     * Constructs Response object.
     * Response object cannot exist without response text to work on,
     * so no default constructor is provided
     * @param rawResponse string of raw response from CPS server. This should be valid XML
     */
    AlternativesResponse(string rawResponse) :
        Response(rawResponse) {
    }
    virtual ~AlternativesResponse() {}

    /**
     * Gets the spelling alternatives to the specified query terms.
     * Returns a map with term as key and an Alternative object as value for each query term
     */
    map<string, Alternative> getAlternatives() {
        if (!_alternatives.empty())
            return _alternatives;
        _alternatives.clear();
        NodeSet alternatives = doc->FindFast("cps:reply/cps:content/alternatives_list/alternatives", true);
        for (unsigned int i = 0; i < alternatives.size(); i++) {
            Node *el = alternatives[i]->getFirstChild();
            Alternative alt;
            while (el != NULL) {
                string name = el->getName();
                if (name == "to")
                    alt.to = el->getContent();
                else if (name == "count")
                    alt.count = atoi(el->getContentPtr());
                else if (name == "word") {
                    Alternative::Word w;
                    w.count = atoi(el->getAttribute("count")->getContentPtr());
                    w.h = atof(el->getAttribute("h")->getContentPtr());
                    w.idif = atof(el->getAttribute("idif")->getContentPtr());
                    w.cr = atof(el->getAttribute("cr")->getContentPtr());
                    w.content = el->getContent();
                    alt.words.push_back(w);
                }
                el = el->getNextSibling();
            }
            _alternatives[alt.to] = alt;
        }
        return _alternatives;
    }

private:
    map<string, Alternative> _alternatives;
};
}

#endif /* CPS_ALTERNATIVESRESPONSE_HPP */
