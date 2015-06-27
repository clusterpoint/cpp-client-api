#ifndef CPS_CONNECTION_HPP
#define CPS_CONNECTION_HPP

#include <cstring> //strncmp
#include <string>
#include <vector>
#include <map>

#ifndef USE_HEADER_ONLY_ASIO
    #include "boost/asio.hpp"
    using namespace boost;
#else
    #define ASIO_DISABLE_THREADS // To disable linking errors
    #include "asio.hpp"
#endif

#include "Response.hpp"
#include "Request.hpp"
#include "Exception.hpp"
#include "Protobuf.hpp"
#include "Utils.hpp"
#include "Socket.hpp"

namespace CPS
{

/**
 * @brief Socket connection type
 * Correct type is extracted from passed Connection::connectionString to Connection constructor
 */
enum ConnectionType {
    SOCKET, TCP, HTTP
};

class Connection
{
public:
    /**
     * @brief Connection API to Clusterpoint Server
     *
     * Constructs an instance of the Connection class. Note that it doesn't
     * necessarily make a connection to CPS when the constructor is called.
     *
     * @param connectionString Specifies the connection string, such as tcp://127.0.0.1:5550
     * @param storageName The name of the storage you want to connect to
     * @param username Username for authenticating with the storage
     * @param password Password for this user
     * @param documentRootXpath Document root tag name. Default is "document"
     * @param documentIdXpath Document ID xpath. Default is "document/id"
     */
    Connection(std::string connectionString, std::string storageName,
               std::string username, std::string password,
               std::string documentRootXpath = "document",
               std::string documentIdXpath = "document/id",
               std::map<std::string, std::string> customEnvelopeParams = Request::MapStringStringType()) {
        this->connectionString = connectionString;
        this->storageName = storageName;
        this->username = username;
        this->password = password;
        // Check if documentIdXpath is under documentrootXpath
        Utils::trim(documentRootXpath, "/");
        Utils::trim(documentIdXpath, "/");
        if (strncmp(documentIdXpath.c_str(), documentRootXpath.c_str(), documentRootXpath.size()) != 0) {
            BOOST_THROW_EXCEPTION(CPS::Exception("Document root xpath not matching document ID xpath", 9003));
        }
        if (documentRootXpath.size() == documentIdXpath.size()) {
            BOOST_THROW_EXCEPTION(CPS::Exception("Empty document id xpath", 9003));
        }
        this->documentRootXpath = documentRootXpath;
        this->documentIdXpath = documentIdXpath;
        this->customEnvelopeParams = customEnvelopeParams;
        // Parse connection string
        if (connectionString.empty() || connectionString == "unix://") {
            // Default connection
            this->connectionType = SOCKET;
            std::string escapedStorageName = this->storageName;
            // Escape all forward slashes (/) to be compliant with URI scheme
            for (unsigned int i = 0; i < escapedStorageName.size(); i++) {
                if (escapedStorageName[i] == '/')
                    escapedStorageName[i] = '_';
            }
            this->host = "/usr/local/cps2/storages/" + escapedStorageName + "/storage.sock";
            socket = boost::shared_ptr<AbstractSocket>(new UnixSocket(this->io_service));
        } else if (strncmp(connectionString.c_str(), "http://", 7) == 0) {
            // HTTP connection
            this->connectionType = HTTP;
            this->url = connectionString;
            // Parse given string
            int pos = 0;
            this->host = this->url.substr(7);
            this->port = 80;
            this->url = "/";
            if ((pos = this->host.find_first_of("/")) != -1) {
                this->url = this->host.substr(pos);
                this->host = this->host.substr(0, pos);
            }
            if ((pos = this->host.find_first_of(":")) != -1) {
                this->port = atoi(this->host.substr(pos + 1).c_str());
                this->host = this->host.substr(0, pos);
            }
            socket = boost::shared_ptr<AbstractSocket>(new HttpSocket(this->io_service, this->host, this->port, this->url));
        } else if (strncmp(connectionString.c_str(), "unix://", 7) == 0) {
            // Unix socket
            this->connectionType = SOCKET;
            this->host = connectionString.substr(7);
            this->port = 0;
#ifdef BOOST_ASIO_HAS_LOCAL_SOCKETS
            socket = boost::shared_ptr<AbstractSocket>(new UnixSocket(this->io_service));
#else
            BOOST_THROW_EXCEPTION(CPS::Exception("Does not have local sockets enabled (BOOST_ASIO_HAS_LOCAL_SOCKETS)"));
#endif
        } else if (strncmp(connectionString.c_str(), "tcp://", 6) == 0) {
            // TCP socket
            this->connectionType = TCP;
            // Extract port or default to 5550
            int portDelim = connectionString.find_first_of(':', 6);
            if (portDelim == -1) {
                this->host = connectionString.substr(6);
                this->port = 5550;
            } else {
                this->host = connectionString.substr(6, portDelim - 6);
                this->port = atoi(connectionString.substr(portDelim + 1).c_str());
            }
            socket = boost::shared_ptr<AbstractSocket>(new TcpSocket(this->io_service));
        } else {
            BOOST_THROW_EXCEPTION(CPS::Exception("Invalid connection protocol", 9004));
        }

        this->applicationId = "CPS_CPP_API";
        this->debug = false;
        this->noCdata = false;
        this->createXML = false;
        this->transactionId = -1;
    }

    virtual ~Connection() {
    }

    /**
     * Connect to server
     */
    void connect() {
        // Connect to server if needed
        if (socket->isConnected() == false) {
            try {
                socket->connect(this->host, this->port);
            } catch (std::exception &e) {
                BOOST_THROW_EXCEPTION(CPS::Exception(string("Connection error - ") + e.what()));
            }
        }
    }

    /**
     * Sends raw xml message (no parsing or formatting is performed)
     *
     * @param xml string
     */
    template<class ResponseType>
    ResponseType *sendRequestRaw(string message) {
        if (this->debug)
            std::cout << "Request:\n" << message << std::endl;

        this->connect();

        try {
            if (this->connectionType == HTTP) {
                // Only HTTP requests send unformatted data
                std::vector<unsigned char> reply = socket->send(message);
                if (this->debug)
                            std::cout << "Response:\n" << std::string(reply.begin(), reply.end()) << std::endl;
                return new ResponseType(std::string(reply.begin(), reply.end()));
            } else {
                // Other requests send data formated using ProtoBuffers
                // (http://code.google.com/apis/protocolbuffers/docs/encoding.html)
                std::string frame = "";

                Protobuf pb;
                pb.newFieldString(1, message);
                if (this->storageName.size() > 0) pb.newFieldString(2, this->storageName);
//                pb.newFieldBool(12, true);
//                pb.newFieldString(13, std::to_string(rand()));
                frame += pb.toString();

                frame = header(frame.size()) + frame;

                // Send request and get reply
                std::vector<unsigned char> reply = socket->send(frame);

                pb.fromBytes(reply);

                if (this->debug)
                            std::cout << "Response:\n" << pb.getField(1)->data << std::endl;

                ResponseType *resp = new ResponseType(pb.getField(1)->data);
                resp->documentRootXpath = this->documentRootXpath;
                resp->documentIdXpath = this->documentIdXpath;
                if (resp->getCommand() == "begin-transaction") {
                	this->transactionId = resp->getParam("transaction_id", -1LL);
                } else if (resp->getCommand() == "commit-transaction" || resp->getCommand() == "rollback-transaction") {
                	this->transactionId = -1;
                }
                if (resp->getErrors().size() > 0 && resp->hasFailed()) {
                	vector <Error> errors = resp->getErrors();
                	BOOST_THROW_EXCEPTION(CPS::Exception(errors[0].message, boost::lexical_cast<int>(errors[0].code), resp));
                }
                return resp;
            }
        } catch (CPS::Exception &e) {
        	// Redirect valid exception up the chain
        	throw e;
        } catch (std::exception &e) {
            BOOST_THROW_EXCEPTION(CPS::Exception(string("Error while sending - ") + e.what()));
        }
    }

    /**
     * Sends the raw xml request and returns generic response
     * @see sendRequest(string message)
     */
    Response *sendRequestRaw(string message) {
        return sendRequestRaw<Response>(message);
    }

    /**
     * @brief Sends the request to CPS
     *
     * For response types see Response
     * @see Response Request
     *
     * @param request An object of the class Request
     * This is slower but might give You feedback if XML is not valid before sending to server
     */
    template<class ResponseType>
    ResponseType* sendRequest(const Request &request) {
        // Create request
        std::map<string, vector<string> > envelopeParams;
        for (std::map<string, string>::iterator it = this->customEnvelopeParams.begin(); it != this->customEnvelopeParams.end(); ++it) {
        	envelopeParams[it->first].push_back(it->second);
        }
        envelopeParams["storage"].push_back(this->storageName);
        envelopeParams["user"].push_back(this->username);
        envelopeParams["password"].push_back(this->password);
        envelopeParams["command"].push_back(request.getCommand());
        if (!request.getRequestId().empty())
            envelopeParams["requestid"].push_back(request.getRequestId());
        if (!this->applicationId.empty())
            envelopeParams["application"].push_back(this->applicationId);
        if (!request.getRequestType().empty())
            envelopeParams["type"].push_back(request.getRequestType());
        if (!request.getClusterLabel().empty())
            envelopeParams["label"].push_back(request.getClusterLabel());

        std::string message = request.getRequestXml(this->documentRootXpath,
                              this->documentIdXpath, envelopeParams, this->createXML, this->transactionId);

        return sendRequestRaw<ResponseType>(message);
    }

    /**
     * Sends the request and returns generic response
     * @see sendRequest(const Request &request)
     */
    Response *sendRequest(const Request &request) {
        return sendRequest<Response>(request);
    }

    /**
     * @brief Sets the application ID for the request
     *
     * This method can be used to set a custom application ID for your requests.
     * This can be useful to identify requests sent by a particular application in a log file
     *
     * @param applicationId a string
     */
    void setApplicationId(const std::string &applicationId = "CPS_CPP_API") {
        this->applicationId = applicationId;
    }

    /**
     * Sets the debugging mode
     *
     * @param debug a boolean (True if API should output debug infromation)
     */
    void setDebug(bool debug) {
        this->debug = debug;
    }

    /**
     * Toggles the CDATA integration flag
     * Set to true, if you want CDATA values to be converted to text
     *
     * @param noCdata a boolean
     */
    void setNoCdata(bool noCdata) {
        this->noCdata = noCdata;
    }

    /**
     * Set if XML should be created when sending requests.
     * This is slower but might give you feedback if XML is not valid before sending to server.
     *
     * @param createXML boolean if XML should created.
     */
    void setCreateXML(bool createXML) {
        this->createXML = createXML;
    }

    /**
     * Get flag value if XML should be created
     * @see setCreateXML(bool createXML)
     */
    bool getCreateXML() {
        return this->createXML;
    }

    /**
     * Set document root that corresponds to
     * root of documents being sent
     * @param documentRootXpath root tag name
     */
    void setDocumentRootXpath(const std::string &documentRootXpath = "document") {
        this->documentRootXpath = documentRootXpath;
    }

    /**
     * Returns document root tag name
     */
    std::string &getDocumentRootXpath() {
        return this->documentRootXpath;
    }

    /**
     * Sets document id xpath.
     * This is xpath to document primary key.
     * You have to specify the full path from the root, including the root tag, separated by slashes
     * @param documentIdXpath string as document ID Xpath
     */
    void setDocumentIdXpath(const std::string &documentIdXpath = "document/id") {
        this->documentIdXpath = documentIdXpath;
    }

    /**
     * Returns document ID xpath
     */
    std::string &getDocumentIdXpath() {
        return this->documentIdXpath;
    }

    /**
     * Sets corresponding socket timeouts in seconds
     * @param connectTimeout in seconds
     * @param sendTimeout in seconds
     * @param recieveTimeout in seconds
     */
    void setSocketTimeouts(int connectTimeout = 5, int sendTimeout = 30, int recieveTimeout = 60) {
    	if (!socket) return;
    	socket->connectTimeout = connectTimeout;
    	socket->sendTimeout = sendTimeout;
    	socket->recieveTimeout = recieveTimeout;
    }

    void clearTransactionId() {
    	this->transactionId = -1;
    }

private:
    std::string header(unsigned int length) {
        std::string res = "";
        res.push_back(0x09);
        res.push_back(0x09);
        res.push_back(0x00);
        res.push_back(0x00);
        res.push_back((length & 0x000000FF));
        res.push_back((length & 0x0000FF00) >> 8);
        res.push_back((length & 0x00FF0000) >> 16);
        res.push_back((length & 0xFF000000) >> 24);
        return res;
    }

private:
    std::string connectionString;
    ConnectionType connectionType; // SOCKET, TCP or HTTP
    std::string host; // Host to connect to
    int port; // Just for Socket connection type
    std::string url; // Just for HTTP connection type
    std::string storageName;
    std::string username;
    std::string password;
    std::string documentRootXpath;
    std::string documentIdXpath;
    std::map<std::string, std::string> customEnvelopeParams;

    std::string applicationId;
    bool debug;
    bool noCdata;
    bool createXML; /// Should actual XML tree be created when sending requests
    long long transactionId; /// TransactionId for current connection

    asio::io_service io_service;
    boost::shared_ptr<AbstractSocket> socket;

};
}

#endif //#ifndef CPS_CONNECTION_HPP
