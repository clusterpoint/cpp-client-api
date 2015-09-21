#ifndef CPS_STATUSRESPONSE_HPP
#define CPS_STATUSRESPONSE_HPP

#include <string>
#include <vector>
#include <map>

#include "../Response.hpp"
#include "../Utils.hpp"

namespace CPS
{
class StatusAuthCache
{
public:
    StatusAuthCache(double seconds, int size) {
        this->seconds = seconds;
        this->size = size;
    }
    virtual ~StatusAuthCache() {
    }

    double seconds;
    int size;
};

class StatusRepository
{
public:
    StatusRepository(int dataSize, int documents) {
        this->dataSize = dataSize;
        this->documents = documents;
    }
    virtual ~StatusRepository() {
    }

    /** total number of documents */
    int dataSize;
    /** total size of document repository */
    int documents;
};

class StatusIndex
{
public:
    StatusIndex(std::string indexSpeed, int memoryPoolUsage, std::string status,
                int totalWords) {
        this->indexSpeed = indexSpeed;
        this->memoryPoolUsage = memoryPoolUsage;
        this->status = status;
        this->totalWords = totalWords;
    }
    virtual ~StatusIndex() {
    }

    /** indexing speed MB/s */
    std::string indexSpeed;
    /** usage of memory pool in RAM */
    int memoryPoolUsage;
    /** index state: normal, dumping, merging */
    std::string status;
    /** number of all words in the Storage */
    int totalWords;
};

class StatusResponse: public Response
{
public:
    /**
     * Constructs Response object.
     * Response object cannot exist without response text to work on,
     * so no default constructor is provided
     * @param rawResponse string of raw response from CPS server. This should be valid XML
     */
    StatusResponse(std::string rawResponse) :
        Response(rawResponse) {
    	single = doc->FindFast("cps:reply/cps:content/all").size() == 0;
    	prefix = single ? "" : "all/";
    }
    virtual ~StatusResponse() {
    }

    /**
     * Returns Clusterpoint Server version number
     */
    std::string getVersion() {
        return getParam<std::string>("version", "");
    }

    /**
     * Returns quotient
     */
    int getQuotient() {
        return getParam<int>("quotient", 0);
    }

    /**
     * Returns license type: persistent/timed
     */
    std::string getLicense() {
        return getParam<std::string>(prefix + "license", "");
    }

    /**
     * Returns Auth Cache statistics
     */
    StatusAuthCache getAuthCache() {
        NodeSet ns = doc->FindFast(single ? "cps:reply/cps:content/auth_cache" : "cps:reply/cps:content/all/auth_cache", false);
        if (ns.size() != 1) {
            return StatusAuthCache(0.0, 0);
        }
        double seconds = 0.0;
        if (ns[0]->getChildren("seconds").size() > 0)
        	seconds = atof(ns[0]->getChildren("seconds").front()->getContentPtr());
        int size = 0;
        if (ns[0]->getChildren("size").size() > 0)
        	size = atoi(ns[0]->getChildren("size").front()->getContentPtr());
        return StatusAuthCache(seconds, size);
    }

    /**
     * Returns repository statistics
     */
    StatusRepository getRepository() {
        NodeSet ns = doc->FindFast("cps:reply/cps:content/repository", false);
        if (ns.size() != 1) {
            return StatusRepository(0, 0);
        }
        int dataSize = 0;
        if (ns[0]->getChildren("data_size").size() > 0)
        	dataSize = atoi(ns[0]->getChildren("data_size").front()->getContentPtr());
        int documents = 0;
        if (ns[0]->getChildren("documents").size() > 0)
        	documents = atoi(ns[0]->getChildren("documents").front()->getContentPtr());
        return StatusRepository(dataSize, documents);
    }

    /**
     * Returns index statistics
     */
    StatusIndex getIndex() {
        NodeSet ns = doc->FindFast("cps:reply/cps:content/index", false);
        if (ns.size() != 1) {
            return StatusIndex("", 0, "", 0);
        }
        int totalWords = 0;
        if (ns[0]->getChildren("total_words").size())
        	totalWords = atoi(ns[0]->getChildren("total_words").front()->getContentPtr());
        if (single == false)
        	ns = doc->FindFast("cps:reply/cps:content/all/index", false);
        if (ns.size() != 1) {
            return StatusIndex("", 0, "", totalWords);
        }
        std::string indexSpeed = "";
        if (ns[0]->getChildren("index_speed").size() > 0)
        	indexSpeed = ns[0]->getChildren("index_speed").front()->getContent();
        int memoryPoolUsage = 0;
        if (ns[0]->getChildren("memory_pool_usage").size() > 0)
        	memoryPoolUsage = atoi(ns[0]->getChildren("memory_pool_usage").front()->getContentPtr());
        std::string status = "";
        if (ns[0]->getChildren("status").size() > 0)
        	status = ns[0]->getChildren("status").front()->getContent();
        return StatusIndex(indexSpeed, memoryPoolUsage, status, totalWords);
    }

    /**
     * Returns progress for process.
     * Possible types: reindex, synchronize, backup, restore
     */
    std::string getProgress(const std::string& type) {
        return getParam<std::string>(prefix + type + "/progress", "");
    }

private:
    bool single;
    std::string prefix;
};
}

#endif /* CPS_LISTWORDSRESPONSE_HPP */
