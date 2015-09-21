#ifndef CPS_XMLDOCUMENT_HPP
#define CPS_XMLDOCUMENT_HPP

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

#include <map>
#include <list>
#include <stack>

namespace CPS
{
class Node;
typedef Node Element;
typedef Node ContentNode;
typedef Node TextNode;
typedef Node Attribute;

typedef std::vector<Node*> NodeSet;

class XMLDocument;

void AddNodeToSet(void *userdata, Node *node);
std::string xmlUtilCreatePath(const char * xpath, const char * value);

class Node
{
public:
    typedef std::map<std::string, std::string> PrefixNsMap;
    typedef std::list<Node *> NodeList;
    typedef std::list<Attribute *> AttributeList;
    Node(rapidxml::xml_node<> *node) {
        pNode = node;
        pAttr = NULL;
        char *nscol = NULL;
        if (pNode
                && pNode->name() && (nscol = strchr(pNode->name(), ':')) != NULL) {
            pNameSpace.assign(pNode->name(), nscol - pNode->name());
            pName = nscol + 1;
        } else
            pName = pNode->name();
    }
    Node(rapidxml::xml_attribute<> *attr) {
        pNode = NULL;
        pAttr = attr;
        char *nscol = NULL;
        if (pAttr && pAttr->name() && (nscol = strchr(pAttr->name(), ':')) != NULL) {
            pNameSpace.assign(pAttr->name(), nscol - pAttr->name());
            pName = nscol + 1;
        } else
            pName = pAttr->name();
    }
    virtual ~Node() {};

    std::string toString(bool formatted = true) {
    	std::string result;
        rapidxml::print(std::back_inserter(result), *pNode, (formatted) ? 0 : 1);
        return result;
    }

    void setName(const std::string &name) {
        if (pNode) {
            pNode->name(pNode->document()->allocate_string(name.c_str(), name.length() + 1), name.length());
            pName = pNode->name();
        }
        if (pAttr) {
            pAttr->name(pAttr->document()->allocate_string(name.c_str(), name.length() + 1), name.length());
            pName = pAttr->name();
        }
    }

    std::string getName() const {
        return getNamePtr();
    }

    const char* getNamePtr() const {
        if (!pName) {
            if (pNode) {
                char *colpos = strchr(pNode->name(), ':');
                if (colpos)
                    pName = colpos + 1;
                else
                    pName = pNode->name();
                return pName;
            }
            if (pAttr) {
                char *colpos = strchr(pAttr->name(), ':');
                if (colpos)
                    pName = colpos + 1;
                else
                    pName = pAttr->name();
                return pName;
            }
        }
        return pName;
    }

    Attribute* setAttribute(const std::string &name, const std::string &value, const std::string &ns_prefix = "") {
        if (!pNode)
            return NULL;

        std::string fulltag = ns_prefix.empty() ? name : ns_prefix + ":" + name;

        rapidxml::xml_attribute<> *attr = pNode->first_attribute(fulltag.c_str(), fulltag.length());
        if (attr) {
            attr->value(attr->document()->allocate_string(value.c_str(), value.length() + 1), value.length());
            if (!attr->_private) createWrapper(attr);
            Attribute *ret = static_cast<Attribute *>(attr->_private);
            return ret;
        } else {
            attr = pNode->document()->allocate_attribute(pNode->document()->allocate_string(fulltag.c_str(), fulltag.length() + 1), pNode->document()->allocate_string(value.c_str(), value.length() + 1), fulltag.length(), value.length());
            createWrapper(attr);
            pNode->append_attribute(attr);
            Attribute *ret = static_cast<Attribute *>(attr->_private);
            return ret;
        }
    }

    Attribute *getAttribute(const std::string &name, const std::string &ns_prefix = "") const {
        if (!pNode)
            return NULL;

        std::string fulltag = ns_prefix.empty() ? name : ns_prefix + ":" + name;

        rapidxml::xml_attribute<> *attr = pNode->first_attribute(fulltag.c_str(), fulltag.length());
        if (attr) {
            if (!attr->_private)
                createWrapper(attr);
            return static_cast<Attribute *>(attr->_private);
        } else
            return NULL;
    }

    AttributeList getAttributes() {
        AttributeList ret;
        if (!pNode)
            return ret;

        for (rapidxml::xml_attribute<> *attr = pNode->first_attribute(); attr != NULL; attr = attr->next_attribute()) {
            if (!attr->_private)
                createWrapper(attr);
            ret.push_back(static_cast<Attribute *>(attr->_private));
        }

        return ret;
    }

    void setNamespaceDeclaration(const std::string &ns_uri, const std::string &ns_prefix) {
        setAttribute(ns_prefix, ns_uri, "xmlns");
    }

    void setNamespace(const std::string &ns_prefix) {
        pNameSpace = ns_prefix;
        std::string fullname = ns_prefix + ":";
        if (pNode) {
            char *nscol = strchr(pNode->name(), ':');
            if (nscol)
                nscol += 1;
            else
                nscol = pNode->name();
            fullname.append(nscol);
            pNode->name(pNode->document()->allocate_string(fullname.c_str(), fullname.length() + 1), fullname.length());
        }
        if (pAttr) {
            char *nscol = strchr(pAttr->name(), ':');
            if (nscol)
                nscol += 1;
            else
                nscol = pAttr->name();
            fullname.append(nscol);
            pAttr->name(pAttr->document()->allocate_string(fullname.c_str(), fullname.length() + 1), fullname.length());
        }
    }

    std::string getNamespacePrefix() const {
        return pNameSpace;
    }

    const char* getNamespacePrefixPtr() const {
        if (pNameSpace.empty())
            return NULL;
        else
            return pNameSpace.c_str();
    }

    std::string getContent() const {
        return getContentPtr();
    }

    const char *getContentPtr() const {
        if (pNode)
            return pNode->value();
        if (pAttr)
            return pAttr->value();
        return NULL;
    }

    std::string getValue() const {
        return getContentPtr();
    }

    NodeList getChildren(const std::string &name) {
        NodeList ret;
        if (!pNode)
            return ret;

        // do we need to prepend namespace to tag name?
        std::string fulltag = name;
        for (rapidxml::xml_node<> *ch = pNode->first_node(fulltag.empty() ? NULL : fulltag.c_str() , fulltag.length()); ch != NULL; ch = ch->next_sibling()) {
            if (!fulltag.empty() && (!ch->name() || strcmp(ch->name(), fulltag.c_str()) != 0))
                break;
            if (!ch->_private)
                createWrapper(ch);
            ret.push_back(static_cast<Node *>(ch->_private));
        }
        return ret;
    }

    const NodeList getChildren(const std::string &name) const {
        NodeList ret;
        if (!pNode)
            return ret;

        // do we need to prepend namespace to tag name?
        std::string fulltag = name;
        for (rapidxml::xml_node<> *ch = pNode->first_node(fulltag.empty() ? NULL : fulltag.c_str(), fulltag.length()); ch != NULL; ch = ch->next_sibling()) {
            if (!fulltag.empty() && (!ch->name() || strcmp(ch->name(), fulltag.c_str()) != 0))
                break;
            if (!ch->_private)
                createWrapper(ch);
            ret.push_back(static_cast<Node *>(ch->_private));
        }
        return ret;
    }

    Element *getParent() {
        if (pNode && pNode->parent()) {
            rapidxml::xml_node<> *par = pNode->parent();
            if (par == pNode->document() || par->type() == rapidxml::node_document)
                return NULL;
            if (!par->_private)
                createWrapper(par);
            return static_cast<Node *>(par->_private);
        }
        if (pAttr && pAttr->parent()) {
            rapidxml::xml_node<> *par = pAttr->parent();
            if (!par->_private)
                createWrapper(par);
            return static_cast<Node *>(par->_private);
        }
        return NULL;
    }

    const Element *getParent() const {
        if (pNode && pNode->parent()) {
            rapidxml::xml_node<> *par = pNode->parent();
            if (par == pNode->document() || par->type() == rapidxml::node_document)
                return NULL;
            if (!par->_private)
                createWrapper(par);
            return static_cast<Node *>(par->_private);
        }
        if (pAttr && pAttr->parent()) {
            rapidxml::xml_node<> *par = pAttr->parent();
            if (!par->_private)
                createWrapper(par);
            return static_cast<Node *>(par->_private);
        }
        return NULL;
    }

    Node *getFirstChild() const {
        if (!pNode || !pNode->first_node())
            return NULL;

        rapidxml::xml_node<> *ch = pNode->first_node();
        if (!ch->_private)
            createWrapper(ch);
        return static_cast<Node *>(ch->_private);
    }

    Attribute *getFirstAttribute() const {
        if (!pNode || !pNode->first_attribute())
            return NULL;

        rapidxml::xml_attribute<> *ch = pNode->first_attribute();
        if (!ch->_private)
            createWrapper(ch);
        return static_cast<Attribute *>(ch->_private);
    }

    const char *getFirstChildText() const {
        if (pNode && pNode->first_node())
            return pNode->first_node()->value();
        else
            return NULL;
    }

    TextNode *getChildText() {
        if (pNode)
            for (rapidxml::xml_node<> *ch = pNode->first_node(); ch != NULL; ch = ch->next_sibling())
                if (ch->type() == rapidxml::node_data) {
                    if (!ch->_private)
                        createWrapper(ch);
                    return static_cast<Node *>(ch->_private);
                }
        return NULL;
    }

    const TextNode *getChildText() const {
        if (pNode)
            for (rapidxml::xml_node<> *ch = pNode->first_node(); ch != NULL; ch = ch->next_sibling())
                if (ch->type() == rapidxml::node_data) {
                    if (!ch->_private)
                        createWrapper(ch);
                    return static_cast<Node *>(ch->_private);
                }
        return NULL;
    }

    Node *getNextSibling() const {
        if (pNode && pNode->next_sibling()) {
            rapidxml::xml_node<> *nn = pNode->next_sibling();
            if (!nn->_private)
                createWrapper(nn);
            return static_cast<Node *>(nn->_private);
        }
        if (pAttr && pAttr->next_attribute()) {
            rapidxml::xml_attribute<> *nn = pAttr->next_attribute();
            if (!nn->_private)
                createWrapper(nn);
            return static_cast<Attribute *>(nn->_private);
        }
        return NULL;
    }

    Attribute *getNextAttribute() const {
        if (pAttr && pAttr->next_attribute()) {
            rapidxml::xml_attribute<> *nn = pAttr->next_attribute();
            if (!nn->_private)
                createWrapper(nn);
            return static_cast<Attribute *>(nn->_private);
        }
        return NULL;
    }

    Node* importNode(const Node *node, bool recursive = true, bool first = false) {
        rapidxml::xml_node<> *src = node->pNode;
        rapidxml::xml_node<> *dst = pNode;

        if (!src || !dst)
            return NULL;

        if (src->document() != dst->document()) {
            src = dst->document()->clone_node(src, NULL, true, recursive);
            createWrapper(src);
        } else if (!src->_private) {
            createWrapper(src);
        }

        if (first)
            dst->prepend_node(src);
        else
            dst->append_node(src);

        return static_cast<Node *>(src->_private);
    }

    Element* addChild(const std::string &name, const std::string &ns_prefix = "") {
        if (!pNode)
            return NULL;

        rapidxml::xml_node<> *nn = NULL;
        if (!ns_prefix.empty()) {
        	std::string fulltag = ns_prefix + ":" + name;
            nn = pNode->document()->allocate_node(rapidxml::node_element, pNode->document()->allocate_string(fulltag.c_str(), fulltag.length() + 1), NULL, fulltag.length(), 0);
        } else {
            nn = pNode->document()->allocate_node(rapidxml::node_element, pNode->document()->allocate_string(name.c_str(), name.length() + 1), NULL, name.length(), 0);
        }
        pNode->append_node(nn);
        createWrapper(nn);
        Element *ret = static_cast<Element *>(nn->_private);
        return ret;
    }

    TextNode* addChildText(const std::string &content) {
        if (!pNode)
            return NULL;

        rapidxml::xml_node<> *nn = pNode->document()->allocate_node(rapidxml::node_data, NULL, pNode->document()->allocate_string(content.c_str(), content.length() + 1), 0, content.length());
        pNode->append_node(nn);
        createWrapper(nn);
        TextNode *ret = static_cast<TextNode *>(nn->_private);
        return ret;
    }

    static void createWrapper(rapidxml::xml_node<> *node) {
        if (!node->_private)
            node->_private = new Node(node);
    }

    static void createWrapper(rapidxml::xml_attribute<> *attr) {
        if (!attr->_private)
            attr->_private = new Attribute(attr);
    }

    static void freeWrappers(rapidxml::xml_node<> *node) {
        for (rapidxml::xml_node<> *child = node->first_node(); child != NULL; child = child->next_sibling())
            freeWrappers(child);

        for (rapidxml::xml_attribute<> *attr = node->first_attribute(); attr != NULL; attr = attr->next_attribute())
            freeWrappers(attr);

        if (node->_private)
            delete static_cast<Node *>(node->_private);
    }

    static void freeWrappers(rapidxml::xml_attribute<> *attr) {
        if (attr->_private)
            delete static_cast<Attribute *>(attr->_private);
    }

    bool isElementNode() const {
        return (pNode && pNode->type() == rapidxml::node_element);
    }

    bool isTextNode() const {
        return (pNode && pNode->type() == rapidxml::node_data);
    }

    bool isCdataNode() const {
        return (pNode && pNode->type() == rapidxml::node_cdata);
    }

    bool isBlankNode() const {
        if (pNode) {
            if (pNode->type() == rapidxml::node_element && !pNode->first_node())
                return true;

            if (pNode->type() == rapidxml::node_data) {
                bool isws = true;
                for (size_t t = 0; t < pNode->value_size() && isws; t++)
                    isws = isws && (pNode->value()[t] == ' '); // is 'whitespace' only 'space' or some additional characters like 'newline', 'tab', etc?
                return isws;
            }

            return false;
        } else
            return false;
    }

    bool isAttribute() const {
        return (pAttr != NULL);
    }

    rapidxml::xml_node<> *pNode;
    rapidxml::xml_attribute<> *pAttr;
    std::string pNameSpace;
    mutable char *pName;
};

class XMLDocument
{
public:
    XMLDocument(): pDoc(NULL) {
    }

    ~XMLDocument() {
        for (rapidxml::xml_node<> *child = pDoc->first_node(); child != NULL;
                child = child->next_sibling())
            Node::freeWrappers(child);

        for (rapidxml::xml_attribute<> *attr = pDoc->first_attribute();
                attr != NULL; attr = attr->next_attribute())
            Node::freeWrappers(attr);

        delete pDoc;
    }

    static XMLDocument* create(rapidxml::xml_document<>* doc) {
        XMLDocument *ret = new XMLDocument();
        ret->pDoc = doc;
        return ret;
    }
    static XMLDocument* parseFromMemory(std::string contents) {
        XMLDocument *ret = new XMLDocument();
        rapidxml::xml_document<>* doc = new rapidxml::xml_document<>();
        contents.push_back(0);
        char *xml_string = doc->allocate_string(contents.c_str(),
                                                contents.size());
#ifdef CPS_XMLDOCUMENT_HPP_PARSE_FULL
        doc->parse<rapidxml::parse_full>(xml_string);
#else // CPS_XMLDOCUMENT_HPP_PARSE_FULL
        doc->parse<0>(xml_string);
#endif // CPS_XMLDOCUMENT_HPP_PARSE_FULL
        ret->pDoc = doc;
        return ret;
    }

    NodeSet FindFast(const char *xp_string, bool multiple_matches = true) {
        NodeSet result;
        while (*xp_string == '/') {
            xp_string++;
        }

        if (!*xp_string) {
            // empty xpath
            result.push_back(this->getRootNode());
            return result;
        }

        this->findXpath(NULL, this->getRootNode(), multiple_matches, xp_string,
                        &AddNodeToSet, &result);
        return result;
    }

    NodeSet FindFast(const std::string &xpath, bool multiple_matches = true) {
        const char *xp_string = xpath.c_str();
        return this->FindFast(xp_string, multiple_matches);
    }

    inline NodeSet FindFast(Node * const &where, bool multiple_matches = true) {
        NodeSet result;
        result.push_back(where);
        return result;
    }

    rapidxml::xml_document<>* getDoc() {
        return this->pDoc;
    }
    Node* getNode() {
        return this->getRootNode();
    }

    std::string toString(bool format = true) {
    	std::string xml_as_string;
        rapidxml::print(std::back_inserter(xml_as_string), *pDoc, !format);
        return xml_as_string;
    }

    Element* createRootNode(const std::string &name, const std::string &ns_uri,
                            const std::string &ns_prefix) {
        pDoc->remove_all_nodes();
        rapidxml::xml_node<> *node = NULL;
        if (!ns_prefix.empty()) {
            std::string fulltag = ns_prefix + ":" + name;
            node = pDoc->allocate_node(rapidxml::node_element,
                                       pDoc->allocate_string(fulltag.c_str(),
                                               fulltag.length() + 1), NULL, fulltag.length(), 0);
            fulltag = "xmlns:" + ns_prefix;
            node->append_attribute(
                pDoc->allocate_attribute(
                    pDoc->allocate_string(fulltag.c_str(),
                                          fulltag.length() + 1),
                    pDoc->allocate_string(ns_uri.c_str(),
                                          ns_uri.length() + 1), fulltag.length(),
                    ns_uri.length()));
        } else {
            node = pDoc->allocate_node(rapidxml::node_element,
                                       pDoc->allocate_string(name.c_str(), name.length() + 1),
                                       NULL, name.length(), 0);
        }
        pDoc->append_node(node);
        Node::createWrapper(node);
        Node *ret = static_cast<Node *>(node->_private);
        return ret;
    }

    Element* getRootNode() {
        rapidxml::xml_node<> *node = pDoc->first_node();
        for (; node != NULL; node = node->next_sibling())
            if (node->type() == rapidxml::node_element) {
                if (!node->_private)
                    Node::createWrapper(node);
                return static_cast<Node *>(node->_private);
            }
        return NULL;
    }

    template<class Path> Node *CreateNode(const Path &where,
                                          const std::string &tagname) {
        if (!pDoc)
            return NULL;

        Node *ret = NULL;
        NodeSet nodes = this->FindFast(where, false);
        NodeSet::iterator it;

        for (it = nodes.begin(); it != nodes.end(); it++) {
            size_t nsc = tagname.find(":");
            if (nsc == tagname.npos)
                ret = (*it)->addChild(tagname);
            else
                ret = (*it)->addChild(tagname.substr(nsc + 1),
                                      tagname.substr(0, nsc));
        }

        return ret;
    }
    template<class Path> Node *CreateNodeString(const Path &where,
            const std::string &tagname, const std::string &contents) {
        Element *ret = NULL;
        try {
            ret = dynamic_cast<Element *>(this->CreateNode(where, tagname));
        } catch (std::exception &e) {
            return NULL;
        }
        if (!ret)
            return NULL;
        ret->addChildText(contents);
        return ret;
    }
    template<class Path> Node *CreateNodeInt(const Path &where,
            const std::string &tagname, const int &contents) {
        char buf[1024] = "";
        sprintf(buf, "%d", contents);
        return CreateNodeString(where, tagname, buf);
    }
    template<class Path> Node *CreateNodeFloat(const Path &where,
            const std::string &tagname, const double &contents) {
        char buf[1024] = "";
        sprintf(buf, "%lf", contents);
        return CreateNodeString(where, tagname, buf);
    }
    template<class Path> Node *CreateNodeBool(const Path &where,
            const std::string &tagname, const bool &contents) {
        if (contents)
            return CreateNodeString(where, tagname, "yes");
        else
            return CreateNodeString(where, tagname, "no");
    }
    template<class Path> int CreateAttributeString(const Path &where,
            const std::string &attrname, const std::string &contents) {
        if (!pDoc)
            return 0;

        int ret = 0;
        NodeSet nodes = this->FindFast(where);
        NodeSet::iterator it;

        for (it = nodes.begin(); it != nodes.end(); it++) {
            Element *ne = dynamic_cast<Element *>(*it);
            if (ne) {
                size_t nsc = attrname.find(":");
                if (nsc == attrname.npos)
                    ne->setAttribute(attrname, contents);
                else
                    ne->setAttribute(attrname.substr(nsc + 1), contents,
                                     attrname.substr(0, nsc));
                ret++;
            }
        }

        return ret;
    }
    template<class Path> int CreateAttributeInt(const Path &where,
            const std::string &attrname, const int &contents) {
        char buf[1024] = "";
        sprintf(buf, "%d", contents);
        return CreateAttributeString(where, attrname, buf);
    }
    template<class Path> int CreateAttributeFloat(const Path &where,
            const std::string &attrname, const double &contents) {
        char buf[1024] = "";
        sprintf(buf, "%lf", contents);
        return CreateAttributeString(where, attrname, buf);
    }
    template<class Path> int CreateAttributeBool(const Path &where,
            const std::string &attrname, const bool &contents) {
        if (contents)
            return CreateAttributeString(where, attrname, "yes");
        else
            return CreateAttributeString(where, attrname, "no");
    }

private:
    rapidxml::xml_document<>* pDoc;

    void findXpath(Node *const_child, Node *child, bool multiple_matches,
                   const char *start_pos, void (*func)(void *, Node *), void *userdata) {
        int node_number = -1;
        const char *ns = NULL;
        int ns_length = 0;
        const char *name = start_pos;
        int name_length = -1;

        const char *cur_pos = start_pos;
        const char *bracket_pos = NULL;
        bool ends_with_asterisk = false;
        bool process_string = true;
        bool select_attribute = false;
        while (process_string) {
            switch (*cur_pos) {
            case 0: // end of string
            case '/': // end of xpath
            case '*': // only works if it's at the end
            case '@':
                if (name_length == -1) {
                    name_length = cur_pos - name;
                }
                process_string = false;
                if (*cur_pos == '*') {
                    ends_with_asterisk = true;
                    while ((*cur_pos != 0) && (*cur_pos != '/')) {
                        cur_pos++;
                    }
                }
                if (*cur_pos == '@')
                    select_attribute = true;
                break;
            case '[':
                bracket_pos = cur_pos;
                name_length = cur_pos - name;
                break;
            case ']':
                if (bracket_pos) {
                    node_number = atoi(bracket_pos + 1);
                }
                break;
            case ':':
                ns = start_pos;
                ns_length = cur_pos - start_pos;
                name = cur_pos + 1;
                break;
            }
            ++cur_pos;
        }
        --cur_pos;

        int found_paths = 0;

        while (child) {
            if ((child->isElementNode()
                    || (child->isTextNode() && name_length == 0 && *name == '*'))
                    && (((int) strlen((char *) child->getNamePtr())
                         == name_length) || ends_with_asterisk
                        || (name_length == 0 && select_attribute))
                    && (strncmp((char *) child->getNamePtr(), name, name_length)
                        == 0)
                    && ((!ns && !child->getNamespacePrefixPtr())
                        || (ns && child->getNamespacePrefixPtr()
                            && ((int) strlen(
                                    (char *) child->getNamespacePrefixPtr())
                                == ns_length)
                            && (strncmp(
                                    (char *) child->getNamespacePrefixPtr(),
                                    ns, ns_length) == 0))
                        || (!ns && ends_with_asterisk))) {
                // element name & namespace matches
                if ((node_number == -1) || (node_number == found_paths + 1)) {
                    // number also matches or absent
                    while (*cur_pos == '/') {
                        cur_pos++;
                    }
                    if (*cur_pos) {
                        if (select_attribute) {
                            char *nspos = strchr((char *) cur_pos + 1, ':');
                            Attribute *atr = NULL;
                            if (nspos) {
                                *nspos = '\0';
                                child->getAttribute(nspos + 1, cur_pos + 1);
                            } else
                                atr = child->getAttribute(cur_pos + 1);
                            if (atr)
                                func((void *) userdata, atr);
                        } else
                            // traverse to find the children
                            if (!select_attribute && child->getFirstChild())
                                this->findXpath(const_child, child->getFirstChild(),
                                                multiple_matches, cur_pos, func, userdata);
                    } else {
                        // no more xpath, we found our tag!
                        //func((void *)userdata, reinterpret_cast<xmlpp::Node *>(child->_private));
                        func((void *) userdata, child);
                        if (!multiple_matches)
                            break;
                    }
                }
                found_paths++;
            }
            if (const_child != child)
                child = child->getNextSibling();
            else
                child = NULL;
        }
    }
};

inline void AddNodeToSet(void *userdata, Node *node)
{
    ((NodeSet *) userdata)->push_back(node);
}

inline std::string xmlUtilCreatePath(const char * xpath, const char * value)
{
	std::string xp(xpath), output;
	std::stack<std::string> xp_stack;
    size_t pos, start = 0;
    while ((pos = xp.find('/', start)) != std::string::npos) {
        if (start + 1 <= pos)
            xp_stack.push(xp.substr(start, pos - start));
        start = pos + 1;
    }
    //if we are not at the end of xpath push last tag
    if (start < xp.size())
        xp_stack.push(xp.substr(start));
    while (xp_stack.size()) {
        if (!output.size())
            output = std::string(
                         "<" + xp_stack.top() + ">" + value + "</" + xp_stack.top()
                         + ">");
        else
            output = std::string(
                         "<" + xp_stack.top() + ">" + output + "</" + xp_stack.top()
                         + ">");
        xp_stack.pop();
    }
    return output;
}
}

#endif //#ifndef CPS_XMLDOCUMENT_HPP
