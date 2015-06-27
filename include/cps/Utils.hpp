#ifndef CPS_UTILS_HPP
#define CPS_UTILS_HPP

#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>

using namespace std;

namespace CPS
{
class Error
{
public:
    Error() {
    }
    virtual ~Error() {
    }

    /** Error code */
    string code;
    /** Error text */
    string text;
    /** Error message */
    string message;
    /** Error severity */
    string level;
    /** Error source */
    string source;
    /** Document ids affected */
    vector<string> documentIds;
};

namespace Utils
{
inline string &str_replace(const string &needle, const string &replace,
                    string &haystack)
{
    string oldstack = haystack;
    size_t nlen = needle.length(), pos = 0;

    do {
        oldstack = haystack;

        if ((pos = haystack.find(needle)) != haystack.npos)
            haystack.replace(pos, nlen, replace);
    } while (oldstack != haystack);

    return haystack;
}

/**
 * Replace ', &, ", < and > characters with their XML entities
 * @param text string to escape
 */
inline string xmlspecialchars(const string &text)
{
    string ret = text;
    str_replace("&", "&amp;", ret);
    str_replace("\"", "&quot;", ret);
    str_replace("'", "&apos;", ret);
    str_replace("<", "&lt;", ret);
    str_replace(">", "&gt;", ret);
    return ret;
}

/**
 * Explode passed string at delimiter positions
 * @param delimiter string of characters that can act as delimiters, string will be split when ANY of them is encountered
 * @param data string to explode
 * @param limit maximum number of splits to return
 * @param returnEmpty should empty splits be returned
 */
inline vector<string> explode(const string &delimiter, const string &data,
                       const int limit = 0, const bool returnEmpty = true)
{
    vector<string> ret;
    size_t p1 = 0, p2 = 0;

    ret.clear();

    if (data.empty())
        return ret;

    if (delimiter.length() <= 0) {
        if (returnEmpty || !data.empty())
            ret.push_back(data);
        return ret;
    }

    while (limit <= 0 || (int) ret.size() < limit) {
        p2 = data.find(delimiter, p1);
        if (p2 == data.npos) {
            if (returnEmpty || p1 < data.length())
                ret.push_back(data.substr(p1));
            break;
        }

        if (returnEmpty || p1 < p2)
            ret.push_back(data.substr(p1, p2 - p1));
        p1 = p2 + delimiter.length();
    }

    return ret;
}

/**
 * Join vector of string together using delimeter
 * @param strings vector of strings to join together
 * @param delim delimiter to use when joining (default = ", ")
 */
inline string join(const vector <string> &strings, const string &delim = ", ") {
    string ret;
    for (vector <string>::const_iterator it = strings.begin(); it != strings.end(); ++it) {
        if (it != strings.begin()) ret += delim;
        ret += *it;
    }
    return ret;
}

/**
 * Trim string by removing all provided characters from start and end of string
 * @param input string to trim
 * @param chars characters to trim default are space, tab and new line symbols
 */
inline string &trim(string &input, const string &chars = " \t\n\r")
{
    size_t first = input.find_first_not_of(chars);
    if (first == input.npos) { // all chars are bad
        input.clear();
        return input;
    }
    size_t last = input.find_last_not_of(chars);
    // last == input.npos -> impossible, in this case previous test will fail first

    if (first > 0 || last + 1 < input.length())
        input = input.substr(first, last - first + 1);
    return input;
}

/**
 * Convert value to string by using boost::lexical_cast
 * @param value value to covert to string
 */
template<class T>
inline string toString(const T& value)
{
    return boost::lexical_cast<string>(value);
}
} //namespace Utils
} //namespace cps

#endif //#ifndef CPS_UTILS_HPP
