#ifndef CPS_LISTWORDSRESPONSE_HPP
#define CPS_LISTWORDSRESPONSE_HPP

#include <string>
#include <vector>
#include <map>

#include "../Response.hpp"
#include "../Utils.hpp"

namespace CPS
{
class ListWordsResponse: public Response
{
public:
    /**
     * Constructs Response object.
     * Response object cannot exist without response text to work on,
     * so no default constructor is provided
     * @param rawResponse string of raw response from CPS server. This should be valid XML
     */
    ListWordsResponse(string rawResponse) :
        Response(rawResponse) {
    }
    virtual ~ListWordsResponse() {
    }

    /**
     * Returns words matching the given wildcard.
     * Return map with key as a word wildcard and value as matching word and count of this matching word found
     */
    map<string, map<string, int> > getWords() {
        if (!_words.empty())
            return _words;
        _words.clear();
        NodeSet ns = doc->FindFast("cps:reply/cps:content/list", true);
        for (unsigned int i = 0; i < ns.size(); i++) {
            string to = ns[i]->getAttribute("to")->getValue();
            Node* el = ns[i]->getFirstChild();
            while (el != NULL) {
                int count = atoi(el->getAttribute("count")->getContentPtr());
                string word = el->getContent();
                _words[to][word] = count;
                el = el->getNextSibling();
            }
        }
        return _words;
    }

private:
    map<string, map<string, int> > _words;
};
}

#endif /* CPS_LISTWORDSRESPONSE_HPP */
