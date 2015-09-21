#ifndef CPS_MODIFYREQUEST_HPP
#define CPS_MODIFYREQUEST_HPP

#include <string>
#include <vector>
#include <map>

#include "../Request.hpp"
#include "../Utils.hpp"

namespace CPS
{
class ModifyRequest: public Request
{
public:
    /**
     * @param command name of the command
     */
    ModifyRequest(const std::string &command) :
        Request(command) {
    }
    /**
     * @param command name of the command
     * @param document document XML as string
     */
    ModifyRequest(const std::string &command, const std::string &document) :
        Request(command) {
        setDocument(document);
    }
    /**
     * @param command name of the command
     * @param document document XML as string
     */
    ModifyRequest(const std::string &command, const char document[]) :
        Request(command) {
        setDocument(document);
    }
    /**
     * @param command name of the command
     * @param id id of document
     * @param document document XML as string
     */
    ModifyRequest(const std::string &command, const std::string &id, const std::string &document) :
        Request(command) {
        setDocument(id, document);
    }
    /**
     * @param command name of the command
     * @param documents documents as array of strings or map with key as document id and value as document XML as string
     */
    template<class T> ModifyRequest(const std::string& command, const T &documents) :
        Request(command) {
        setDocuments(documents);
    }
    virtual ~ModifyRequest() {
    }
};

class InsertRequest: public ModifyRequest
{
public:
    /**
     * @param id id of document to insert
     * @param document document XML as string
     */
    InsertRequest(const std::string &id, const std::string &document) :
        ModifyRequest("insert", id, document) {
    }
    /**
     * @param documents document to insert in form of a string, map <id, string> or vector of strings
     */
    template<class T> InsertRequest(const T &documents) :
        ModifyRequest("insert", documents) {
    }
    virtual ~InsertRequest() {
    }
};

class UpdateRequest: public ModifyRequest
{
public:
    /**
     * @param id id of document to replace
     * @param document string to update found document with
     */
    UpdateRequest(const std::string &id, const std::string &document) :
        ModifyRequest("update", id, document) {
    }
    /**
     * @param documents document to update in form of a string, map <id, string> or vector of strings
     */
    template<class T> UpdateRequest(const T &documents) :
        ModifyRequest("update", documents) {
    }
    virtual ~UpdateRequest() {
    }
};

class ReplaceRequest: public ModifyRequest
{
public:
    /**
     * @param id id of document to update
     * @param document string to replace found document with
     */
    ReplaceRequest(const std::string &id, const std::string &document) :
        ModifyRequest("replace", id, document) {
    }
    /**
     * @param documents document to replace in form of a string, map <id, string> or vector of strings
     */
    template<class T> ReplaceRequest(const T &documents) :
        ModifyRequest("replace", documents) {
    }
    virtual ~ReplaceRequest() {
    }
};

class PartialReplaceRequest: public ModifyRequest
{
public:
    /**
     * @param id id of document to partially replace
     * @param document string to partially replace found document with
     */
    PartialReplaceRequest(const std::string &id, const std::string &document) :
        ModifyRequest("partial-replace", id, document) {
    }
    /**
     * @param documents document to partially replace in form of a string, map <id, string> or vector of strings
     */
    template<class T> PartialReplaceRequest(const T &documents) :
        ModifyRequest("partial-replace", documents) {
    }
    virtual ~PartialReplaceRequest() {
    }
};

class DeleteRequest: public ModifyRequest
{
public:
    /**
     * @param id ID of document to delete
     */
    DeleteRequest(const std::string &id) :
        ModifyRequest("delete", id, "") {
    }
    /**
     * @param ids array of ids to delete
     */
    DeleteRequest(const std::vector<std::string> &ids) :
        ModifyRequest("delete") {
        for (unsigned int i = 0; i < ids.size(); i++) {
            setDocument(ids[i], "");
        }
    }
    virtual ~DeleteRequest() {
    }
};
}

#endif //#ifndef CPS_MODIFYREQUEST_HPP
